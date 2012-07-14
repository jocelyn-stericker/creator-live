#-------------------------------------------------
#
# Project created by QtCreator 2012-05-31T07:22:21
#
#-------------------------------------------------

QT       -= gui
QT += network

TARGET = livecore
TEMPLATE = lib

DESTDIR += ../lib

DEFINES += LIBLIVECORE_LIBRARY

SOURCES += \
    anabeat.cpp \
    mapping.cpp \
    songsystem.cpp \
    metronome.cpp \
    timesignature.cpp \
    variantbinding.cpp \
    keysignature.cpp \
    pitch.cpp \
    midifilter.cpp \
    midibinding.cpp \
    asyncconnect.cpp \
    midievent.cpp \
    core_dotlive.cpp \
    chord.cpp \
    time.cpp \
    object.cpp \
    audiotrack.cpp \
    miditrack.cpp \
    midieventcounter.cpp \
    extern/midifile/src/Options_private.cpp \
    extern/midifile/src/MidiFile.cpp \
    extern/midifile/src/FileIO.cpp \
    extern/midifile/src/Options.cpp \
    appinterface.cpp \
    audiointerface.cpp \
    midi.cpp \
    instrumentinterface.cpp \
    app.cpp \
    instrument.cpp \
    midiqnx.cpp \
    midibridge.cpp

qnx:SOURCES += audio_qnx.cpp
!qnx:SOURCES += audio.cpp

HEADERS +=\
        live/liblivecore_global.h \
    live/anabeat.h \
    audiosystem_p.h \
    live/mapping.h \
    midisystem_p.h \
    live/songsystem.h \
    live/metronome.h \
    live/timesignature.h \
    live/variantbinding.h \
    live/keysignature.h \
    live/pitch.h \
    live/midifilter.h \
    live/midibinding.h \
    live/asyncconnect.h \
    asyncconnect_p.h \
    live/midievent.h \
    live/chord.h \
    live/object.h \
    live/time.h \
    live/core.h \
    live/audiosecond.h \
    live/audiotrack.h \
    live/miditrack.h \
    live/midieventcounter.h \
    live/appinterface.h \
    live/audiointerface.h \
    live/audio.h \
    live/midi.h \
    live/instrumentinterface.h \
    live/app.h \
    live/instrument.h \
    audiosystem_qnx_p.h \
    midibridge.h

unix:!symbian {
    target.path = ../bin
    INSTALLS += target
}
