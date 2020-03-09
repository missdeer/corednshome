#include <QCoreApplication>
#include <QDesktopServices>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QUrl>

#include "mainwindow.h"

#include "donatedialog.h"
#include "networkreplyhelper.h"
#include "settings.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/corednsgui.png"));
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
    ui->edtRedis->setText(g_settings->redis());
    ui->edtListenAddress->setText(g_settings->listenAddress());
    ui->actionStartCoreDNS->setEnabled(!m_isCoreDNSStarted);
    ui->actionStopCoreDNS->setEnabled(m_isCoreDNSStarted);

    auto servers = g_settings->chinaDNSServerList();
    for (const auto &server : servers)
    {
        auto items = ui->listChinaDNSServers->findItems(server, Qt::MatchFixedString);
        for (auto item : items)
            item->setCheckState(Qt::Checked);
    }
    servers = g_settings->abroadDNSServerList();
    for (const auto &server : servers)
    {
        auto items = ui->listAbroadDNSServers->findItems(server, Qt::MatchFixedString);
        for (auto item : items)
            item->setCheckState(Qt::Checked);
    }

    updateBogusList();
    updateAppleDomainList();
    updateGoogleDomainList();
    updateChinaDomainList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startCoreDNS()
{
    if (!m_isCoreDNSStarted)
    {
        m_isCoreDNSStarted = true;
        ui->actionStartCoreDNS->setEnabled(!m_isCoreDNSStarted);
        ui->actionStopCoreDNS->setEnabled(m_isCoreDNSStarted);
    }
}

void MainWindow::stopCoreDNS()
{
    if (m_isCoreDNSStarted)
    {
        m_isCoreDNSStarted = false;
        ui->actionStartCoreDNS->setEnabled(!m_isCoreDNSStarted);
        ui->actionStopCoreDNS->setEnabled(m_isCoreDNSStarted);
    }
}

void MainWindow::on_actionExportCorefile_triggered() {}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionHomepage_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/missdeer/corednsgui"));
}

void MainWindow::on_actionDonate_triggered()
{
    DonateDialog dlg(this);
    dlg.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,
                       tr("About CoreDNS GUI"),
                       tr("GUI configuration tool for custom build CoreDNS.<br>"
                          "<a href=\"https://github.com/missdeer/corednsgui\">https://github.com/missdeer/corednsgui</a>"));
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
    m_chinaDomainList = res.join(' ');
}

void MainWindow::onGoogleDomainRequestFinished()
{
    auto *reply = qobject_cast<NetworkReplyHelper *>(sender());
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
    m_googleDomainList = res.join(' ');
}

void MainWindow::onAppleDomainRequestFinished()
{
    auto *reply = qobject_cast<NetworkReplyHelper *>(sender());
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
    m_appleDomainList = res.join(' ');
}

void MainWindow::onBogusListRequestFinished()
{
    auto *reply = qobject_cast<NetworkReplyHelper *>(sender());
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
    m_bogusList = res.join(' ');
}

void MainWindow::updateChinaDomainList()
{
    QUrl            u("https://cdn.jsdelivr.net/gh/felixonmars/dnsmasq-china-list@master/accelerated-domains.china.conf");
    QNetworkRequest request(u);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.12; rv:55.0) "
                      "Gecko/20100101 Firefox/55.0");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Referer", "https://godbolt.org/");
    request.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
    request.setRawHeader("Accept-Encoding", "gzip, deflate");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, QVariant(true));

    auto *reply       = m_nam.get(request);
    auto *replyHelper = new NetworkReplyHelper(reply);
    replyHelper->setTimeout(30000);
    connect(replyHelper, SIGNAL(done()), this, SLOT(onChinaDomainRequestFinished()));
}

void MainWindow::updateAppleDomainList()
{
    QUrl            u("https://cdn.jsdelivr.net/gh/felixonmars/dnsmasq-china-list@master/apple.china.conf");
    QNetworkRequest request(u);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.12; rv:55.0) "
                      "Gecko/20100101 Firefox/55.0");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Referer", "https://godbolt.org/");
    request.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
    request.setRawHeader("Accept-Encoding", "gzip, deflate");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, QVariant(true));

    auto *reply       = m_nam.get(request);
    auto *replyHelper = new NetworkReplyHelper(reply);
    replyHelper->setTimeout(30000);
    connect(replyHelper, SIGNAL(done()), this, SLOT(onAppleDomainRequestFinished()));
}

void MainWindow::updateGoogleDomainList()
{
    QUrl            u("https://cdn.jsdelivr.net/gh/felixonmars/dnsmasq-china-list@master/google.china.conf");
    QNetworkRequest request(u);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.12; rv:55.0) "
                      "Gecko/20100101 Firefox/55.0");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Referer", "https://godbolt.org/");
    request.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
    request.setRawHeader("Accept-Encoding", "gzip, deflate");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, QVariant(true));

    auto *reply       = m_nam.get(request);
    auto *replyHelper = new NetworkReplyHelper(reply);
    replyHelper->setTimeout(30000);
    connect(replyHelper, SIGNAL(done()), this, SLOT(onGoogleDomainRequestFinished()));
}

void MainWindow::updateBogusList()
{
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
    request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, QVariant(true));

    auto *reply       = m_nam.get(request);
    auto *replyHelper = new NetworkReplyHelper(reply);
    replyHelper->setTimeout(30000);
    connect(replyHelper, SIGNAL(done()), this, SLOT(onBogusListRequestFinished()));
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
