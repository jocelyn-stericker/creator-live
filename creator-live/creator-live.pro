#-------------------------------------------------
#
# Project created by QtCreator 2012-06-03T22:30:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = creator-live
TEMPLATE = app


SOURCES += main.cpp\
        livewindow.cpp \
    ambition.cpp \
    track.cpp \
    trackgroupaudio.cpp \
    trackgroupmidi.cpp \
    settingslinux.cpp \
    settingstabwidget.cpp \
    app_dotlive.cpp \
    liveapplication.cpp \
    audiooutputchooser.cpp

HEADERS  += livewindow.h \
    ambition.h \
    track.h \
    trackgroup.h \
    trackgroupaudio.h \
    trackgroupmidi.h \
    settingslinux.h \
    settingstabwidget.h \
    liveapplication.h \
    audiooutputchooser.h

FORMS    += livewindow.ui \
    settingslinux.ui \
    settingstabwidget.ui \
    audiooutputchooser.ui

RESOURCES += \
    Icons/Icons.qrc \
    styleSheet.qrc

INCLUDEPATH += ../liblivecore/ ../liblivewidgets

LIBS += -L../lib -llivecore -llivewidgets

!qnx:LIBS +=  -lportmidi -ljack -lsndfile
qnx:LIBS += -lasound

!win32:QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'/../lib
