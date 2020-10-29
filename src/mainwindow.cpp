#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>

#include "mainwindow.h"

#include <private/qzipreader_p.h>

#include "donatedialog.h"
#include "networkreplyhelper.h"
#include "scopedguard.h"
#include "settings.h"
#include "ui_mainwindow.h"

namespace
{
    // data from https://www.ip.cn/dns.html
    static std::map<QString, QString> chinaDNSServers = {
        {"114.114.114.114", "114 DNS"},
        {"114.114.115.115", "114 DNS"},
        {"223.5.5.5", "阿里 AliDNS"},
        {"223.6.6.6", "阿里 AliDNS"},
        {"180.76.76.76", "百度 BaiduDNS"},
        {"119.29.29.29", "DNSPod DNS+"},
        {"182.254.116.116", "DNSPod DNS+"},
        {"1.2.4.8", "CNNIC SDNS"},
        {"210.2.4.8", "CNNIC SDNS"},
    };

    static std::map<QString, QString> abroadDNSServers = {
        {"dns://208.67.222.222:443", "OpenDNS"},
        {"dns://208.67.222.222:5353", "OpenDNS"},
        {"dns://208.67.220.220:443", "OpenDNS"},
        {"dns://208.67.220.220:5353", "OpenDNS"},
        {"tcp://8.8.8.8", "Google DNS"},
        {"tcp://8.8.4.4", "Google DNS"},
        {"udp://8.8.8.8:443", "Google DNS"},
        {"udp://8.8.4.4:443", "Google DNS"},
        {"tls://1.1.1.1:853", "Cloudflare DNS"},
        {"tls://1.0.0.1:853", "Cloudflare DNS"},
        {"tls://8.8.8.8:853", "Google DNS"},
        {"tls://8.8.4.4:853", "Google DNS"},
        {"tls://9.9.9.9:853", "IBM Quad9"},
        {"tls://9.9.9.10:853", "IBM Quad9"},
        {"tls://145.100.185.15:853", "dnsovertls.sinodun.com"},
        {"tls://145.100.185.16:853", "dnsovertls1.sinodun.com"},
        {"tls://145.100.185.17:853", "dnsovertls2.sinodun.com"},
        {"tls://145.100.185.18:853", "dnsovertls3.sinodun.com"},
        {"tls://185.49.141.37:853", "getdnsapi.net"},
        {"tls://89.233.43.71:853", "unicast.censurfridns.dk"},
        {"tls://158.64.1.29:853", "kaitain.restena.lu"},
        {"tls://176.103.130.130:853", "dns.adguard.com"},
        {"tls://176.103.130.131:853", "dns.adguard.com"},
        {"tls://176.103.130.132:853", "dns-family.adguard.com"},
        {"tls://176.103.130.134:853", "dns-family.adguard.com"},
        {"tls://199.58.81.218:853", "dns.cmrg.net"},
        {"tls://51.15.70.167:853", "dns.larsdebruin.net"},
        {"tls://146.185.167.43:853", "dot.securedns.eu"},
        {"tls://81.187.221.24:853", "dns-tls.bitwiseshift.net"},
        {"tls://94.130.110.185:853", "ns1.dnsprivacy.at"},
        {"tls://94.130.110.178:853", "ns2.dnsprivacy.at"},
        {"tls://139.59.51.46:853", "dns.bitgeek.in"},
        {"tls://89.234.186.112:853", "dns.neutopia.org"},
        {"tls://93.177.65.183:853", "dot1.applied-privacy.net"},
        {"tls://184.105.193.78:853", "tls-dns-u.odvr.dns-oarc.net"},
        // ipv6
        {"tls://[2001:610:1:40ba:145:100:185:15]:853", "dnsovertls.sinodun.com"},
        {"tls://[2001:610:1:40ba:145:100:185:16]:853", "dnsovertls1.sinodun.com"},
        {"tls://[2a04:b900:0:100::38]:853", "getdnsapi.net"},
        {"tls://[2620:fe::fe]:853", "dns.quad9.net"},
        {"tls://[2620:fe::10]:853", "dns.quad9.net"},
        {"tls://[2606:4700:4700::1111]:853", "cloudflare-dns.com"},
        {"tls://[2606:4700:4700::1001]:853", "cloudflare-dns.com"},
        {"tls://[2001:4860:4860::8888]:853", "dns.google"},
        {"tls://[2001:4860:4860::8844]:853", "dns.google"},
        {"tls://[2a00:5a60::ad1:0ff]:853", "dns.adguard.com"},
        {"tls://[2a00:5a60::ad2:0ff]:853", "dns.adguard.com"},
        {"tls://[2a00:5a60::bad1:0ff]:853", "dns-family.adguard.com"},
        {"tls://[2a00:5a60::bad2:0ff]:853", "dns-family.adguard.com"},
        {"tls://[2a01:3a0:53:53::0]:853", "unicast.censurfridns.dk"},
        {"tls://[2001:a18:1::29]:853", "kaitain.restena.lu"},
        {"tls://[2001:610:1:40ba:145:100:185:18]:853", "dnsovertls3.sinodun.com"},
        {"tls://[2001:610:1:40ba:145:100:185:17]:853", "dnsovertls2.sinodun.com"},
        {"tls://[2001:470:1c:76d::53]:853", "dns.cmrg.net"},
        {"tls://[2a03:b0c0:0:1010::e9a:3001]:853", "dot.securedns.eu"},
        {"tls://[2001:8b0:24:24::24]:853", "dns-tls.bitwiseshift.net"},
        {"tls://[2a01:4f8:c0c:3c03::2]:853", "ns1.dnsprivacy.at"},
        {"tls://[2a01:4f8:c0c:3bfc::2]:853", "ns2.dnsprivacy.at"},
        {"tls://[2001:67c:27e4::35]:853", "privacydns.go6lab.si"},
        {"tls://[2a00:5884:8209::2]:853", "dns.neutopia.org"},
        {"tls://[2a03:4000:38:53c::2]:853", "dot1.applied-privacy.net"},
        {"tls://[2620:ff:c000:0:1::64:25]:853", "tls-dns-u.odvr.dns-oarc.net"},
    };
    // data from https://github.com/getdnsapi/stubby/blob/develop/stubby.yml.example
    static std::map<QString, QString> tlsNameMap = {
        // ipv4
        {"8.8.8.8", "dns.google"},
        {"8.8.4.4", "dns.google"},
        {"1.1.1.1", "cloudflare-dns.com"},
        {"1.0.0.1", "cloudflare-dns.com"},
        {"9.9.9.9", "dns.quad9.net"},
        {"9.9.9.10", "dns.quad9.net"},
        {"145.100.185.15", "dnsovertls.sinodun.com"},
        {"145.100.185.16", "dnsovertls1.sinodun.com"},
        {"145.100.185.17", "dnsovertls2.sinodun.com"},
        {"145.100.185.18", "dnsovertls3.sinodun.com"},
        {"185.49.141.37", "getdnsapi.net"},
        {"89.233.43.71", "unicast.censurfridns.dk"},
        {"158.64.1.29", "kaitain.restena.lu"},
        {"176.103.130.130", "dns.adguard.com"},
        {"176.103.130.131", "dns.adguard.com"},
        {"176.103.130.132", "dns-family.adguard.com"},
        {"176.103.130.134", "dns-family.adguard.com"},
        {"199.58.81.218", "dns.cmrg.net"},
        {"51.15.70.167", "dns.larsdebruin.net"},
        {"146.185.167.43", "dot.securedns.eu"},
        {"81.187.221.24", "dns-tls.bitwiseshift.net"},
        {"94.130.110.185", "ns1.dnsprivacy.at"},
        {"94.130.110.178", "ns2.dnsprivacy.at"},
        {"139.59.51.46", "dns.bitgeek.in"},
        {"89.234.186.112", "dns.neutopia.org"},
        {"93.177.65.183", "dot1.applied-privacy.net"},
        {"184.105.193.78", "tls-dns-u.odvr.dns-oarc.net"},
        // ipv6
        {"2001:610:1:40ba:145:100:185:15", "dnsovertls.sinodun.com"},
        {"2001:610:1:40ba:145:100:185:16", "dnsovertls1.sinodun.com"},
        {"2a04:b900:0:100::38", "getdnsapi.net"},
        {"2620:fe::fe", "dns.quad9.net"},
        {"2620:fe::10", "dns.quad9.net"},
        {"2606:4700:4700::1111", "cloudflare-dns.com"},
        {"2606:4700:4700::1001", "cloudflare-dns.com"},
        {"2001:4860:4860::8888", "dns.google"},
        {"2001:4860:4860::8844", "dns.google"},
        {"2a00:5a60::ad1:0ff", "dns.adguard.com"},
        {"2a00:5a60::ad2:0ff", "dns.adguard.com"},
        {"2a00:5a60::bad1:0ff", "dns-family.adguard.com"},
        {"2a00:5a60::bad2:0ff", "dns-family.adguard.com"},
        {"2a01:3a0:53:53::0", "unicast.censurfridns.dk"},
        {"2001:a18:1::29", "kaitain.restena.lu"},
        {"2001:610:1:40ba:145:100:185:18", "dnsovertls3.sinodun.com"},
        {"2001:610:1:40ba:145:100:185:17", "dnsovertls2.sinodun.com"},
        {"2001:470:1c:76d::53", "dns.cmrg.net"},
        {"2a03:b0c0:0:1010::e9a:3001", "dot.securedns.eu"},
        {"2001:8b0:24:24::24", "dns-tls.bitwiseshift.net"},
        {"2a01:4f8:c0c:3c03::2", "ns1.dnsprivacy.at"},
        {"2a01:4f8:c0c:3bfc::2", "ns2.dnsprivacy.at"},
        {"2001:67c:27e4::35", "privacydns.go6lab.si"},
        {"2a00:5884:8209::2", "dns.neutopia.org"},
        {"2a03:4000:38:53c::2", "dot1.applied-privacy.net"},
        {"2620:ff:c000:0:1::64:25", "tls-dns-u.odvr.dns-oarc.net"},
    };
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/corednshome.png"));
    ui->edtChinaDNSServers->setText(g_settings->customChinaDNSServerList());
    ui->edtAbroadDNSServers->setText(g_settings->customAbroadDNSServerList());
    ui->cbAdsPlugin->setChecked(g_settings->adsEnabled());
    ui->cbAdsListStore->setChecked(g_settings->adsCacheStore());
    ui->rbDefaultAdsPolicy->setChecked(g_settings->defaultAdsPolicy());
    ui->rbStrictAdsPolicy->setChecked(!g_settings->defaultAdsPolicy());
    ui->edtAdsBlackList->setText(g_settings->customAdsBlackList());
    ui->edtAdsWhiteList->setText(g_settings->customAdsWhiteList());
    ui->edtAdsAutoUpdateInterval->setText(g_settings->adsAutoUpdateInterval());
    ui->cbHealthCheck->setChecked(g_settings->healthCheckEnabled());
    ui->cbConfigurationHotReload->setChecked(g_settings->hotReloadEnabled());
    ui->cbLogs->setChecked(g_settings->logsEnabled());
    ui->cbSystemHostFile->setChecked(g_settings->hostEnabled());
    ui->cbTTLCache->setChecked(g_settings->ttlCacheEnabled());
    ui->cbBogusPlugin->setChecked(g_settings->bogusEnabled());
    ui->cbResolveAppleDomainByChinaDNS->setChecked(g_settings->exceptAppleDomain());
    ui->cbResolveGoogleDomainByChinaDNS->setChecked(g_settings->exceptGoogleDomain());
    ui->edtRedis->setText(g_settings->redis());
    ui->edtListenAddress->setText(g_settings->listenAddress());
    ui->actionStartCoreDNS->setEnabled(!m_isCoreDNSStarted);
    ui->actionStopCoreDNS->setEnabled(m_isCoreDNSStarted);

