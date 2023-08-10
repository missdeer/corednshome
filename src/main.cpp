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

QStringList sortUILanguages(const QStringList &languages)
{
    QStringList sortedLanguages;
    QStringList englishLanguages;

    for (const QString &lang : languages)
    {
        if (lang.startsWith(QStringLiteral("en")))
        {
            englishLanguages << lang;
        }
        else
        {
            sortedLanguages << lang;
        }
    }

    sortedLanguages << englishLanguages;
    return sortedLanguages;
}

void installTranslator(QTranslator &translator, QTranslator &qtTranslator)
{
    QDir translationDir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_MAC)
    translationDir.cdUp();
    translationDir.cd(QStringLiteral("Resources"));
#endif
    translationDir.cd(QStringLiteral("translations"));

    const QStringList uiLanguages = sortUILanguages(QLocale::system().uiLanguages());
    for (const auto &locale : uiLanguages)
    {
        QString qmFileName = QStringLiteral("corednshome_%1.qm").arg(locale).replace('-', '_');
        if (translator.load(qmFileName, translationDir.absolutePath()))
        {
            QApplication::installTranslator(&translator);

            qDebug() << "all ui languages:" << uiLanguages << ", locale:" << locale << ", use qm file name:" << qmFileName;
            if (qtTranslator.load(QStringLiteral("qt_%1.qm").arg(locale).replace('-', '_'), translationDir.absolutePath()))
            {
                QApplication::installTranslator(&qtTranslator);
            }
            return;
        }

        qDebug() << "no translation file is matched for " << locale << " from " << translationDir.absolutePath();
    }
    for (const auto &locale : uiLanguages)
    {
        QString qmFileName = QStringLiteral("corednshome_%1.qm").arg(QLocale(locale).name()).replace('-', '_');
        if (translator.load(qmFileName, translationDir.absolutePath()))
        {
            QApplication::installTranslator(&translator);

            qDebug() << "all ui languages:" << uiLanguages << ", locale:" << QLocale(locale).name() << ", use qm file name:" << qmFileName;
            if (qtTranslator.load(QStringLiteral("qt_%1.qm").arg(QLocale(locale).name()).replace('-', '_'), translationDir.absolutePath()))
            {
                QApplication::installTranslator(&qtTranslator);
            }
            return;
        }

        qDebug() << "no translation file is matched for " << locale << " from " << translationDir.absolutePath();
    }
}

int main(int argc, char *argv[])
{
#ifndef Q_OS_WIN
    QCoreApplication::setSetuidAllowed(true);
#endif
    QCoreApplication::setOrganizationName(QLatin1String("minidump.info"));
    QCoreApplication::setApplicationName("CoreDNSHome");
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

    QTranslator translator;
    QTranslator qtTranslator;
    installTranslator(translator, qtTranslator);
    
    Settings settings;
    g_settings = &settings;
    g_settings->initialize();

    MainWindow w;
    w.show();
    return a.exec();
}
