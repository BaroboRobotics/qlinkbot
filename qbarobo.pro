#-------------------------------------------------
#
# Project created by QtCreator 2014-02-27T10:17:35
#
#-------------------------------------------------

QT       -= gui

TARGET = qbarobo
TEMPLATE = lib

INCLUDEPATH = ../libbarobo/include

LIBS += -L../libbarobo/build -lbarobo

DEFINES += QBAROBO_LIBRARY

SOURCES += QBaroboBridge.cpp\
           QLinkbot.cpp

HEADERS += QBaroboBridge.h\
        qbarobo_global.h\
        QLinkbot.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