    // fill China DNS server list
    auto servers = g_settings->chinaDNSServerList();
    for (const auto &[ip, tooltip] : chinaDNSServers)
    {
        auto *item = new QListWidgetItem(ip, ui->listChinaDNSServers);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setToolTip(tooltip);
        ui->listChinaDNSServers->addItem(item);
        if (servers.contains(ip))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
    }

    // fill abroad DNS server list
    servers = g_settings->abroadDNSServerList();
    for (const auto &[ip, tooltip] : abroadDNSServers)
    {
        auto *item = new QListWidgetItem(ip, ui->listAbroadDNSServers);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setToolTip(tooltip);
        ui->listAbroadDNSServers->addItem(item);
        if (servers.contains(ip))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
    }

    updateBogusList();
    updateAppleDomainList();
    updateGoogleDomainList();
    updateChinaDomainList();

    QString path   = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    m_corefilePath = path + "/Corefile";
    QDir d(path);
    if (!d.exists())
        d.mkpath(path);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &MainWindow::onAboutToQuit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startCoreDNS()
{
    exportCorefile(m_corefilePath);

    m_isCoreDNSStarted = true;
    ui->actionStartCoreDNS->setEnabled(!m_isCoreDNSStarted);
    ui->actionStopCoreDNS->setEnabled(m_isCoreDNSStarted);
    if (m_coredns)
        m_coredns->kill();
    if (!m_coredns)
        m_coredns = new QProcess;
    connect(m_coredns, &QProcess::readyReadStandardOutput, this, &MainWindow::onReadyReadStandardOutput);
    connect(m_coredns, &QProcess::readyReadStandardError, this, &MainWindow::onReadyReadStandardError);
    connect(m_coredns, &QProcess::errorOccurred, this, &MainWindow::onErrorOccurred);
    connect(m_coredns, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &MainWindow::onFinished);
    connect(m_coredns, &QProcess::started, this, &MainWindow::onStarted);
    connect(m_coredns, &QProcess::stateChanged, this, &MainWindow::onStateChanged);
    QString corednsPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) +
#ifdef Q_OS_WIN
                          "/coredns.exe";
#else
                          "/coredns";
#endif
    if (!QFile::exists(corednsPath))
        corednsPath = QCoreApplication::applicationDirPath() +
#ifdef Q_OS_WIN
                      "/coredns.exe";
#else
                      "/coredns";
#endif
    m_coredns->setProgram(corednsPath);
    m_coredns->setArguments(QStringList() << "-conf" << QDir::toNativeSeparators(m_corefilePath));
    m_coredns->setWorkingDirectory(QCoreApplication::applicationDirPath());
    m_coredns->start();
}

