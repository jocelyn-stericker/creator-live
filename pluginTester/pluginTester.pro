#-------------------------------------------------
#
# Project created by QtCreator 2012-06-09T17:09:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = appTester
TEMPLATE = app


SOURCES += main.cpp\
        apptester.cpp \
    instrumenttester.cpp

HEADERS  += apptester.h \
    instrumenttester.h

FORMS    += apptester.ui \
    instrumenttester.ui


INCLUDEPATH += ../liblivecore/ ../liblivewidgets

!qnx:LIBS += -L../lib -llivecore -llivewidgets -lportmidi -ljack -lsndfile
qnx:LIBS += -L../lib -llivecore -llivewidgets -lasound

!win32:QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'/../lib

RESOURCES += \
    lite.qrc

