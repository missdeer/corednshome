#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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
    void on_actionExportCorefile_triggered();

    void on_actionExit_triggered();

    void on_actionHomepage_triggered();

    void on_actionDonate_triggered();

    void on_actionAbout_triggered();

    void on_actionStartCoreDNS_triggered();

    void on_actionStopCoreDNS_triggered();

private:
    Ui::MainWindow *ui;
    bool            m_isCoreDNSStarted {false};
};
#endif // MAINWINDOW_H
