#include <QCoreApplication>
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>

#include "mainwindow.h"

#include "donatedialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionStartCoreDNS->setEnabled(!m_isCoreDNSStarted);
    ui->actionStopCoreDNS->setEnabled(m_isCoreDNSStarted);
    setWindowIcon(QIcon(":/corednsgui.png"));
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