void MainWindow::stopCoreDNS()
{
    m_isCoreDNSStarted = false;
    ui->actionStartCoreDNS->setEnabled(!m_isCoreDNSStarted);
    ui->actionStopCoreDNS->setEnabled(m_isCoreDNSStarted);
    if (m_coredns)
        m_coredns->kill();
}

void MainWindow::onAboutToQuit()
{
    stopCoreDNS();
}

void MainWindow::on_actionExportCorefile_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export Corefile"));
    if (filePath.isEmpty())
        return;
    if (exportCorefile(filePath))
    {
        QMessageBox::information(
            this, tr("Corefile exported"), tr("Configurations have been exported to %1.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(
            this, tr("Exporting failed"), tr("Exporting configurations to %1 failed.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
    }
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionHomepage_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/missdeer/corednshome"));
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,
                       tr("About CoreDNS Home"),
                       tr("GUI configuration tool for <a href=\"https://github.com/missdeer/coredns_custom_build\">custom build CoreDNS</a>.<br>"
                          "Homepage: <a href=\"https://github.com/missdeer/corednshome\">https://github.com/missdeer/corednshome</a>"));
}

void MainWindow::on_actionStartCoreDNS_triggered()
{
    startCoreDNS();
}

void MainWindow::on_actionStopCoreDNS_triggered()
{
    stopCoreDNS();
}

void MainWindow::onChinaDomainRequestFinished()
{
    auto *reply = qobject_cast<NetworkReplyHelper *>(sender());
    Q_ASSERT(reply);
    reply->deleteLater();

    QByteArray &content = reply->content();
    auto        list    = content.split('\n');
    QStringList res;
    res.reserve(list.size());
    for (const auto &line : list)
    {
        auto l = line.split('/');
        if (l.size() > 1)
            res.append(l.at(1));
    }
    Q_ASSERT(g_settings);
    g_settings->setChinaDomainList(res.join(' '));
}

void MainWindow::onGoogleDomainRequestFinished()
{
    auto *reply = qobject_cast<NetworkReplyHelper *>(sender());
    Q_ASSERT(reply);
    reply->deleteLater();

    QByteArray &content = reply->content();
    auto        list    = content.split('\n');
    QStringList res;
    res.reserve(list.size());
    for (const auto &line : list)
    {
        auto l = line.split('/');
        if (l.size() > 1)
            res.append(l.at(1));
    }
    Q_ASSERT(g_settings);
    g_settings->setGoogleDomainList(res.join(' '));
}

void MainWindow::onAppleDomainRequestFinished()
{
    auto *reply = qobject_cast<NetworkReplyHelper *>(sender());
    Q_ASSERT(reply);
    reply->deleteLater();

    QByteArray &content = reply->content();
    auto        list    = content.split('\n');
    QStringList res;
    res.reserve(list.size());
    for (const auto &line : list)
    {
        auto l = line.split('/');
        if (l.size() > 1)
            res.append(l.at(1));
    }
    Q_ASSERT(g_settings);
    g_settings->setAppleDomainList(res.join(' '));
}

void MainWindow::onBogusListRequestFinished()
{
    auto *reply = qobject_cast<NetworkReplyHelper *>(sender());
    Q_ASSERT(reply);
    reply->deleteLater();

    QByteArray &content = reply->content();
    auto        list    = content.split('\n');
    QStringList res;
    res.reserve(list.size());
    const QByteArray leading = "bogus-nxdomain=";
    for (const auto &line : list)
    {
        if (line.startsWith(leading))
            res.append(line.mid(leading.size()));
    }
    Q_ASSERT(g_settings);
    g_settings->setBogusIPList(res.join(' '));
}

void MainWindow::onReadyReadStandardOutput()
{
    auto output = m_coredns->readAllStandardOutput();
    ui->edtLogs->append(QString(output));
}

void MainWindow::onReadyReadStandardError()
{
    auto err = m_coredns->readAllStandardError();
    ui->edtLogs->append(QString(err));
}

void MainWindow::onErrorOccurred(QProcess::ProcessError error)
{
    ui->edtLogs->append(QString("CoreDNS process error occurred:%1").arg(error));
}

void MainWindow::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->edtLogs->append(QString("CoreDNS process finished with exit code:%1, status:%2").arg(exitCode).arg(exitStatus));
}

