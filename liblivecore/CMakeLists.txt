############################################################
#                                                          #
#     Part of the Creator Live Music Production Suite.     #
# Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012 #
#                                                          #
#                   All rights reserved.                   #
#                                                          #
############################################################

########################## COMMON ##########################

cmake_minimum_required(VERSION 2.8)

list (APPEND liblivecore_cpp
    anabeat.cpp
    mapping.cpp 
    songsystem.cpp 
    metronome.cpp 
    timesignature.cpp 
    variantbinding.cpp 
    keysignature.cpp 
    pitch.cpp 
    midifilter.cpp 
    midibinding.cpp 
    asyncconnect.cpp 
    midievent.cpp 
    core_dotlive.cpp 
    chord.cpp 
    time.cpp 
    object.cpp 
    audiotrack.cpp 
    miditrack.cpp 
    midieventcounter.cpp 
    extern/midifile/src/Options_private.cpp 
    extern/midifile/src/MidiFile.cpp 
    extern/midifile/src/FileIO.cpp 
    extern/midifile/src/Options.cpp 
    appinterface.cpp 
    audiointerface.cpp 
    instrumentinterface.cpp 
    app.cpp 
    instrument.cpp 
    midibridge.cpp
    ambition.cpp
)

list (APPEND liblivecore_h
    live/audio.h 
    live/liblivecore_global.h 
    live/songsystem.h 
    live/timesignature.h 
    live/mapping.h 
    live/keysignature.h 
    live/midifilter.h 
    live/pitch.h 
    live/midievent.h 
    live/asyncconnect.h 
    live/chord.h 
    live/time.h 
    live/core.h 
    live/audiosecond.h 
    live/audiointerface.h 
    live/appinterface.h 
    live/instrumentinterface.h 
    live/midi.h 
    live/app.h 
    live/instrument.h 
    live/midieventcounter.h 
)

list (APPEND liblivecore_h_qobject
    live/anabeat.h 
    live/ambition.h
    live/metronome.h 
    live/variantbinding.h 
    live/midibinding.h 
    asyncconnect_p.h 
    live/object.h 
    live/audiotrack.h 
    live/miditrack.h 
    midibridge.h
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
    list (APPEND liblivecore_cpp
        audio_qnx.cpp
        midiqnx.cpp 
    )
    list (APPEND liblivecore_h
        audiosystem_qnx_p.h 
    )
    list (APPEND liblivecore_h_qobject
    )
else (QNX)
    list (APPEND liblivecore_cpp
        audio.cpp
        midi.cpp 
    )
    list (APPEND liblivecore_h_qobject
        audiosystem_p.h 
        midisystem_p.h 
    )
endif (QNX)

########################## BUILD! ##########################

add_definitions(-Wall)

find_package (Qt4 REQUIRED)
set (QT_USE_QTNETWORK TRUE)
include (${QT_USE_FILE})

qt4_wrap_cpp (liblivecore_moc ${liblivecore_h_qobject})
include_directories (${cmake_binary_dir}
    .
)
add_library (livecore SHARED
    ${liblivecore_cpp}
    ${liblivecore_moc}
)

set_target_properties(livecore PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
