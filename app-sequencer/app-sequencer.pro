#-------------------------------------------------
#
# Project created by QtCreator 2012-06-05T22:50:46
#
#-------------------------------------------------

QT       += core gui

TARGET = app-sequencer
TEMPLATE = lib
CONFIG += plugin

DESTDIR = ../plugins

SOURCES += sequencerapp.cpp \
    sequencersys.cpp \
    sequencergraph.cpp \
    sequencerframe.cpp \
    looperapp.cpp \
    looperframe.cpp \
    seq_dotlive.cpp

HEADERS += sequencerapp.h \
    sequencersys.h \
    sequencergraph.h \
    sequencerframe.h \
    looperapp.h \
    looperframe.h

FORMS += \
    sequencerframe.ui

INCLUDEPATH += ../liblivecore/ ../liblivewidgets

RESOURCES += \
    sequencer.qrc

LIBS += -L../lib -llivecore -llivewidgets

