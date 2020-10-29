QT       += core gui network gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = corednshome
CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$PWD/../3rdparty/zlib.pri)

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/donatedialog.cpp \
    $$PWD/networkreplyhelper.cpp \
    $$PWD/scopedguard.cpp \
    $$PWD/settings.cpp

HEADERS += \
    $$PWD/mainwindow.h \
    $$PWD/donatedialog.h \
    $$PWD/networkreplyhelper.h \
    $$PWD/scopedguard.h \
    $$PWD/settings.h

FORMS += \
    $$PWD/mainwindow.ui \
    $$PWD/donatedialog.ui

win32: {
    RC_FILE += $$PWD/corednshome.rc
    win32-msvc*: QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
    else: RC_FILE += $$PWD/manifest.rc
}

macx: {
    LIBS += -framework IOKit \
            -framework Cocoa \
            -framework Security \
            -framework AppKit \
            -framework ServiceManagement
}

TRANSLATIONS += \
    $$PWD/corednshome_zh-CN.ts

RESOURCES += \
    $$PWD/corednshome.qrc
