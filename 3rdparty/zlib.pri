SOURCES += \
    $$PWD/zlib/adler32.c \
    $$PWD/zlib/compress.c \
    $$PWD/zlib/crc32.c \
    $$PWD/zlib/deflate.c \
    $$PWD/zlib/gzclose.c \
    $$PWD/zlib/gzlib.c \
    $$PWD/zlib/gzread.c \
    $$PWD/zlib/gzwrite.c \
    $$PWD/zlib/infback.c \
    $$PWD/zlib/inffast.c \
    $$PWD/zlib/inflate.c \
    $$PWD/zlib/inftrees.c \
    $$PWD/zlib/trees.c \
    $$PWD/zlib/uncompr.c \
    $$PWD/zlib/zutil.c

HEADERS  += \
    $$PWD/zlib/crc32.h \
    $$PWD/zlib/deflate.h \
    $$PWD/zlib/gzguts.h \
    $$PWD/zlib/inffast.h \
    $$PWD/zlib/inffixed.h \
    $$PWD/zlib/inflate.h \
    $$PWD/zlib/inftrees.h \
    $$PWD/zlib/trees.h \
    $$PWD/zlib/zconf.h \
    $$PWD/zlib/zlib.h \
    $$PWD/zlib/zutil.h 

INCLUDEPATH += $$PWD/zlib

DEFINES += _CRT_SECURE_NO_DEPRECATE=1  EXPORT_IMPORT_API=

CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1
}

macx {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
}
