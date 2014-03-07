#-------------------------------------------------
#
# Project created by QtCreator 2014-02-27T10:17:35
#
#-------------------------------------------------

QT       -= gui

CONFIG += debug

TARGET = qbarobo
TEMPLATE = lib

INCLUDEPATH = ../libbarobo/include include/

LIBS += -L../libbarobo/build -lbarobo

DEFINES += QBAROBO_LIBRARY

SOURCES += src/QLinkbot.cpp

HEADERS += include/qbarobo_global.h include/QLinkbot.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