void MainWindow::onStarted()
{
    ui->edtLogs->append("CoreDNS started.");
}

void MainWindow::onStateChanged(QProcess::ProcessState newState)
{
    ui->edtLogs->append(QString("CoreDNS process state changed:%1").arg(newState));
    if (newState == QProcess::NotRunning)
        stopCoreDNS();
}

void MainWindow::updateChinaDomainList()
{
    if (!QSslSocket::supportsSsl())
    {
        return;
    }
    QUrl            u("https://cdn.jsdelivr.net/gh/felixonmars/dnsmasq-china-list@master/accelerated-domains.china.conf");
    QNetworkRequest request(u);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.12; rv:55.0) "
                      "Gecko/20100101 Firefox/55.0");
    request.setRawHeader("Accept-Encoding", "gzip, deflate");
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    auto *reply       = m_nam.get(request);
    auto *replyHelper = new NetworkReplyHelper(reply);
    connect(replyHelper, SIGNAL(done()), this, SLOT(onChinaDomainRequestFinished()));
}

void MainWindow::updateAppleDomainList()
{
    if (!QSslSocket::supportsSsl())
    {
        return;
    }

    QUrl            u("https://cdn.jsdelivr.net/gh/felixonmars/dnsmasq-china-list@master/apple.china.conf");
    QNetworkRequest request(u);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.12; rv:55.0) "
                      "Gecko/20100101 Firefox/55.0");
    request.setRawHeader("Accept-Encoding", "gzip, deflate");
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    auto *reply       = m_nam.get(request);
    auto *replyHelper = new NetworkReplyHelper(reply);
    connect(replyHelper, SIGNAL(done()), this, SLOT(onAppleDomainRequestFinished()));
}

