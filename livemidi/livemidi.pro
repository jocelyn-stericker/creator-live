#-------------------------------------------------
#
# Project created by QtCreator 2012-06-02T17:37:05
#
#-------------------------------------------------

QT       += core gui

TARGET = livevst
TEMPLATE = lib
CONFIG += plugin

SOURCES += midiplugin.cpp \
    midiwizard.cpp

HEADERS += midiplugin.h \
    midiwizard.h

DESTDIR = ../plugins/
INSTALLS += target

INCLUDEPATH += ../liblivecore/

FORMS += \
    midiwizard.ui
