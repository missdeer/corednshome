#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QListWidgetItem;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void startCoreDNS();
    void stopCoreDNS();
private slots:
    void onInfoRequestFinished();
    void onArtifactRequestFinished();
    void onAboutToQuit();
    void onChinaDomainRequestFinished();
    void onGoogleDomainRequestFinished();
    void onAppleDomainRequestFinished();
    void onBogusListRequestFinished();
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onErrorOccurred(QProcess::ProcessError error);
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onStarted();
    void onStateChanged(QProcess::ProcessState newState);

    void on_actionExportCorefile_triggered();

    void on_actionExit_triggered();

    void on_actionHomepage_triggered();

    void on_actionDonate_triggered();

    void on_actionAbout_triggered();

    void on_actionStartCoreDNS_triggered();

    void on_actionStopCoreDNS_triggered();

    void on_edtChinaDNSServers_textChanged(const QString &text);

    void on_edtAbroadDNSServers_textChanged(const QString &text);

    void on_edtAdsBlackList_textChanged(const QString &text);

    void on_edtAdsWhiteList_textChanged(const QString &text);

    void on_edtAdsAutoUpdateInterval_textChanged(const QString &text);

    void on_edtListenAddress_textChanged(const QString &text);

    void on_edtRedis_textChanged(const QString &text);

    void on_cbAdsPlugin_stateChanged(int state);

    void on_cbAdsListStore_stateChanged(int state);

    void on_cbHealthCheck_stateChanged(int state);

    void on_cbConfigurationHotReload_stateChanged(int state);

    void on_cbLogs_stateChanged(int state);

    void on_cbSystemHostFile_stateChanged(int state);

    void on_cbTTLCache_stateChanged(int state);

    void on_cbBogusPlugin_stateChanged(int state);

    void on_rbDefaultAdsPolicy_toggled(bool checked);

    void on_listChinaDNSServers_itemChanged(QListWidgetItem *item);

    void on_listAbroadDNSServers_itemChanged(QListWidgetItem *item);

    void on_cbResolveAppleDomainByChinaDNS_stateChanged(int state);

    void on_cbResolveGoogleDomainByChinaDNS_stateChanged(int state);

    void on_actionUpdateCoreDNSBinary_triggered();

private:
    Ui::MainWindow *ui;
    QProcess *            m_coredns {nullptr};
    bool                  m_isCoreDNSStarted {false};
    QNetworkAccessManager m_nam;
    QString               m_corefilePath;

    void updateChinaDomainList();
    void updateAppleDomainList();
    void updateGoogleDomainList();
    void updateBogusList();
    bool exportCorefile(const QString &corefile);
};
#endif // MAINWINDOW_H