void MainWindow::updateGoogleDomainList()
{
    if (!QSslSocket::supportsSsl())
    {
        return;
    }
    QUrl            u("https://cdn.jsdelivr.net/gh/felixonmars/dnsmasq-china-list@master/google.china.conf");
    QNetworkRequest request(u);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.12; rv:55.0) "
                      "Gecko/20100101 Firefox/55.0");
    request.setRawHeader("Accept-Encoding", "gzip, deflate");
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    auto *reply       = m_nam.get(request);
    auto *replyHelper = new NetworkReplyHelper(reply);
    connect(replyHelper, SIGNAL(done()), this, SLOT(onGoogleDomainRequestFinished()));
}

void MainWindow::updateBogusList()
{
    if (!QSslSocket::supportsSsl())
    {
        return;
    }
    QUrl            u("https://cdn.jsdelivr.net/gh/felixonmars/dnsmasq-china-list@master/bogus-nxdomain.china.conf");
    QNetworkRequest request(u);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.12; rv:55.0) "
                      "Gecko/20100101 Firefox/55.0");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Referer", "https://godbolt.org/");
    request.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
    request.setRawHeader("Accept-Encoding", "gzip, deflate");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    auto *reply       = m_nam.get(request);
    auto *replyHelper = new NetworkReplyHelper(reply);
    connect(replyHelper, SIGNAL(done()), this, SLOT(onBogusListRequestFinished()));
}

