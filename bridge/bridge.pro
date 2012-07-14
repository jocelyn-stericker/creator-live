#-------------------------------------------------
#
# Project created by QtCreator 2012-07-08T17:14:28
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bridge
TEMPLATE = app


SOURCES += main.cpp\
        bridgesettings.cpp

HEADERS  += bridgesettings.h

FORMS    += bridgesettings.ui

INCLUDEPATH += ../liblivecore/ ../liblivewidgets

!qnx:LIBS += -L../lib -llivecore -llivewidgets -lportmidi -ljack -lsndfile
qnx:LIBS += -L../lib -llivecore -llivewidgets -lasound

!win32:QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'/../lib

RESOURCES += \
    lite.qrc
