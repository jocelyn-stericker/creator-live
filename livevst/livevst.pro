#-------------------------------------------------
#
# Project created by QtCreator 2012-06-02T17:37:05
#
#-------------------------------------------------

QT       += core gui

TARGET = livevst
TEMPLATE = lib
CONFIG += plugin


SOURCES += \
    vstWin.cpp \
    vstlinux.cpp \
    vstinterface.cpp \
    vstselectionwidget.cpp \
    vstsettingswidget.cpp \
    vst_dotlive.cpp \
    vstwizard.cpp

HEADERS += vst.h \
    vst_p.h \
    vstlinux_p.h \
    vstsidekick.h \
    vstinterface.h \
    vstselectionwidget.h \
    vstsettingswidget.h \
    vstwizard.h \
    hackwidget.h

DESTDIR = ../plugins/
INSTALLS += target

INCLUDEPATH += ../liblivecore/

FORMS += \
    vstselectionwidget.ui \
    vstsettingswidget.ui \
    vstwizard.ui

RESOURCES += \
    icons.qrc