bool MainWindow::exportCorefile(const QString &corefile)
{
    QFile f(corefile);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QString bindAddress = g_settings->listenAddress();
        auto    ba          = bindAddress.split(":");
        if (ba.size() != 2)
        {
            ba.clear();
            ba << ""
               << "53";
        }
        bindAddress                   = ba[0].isEmpty() ? "" : "\tbind " + ba[0];
        QString port                  = ":" + ba[1];
        QString adsPolicy             = g_settings->defaultAdsPolicy() ? "default-lists" : "strict-default-lists";
        QString adsBlacklist          = g_settings->customAdsBlackList().isEmpty() ? "" : "blacklist " + g_settings->customAdsBlackList();
        QString adsWhitelist          = g_settings->customAdsWhiteList().isEmpty() ? "" : "whitelist " + g_settings->customAdsWhiteList();
        QString adsAutoUpdateInterval = QString("auto-update-interval %1").arg(g_settings->adsAutoUpdateInterval());
        QString adsListStore          = g_settings->adsCacheStore() ? "list-store ads-cache" : "";
        QString ads                   = g_settings->adsEnabled() ? QString("ads {\n\t\t%1\n\t\t%2\n\t\t%3\n\t\t%4\n\t\t%5\n\t\tlog\n\t}")
                                                     .arg(adsPolicy, adsBlacklist, adsWhitelist, adsAutoUpdateInterval, adsListStore)
                                               : "";
        QString     host                = g_settings->hostEnabled() ? "hosts {\n\t\tfallthrough\n\t}" : "";
        QStringList abroadDNSServerList = g_settings->abroadDNSServerList();
        abroadDNSServerList.append(g_settings->customAbroadDNSServerList().split(' '));
        QStringList tlsDNSServerList, udpDNSServerList, tcpDNSServerList;
        for (auto &server : abroadDNSServerList)
        {
            if (server.startsWith("udp://"))
            {
                // check if tcp exists
                QString s     = server.replace("udp://", "tcp://");
                int     index = abroadDNSServerList.indexOf(s);
                if (index >= 0)
                {
                    server = s.mid(6);
                    abroadDNSServerList.replace(index, server);
                    continue;
                }
            }
            if (server.startsWith("udp://"))
            {
                udpDNSServerList.append(server);
                server = "127.0.0.1:531" + QString::number(udpDNSServerList.size());
                continue;
            }
            if (server.startsWith("tcp://"))
            {
                tcpDNSServerList.append(server);
                server = "127.0.0.1:532" + QString::number(tcpDNSServerList.size());
                continue;
            }
            if (server.startsWith("tls://"))
            {
                tlsDNSServerList.append(server);
                server = "127.0.0.1:533" + QString::number(tlsDNSServerList.size());
                continue;
            }
        }
        abroadDNSServerList.removeDuplicates();
        QString forwardAbroadDomains = QString("forward . %1 {\n\t\texcept %2 %3 %4\n\t}")
                                           .arg(abroadDNSServerList.join(' ').remove("udp://").remove("dns://"),
                                                g_settings->chinaDomainList(),
                                                (g_settings->exceptGoogleDomain() ? g_settings->googleDomainList() : ""),
                                                (g_settings->exceptAppleDomain() ? g_settings->appleDomainList() : ""));
        QString proxyChinaDomains =
            QString("proxy . %1 %2")
                .arg(g_settings->chinaDNSServerList().join(' ').remove("udp://").remove("dns://"), g_settings->customChinaDNSServerList())
                .remove("udp://")
                .remove("dns://")
                .remove("tcp://");
        QString bogus  = g_settings->bogusIPList().isEmpty() ? "" : "bogus " + g_settings->bogusIPList();
        QString redis  = g_settings->redis().isEmpty() ? "" : QString("redisc {\n\t\tendpoint %1\n\t}").arg(g_settings->redis());
        QString log    = g_settings->logsEnabled() ? "log" : "";
        QString cache  = g_settings->ttlCacheEnabled() ? "cache" : "";
        QString health = g_settings->healthCheckEnabled() ? "health" : "";
        QString reload = g_settings->hotReloadEnabled() ? "reload" : "";
        QString other  = QString("%1\n\t%2\n\t%3\n\t%4").arg(log, cache, health, reload);
        QString total  = QString(".%1{\n\t%2\n\t%3\n\t%4\n\t%5\n\t%6\n\t%7\n\t%8\n\t%9\n}\n")
                            .arg(port, bindAddress, ads, host, forwardAbroadDomains, proxyChinaDomains, bogus, redis, other);

        f.write(total.toUtf8());
        QRegularExpression regexIPv4("tls:\\/\\/(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}):\\d{1,5}");
        QRegularExpression regexIPv6("tls:\\/\\/\\[([a-z0-9:]+)\\]:\\d{1,5}");
        for (int i = 0; i < udpDNSServerList.size(); i++)
        {
            auto    server = udpDNSServerList.at(i);
            QString s =
                QString(".:531%1 {\n\tbind 127.0.0.1\n\tforward . %2 {\n\t\tprefer_udp\n\t}\n\tcache\n}\n").arg(i + 1).arg(server.remove("udp://"));
            f.write(s.toUtf8());
        }
        for (int i = 0; i < tcpDNSServerList.size(); i++)
        {
            auto    server = tcpDNSServerList.at(i);
            QString s =
                QString(".:532%1 {\n\tbind 127.0.0.1\n\tforward . %2 {\n\t\tforce_tcp\n\t}\n\tcache\n}\n").arg(i + 1).arg(server.remove("tcp://"));
            f.write(s.toUtf8());
        }
        for (int i = 0; i < tlsDNSServerList.size(); i++)
        {
            auto    server = tlsDNSServerList.at(i);
            QString ipAddr;
            auto    match = regexIPv6.match(server);
            if (match.hasMatch())
                ipAddr = match.captured(1);
            else
            {
                match = regexIPv4.match(server);
                if (match.hasMatch())
                    ipAddr = match.captured(1);
                else
                    continue;
            }
            QString s = QString(".:533%1 {\n\tbind 127.0.0.1\n\tforward . %2 {\n\t\ttls_servername %3\n\t}\n\tcache\n}\n")
                            .arg(i + 1)
                            .arg(server)
                            .arg(tlsNameMap[ipAddr]);
            f.write(s.toUtf8());
        }

        f.close();
        return true;
    }
    return false;
}

