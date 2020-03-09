#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>

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
    void onChinaDomainRequestFinished();
    void onGoogleDomainRequestFinished();
    void onAppleDomainRequestFinished();
    void onBogusListRequestFinished();

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

    void on_listAbroadDNSServer_itemChanged(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager m_nam;
    bool                  m_isCoreDNSStarted {false};
    QString               m_chinaDomainList;
    QString               m_appleDomainList;
    QString               m_googleDomainList;
    QString               m_bogusList;

    void updateChinaDomainList();
    void updateAppleDomainList();
    void updateGoogleDomainList();
    void updateBogusList();
};
#endif // MAINWINDOW_H
