#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>
#include <QtCore>

#ifdef Q_OS_MAC
#include <Security/Authorization.h>
#include <mach-o/dyld.h>
#endif

#include "mainwindow.h"
#include "settings.h"

#ifdef Q_OS_MAC
bool relaunch_as_root()
{
    char             appPath[2048] = {0};
    uint32_t         size          = sizeof(appPath) / sizeof(appPath[0]);
    AuthorizationRef authRef;
    OSStatus         status;

    /* Get the path of the current program */
    if (_NSGetExecutablePath(appPath, &size) != 0)
    {
        QMessageBox::critical(
            nullptr, QObject::tr("Critial error"), QObject::tr("Could not get program path to elevate privileges."), QMessageBox::Ok);
        return false;
    }

    status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);

    if (status != errAuthorizationSuccess)
    {
        QMessageBox::critical(nullptr, QObject::tr("Critial error"), QObject::tr("Failed to create authorization reference."), QMessageBox::Ok);
        return false;
    }
    status = AuthorizationExecuteWithPrivileges(authRef, appPath, kAuthorizationFlagDefaults, NULL, NULL);
    AuthorizationFree(authRef, kAuthorizationFlagDestroyRights);

    if (status == errAuthorizationSuccess)
    {
        /* We've successfully re-launched with root privs. */
        return true;
    }

    return false;
}
#endif

int main(int argc, char *argv[])
{
#ifndef Q_OS_WIN
    QCoreApplication::setSetuidAllowed(true);
#endif
    QCoreApplication::setOrganizationName(QLatin1String("minidump.info"));
    QCoreApplication::setApplicationName("CoreDNS-Home");
    QCoreApplication::setApplicationVersion(QLatin1String("1.0"));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);

    //! Commented for debug convenience.
    //#ifdef Q_OS_MAC
    //    if (geteuid() != 0)
    //    {
    //        if (relaunch_as_root())
    //        {
    //            /* We have re-launched with root privs. Exit this process. */
    //            return 0;
    //        }

    //        QMessageBox::critical(
    //            nullptr, QObject::tr("Critial error"), QObject::tr("This program requires root privileges to fully function."), QMessageBox::Ok);
    //        return -1;
    //    }
    //#endif

    if (!QSslSocket::supportsSsl())
    {
        QMessageBox::critical(
            nullptr, QObject::tr("Critical error"), QObject::tr("SSL not supported, some features may stop working."), QMessageBox::Ok);
    }

    QString     locale = QLocale().uiLanguages()[0];
    QTranslator translator;
    QTranslator qtTranslator;

    // main application and dynamic linked library locale
#if defined(Q_OS_MAC)
    QString localeDirPath = QApplication::applicationDirPath() + "/../Resources/translations";
#else
    QString localeDirPath = QApplication::applicationDirPath() + "/translations";
    if (!QDir(localeDirPath).exists())
    {
        localeDirPath = QApplication::applicationDirPath() + "/../translations";
    }
#endif

    if (!translator.load("corednshome_" + locale, localeDirPath))
    {
        qDebug() << "loading "
                 << "corednshome_" + locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading "
                 << "corednshome_" + locale << " from " << localeDirPath << " success";
        if (!a.installTranslator(&translator))
        {
            qDebug() << "installing corednshome translator failed ";
        }
    }

    // qt locale
    if (!qtTranslator.load("qt_" + locale, localeDirPath))
    {
        qDebug() << "loading "
                 << "qt_" + locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading "
                 << "qt_" + locale << " from " << localeDirPath << " success";
        if (!a.installTranslator(&qtTranslator))
        {
            qDebug() << "installing qt translator failed ";
        }
    }
    Settings settings;
    g_settings = &settings;
    g_settings->initialize();

    MainWindow w;
    w.show();
    return a.exec();
}