void MainWindow::on_edtChinaDNSServers_textChanged(const QString &text)
{
    Q_ASSERT(g_settings);
    g_settings->setCustomChinaDNSServerList(text);
}

void MainWindow::on_edtAbroadDNSServers_textChanged(const QString &text)
{
    Q_ASSERT(g_settings);
    g_settings->setCustomAbroadDNSServerList(text);
}

void MainWindow::on_edtAdsBlackList_textChanged(const QString &text)
{
    Q_ASSERT(g_settings);
    g_settings->setCustomAdsBlackList(text);
}

void MainWindow::on_edtAdsWhiteList_textChanged(const QString &text)
{
    Q_ASSERT(g_settings);
    g_settings->setCustomAdsWhiteList(text);
}

void MainWindow::on_edtAdsAutoUpdateInterval_textChanged(const QString &text)
{
    Q_ASSERT(g_settings);
    g_settings->setAdsAutoUpdateInterval(text);
}

void MainWindow::on_edtListenAddress_textChanged(const QString &text)
{
    Q_ASSERT(g_settings);
    g_settings->setListenAddress(text);
}

void MainWindow::on_edtRedis_textChanged(const QString &text)
{
    Q_ASSERT(g_settings);
    g_settings->setRedis(text);
}

void MainWindow::on_cbAdsPlugin_stateChanged(int state)
{
    Q_ASSERT(g_settings);
    g_settings->setAdsEnabled(state == Qt::Checked);
}

void MainWindow::on_cbAdsListStore_stateChanged(int state)
{
    Q_ASSERT(g_settings);
    g_settings->setAdsCacheStore(state == Qt::Checked);
}

void MainWindow::on_cbHealthCheck_stateChanged(int state)
{
    Q_ASSERT(g_settings);
    g_settings->setHealthCheckEnabled(state == Qt::Checked);
}

void MainWindow::on_cbConfigurationHotReload_stateChanged(int state)
{
    Q_ASSERT(g_settings);
    g_settings->setHotReloadEnabled(state == Qt::Checked);
}

void MainWindow::on_cbLogs_stateChanged(int state)
{
    Q_ASSERT(g_settings);
    g_settings->setLogsEnabled(state == Qt::Checked);
}

void MainWindow::on_cbSystemHostFile_stateChanged(int state)
{
    Q_ASSERT(g_settings);
    g_settings->setHostEnabled(state == Qt::Checked);
}

void MainWindow::on_cbTTLCache_stateChanged(int state)
{
    Q_ASSERT(g_settings);
    g_settings->setTtlCacheEnabled(state == Qt::Checked);
}

void MainWindow::on_cbBogusPlugin_stateChanged(int state)
{
    Q_ASSERT(g_settings);
    g_settings->setBogusEnabled(state == Qt::Checked);
}

void MainWindow::on_rbDefaultAdsPolicy_toggled(bool checked)
{
    Q_ASSERT(g_settings);
    g_settings->setDefaultAdsPolicy(checked);
}

void MainWindow::on_listChinaDNSServers_itemChanged(QListWidgetItem *item)
{
    QString text    = item->text();
    bool    checked = (item->checkState() == Qt::Checked);

    Q_ASSERT(g_settings);
    if (checked)
        g_settings->addChinaDNSServer(text);
    else
        g_settings->removeChinaDNSServer(text);
}

void MainWindow::on_listAbroadDNSServers_itemChanged(QListWidgetItem *item)
{
    QString text    = item->text();
    bool    checked = (item->checkState() == Qt::Checked);

    Q_ASSERT(g_settings);
    if (checked)
        g_settings->addAbroadDNSServer(text);
    else
        g_settings->removeAbroadDNSServer(text);
}

void MainWindow::on_cbResolveAppleDomainByChinaDNS_stateChanged(int state)
{
    Q_ASSERT(g_settings);
    g_settings->setExceptAppleDomain(state == Qt::Checked);
}

void MainWindow::on_cbResolveGoogleDomainByChinaDNS_stateChanged(int state)
{
    Q_ASSERT(g_settings);
    g_settings->setExceptGoogleDomain(state == Qt::Checked);
}

