#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>
#include <QtCore>

#ifdef Q_OS_MAC
#    include <Authorization.h>

#    include <mach-o/dyld.h>
#endif

#include "mainwindow.h"

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
    QCoreApplication::setOrganizationName(QLatin1String("https://github.com/missdeer"));
    QCoreApplication::setApplicationName(QObject::tr("CoreDNS GUI"));
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

    QString     locale = QLocale::system().name();
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

    if (!translator.load("corednsgui_" + locale, localeDirPath))
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " success";
        if (!a.installTranslator(&translator))
        {
            qDebug() << "installing translator failed ";
        }
    }

    // qt locale
    if (!qtTranslator.load("qt_" + locale, localeDirPath))
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " success";
        if (!a.installTranslator(&qtTranslator))
        {
            qDebug() << "installing qt translator failed ";
        }
    }

    MainWindow w;
    w.show();
    return a.exec();
}