void MainWindow::on_actionUpdateCoreDNSBinary_triggered()
{
    if (!QSslSocket::supportsSsl())
    {
        QMessageBox::critical(
            nullptr, QObject::tr("Critical error"), QObject::tr("SSL not supported, cannot update CoreDNS binary."), QMessageBox::Ok);
        return;
    }
    ui->actionUpdateCoreDNSBinary->setEnabled(false);
    bool isBuild64Bit = QSysInfo::buildCpuArchitecture().contains(QLatin1String("64"));
#if defined(Q_OS_WIN)
    QUrl u(isBuild64Bit ? "https://coredns.minidump.info/dl/coredns-windows-amd64.zip" : "https://coredns.minidump.info/dl/coredns-windows-386.zip");
#elif defined(Q_OS_MAC)
    Q_UNUSED(isBuild64Bit);
    QUrl u("https://coredns.minidump.info/dl/coredns-darwin-amd64.zip");
#elif defined(Q_OS_LINUX)
    QUrl u(isBuild64Bit ? "https://coredns.minidump.info/dl/coredns-linux-amd64.zip" : "https://coredns.minidump.info/dl/coredns-linux-386.zip");
#elif defined(Q_OS_FREEBSD)
    QUrl u(isBuild64Bit ? "https://coredns.minidump.info/dl/coredns-freebsd-amd64.zip" : "https://coredns.minidump.info/dl/coredns-freebsd-386.zip");
#elif defined(Q_OS_NETBSD)
    QUrl u(isBuild64Bit ? "https://coredns.minidump.info/dl/coredns-netbsd-amd64.zip" : "https://coredns.minidump.info/dl/coredns-netbsd-386.zip");
#elif defined(Q_OS_OPENBSD)
    QUrl u(isBuild64Bit ? "https://coredns.minidump.info/dl/coredns-openbsd-amd64.zip" : "https://coredns.minidump.info/dl/coredns-openbsd-386.zip");
#elif defined(Q_OS_SOLARIS)
    QUrl u("https://coredns.minidump.info/dl/coredns-solaris-amd64.zip");
#else
#endif

    QNetworkRequest request(u);
    request.setHeader(QNetworkRequest::UserAgentHeader, "CoreDNSHome");
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    // save as coredns.7z then uncompress it
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir    d(path);
    if (!d.exists())
        d.mkpath(path);
    QString pkgPath = path + "/coredns.zip";
    auto *  f       = new QFile(pkgPath);
    if (!f->open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QMessageBox::warning(this, tr("Error"), tr("Saving package to %1 failed.").arg(QDir::toNativeSeparators(pkgPath)), QMessageBox::Ok);
        ui->actionUpdateCoreDNSBinary->setEnabled(true);
        return;
    }

    auto *r           = m_nam.get(request);
    auto *replyHelper = new NetworkReplyHelper(r, f);
    connect(replyHelper, SIGNAL(done()), this, SLOT(onArtifactRequestFinished()));
}

void MainWindow::onArtifactRequestFinished()
{
    auto *reply = qobject_cast<NetworkReplyHelper *>(sender());
    Q_ASSERT(reply);
    reply->deleteLater();

    auto *f = qobject_cast<QFile *>(reply->storage());
    Q_ASSERT(f);
    f->close();
    f->deleteLater();

    ScopedGuard sg([this] { ui->actionUpdateCoreDNSBinary->setEnabled(true); });

    auto       pkgPath = f->fileName();
    QZipReader pkgReader(pkgPath);
    auto       path         = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    auto       fileNameList = pkgReader.fileInfoList();
    bool       gotBinary    = false;
    for (auto &fi : fileNameList)
    {
#if defined(Q_OS_WIN)
        if (fi.filePath.endsWith("coredns.exe"))
        {
            QFile f(path + "/coredns.exe");
#else
        if (fi.filePath.endsWith("coredns"))
        {
            QFile f(path + "/coredns");
#endif
            if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                QMessageBox::warning(
                    this, tr("Error"), tr("Saving CoreDNS binary to %1 failed.").arg(QDir::toNativeSeparators(path)), QMessageBox::Ok);
                continue;
            }
            QByteArray ba = pkgReader.fileData(fi.filePath);
            f.write(ba);
            f.close();
            gotBinary = true;
        }
    }

    if (pkgReader.isReadable())
        pkgReader.close();
    QFile::remove(pkgPath);

    if (gotBinary)
        QMessageBox::information(this, tr("Notice"), tr("Updating CoreDNS binary finished."), QMessageBox::Ok);
    else
        QMessageBox::warning(this, tr("Error"), tr("Updating CoreDNS binary failed."), QMessageBox::Ok);
}

void MainWindow::on_actionDonateViaPaypal_triggered()
{
    QDesktopServices::openUrl(QUrl("https://www.paypal.me/dfordsoft/"));
}

void MainWindow::on_actionDonateViaAlipayWechat_triggered()
{
    DonateDialog dlg(this);
    dlg.exec();
}
