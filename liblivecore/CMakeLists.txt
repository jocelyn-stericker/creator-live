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
    audiograph.cpp
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
    modeinterface.cpp
)

list (APPEND liblivecore
    live/audio 
    live/audiograph
    live/liblivecore_global 
    live/songsystem 
    live/timesignature 
    live/mapping 
    live/keysignature 
    live/midifilter 
    live/pitch 
    live/midievent 
    live/asyncconnect 
    live/chord 
    live/time 
    live/core 
    live/audiosecond 
    live/audiointerface 
    live/appinterface 
    live/instrumentinterface 
    live/midi 
    live/app 
    live/instrument 
    live/midieventcounter 
    live/modeinterface
)

list (APPEND liblivecore_qobject
    live/anabeat 
    live/ambition
    live/metronome 
    live/variantbinding 
    live/midibinding 
    asyncconnect_p.h 
    live/object 
    live/audiotrack 
    live/miditrack 
    midibridge.h
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
    list (APPEND liblivecore_cpp
        audio_qnx.cpp
        midiqnx.cpp 
    )
    list (APPEND liblivecore
        audiosystem_qnx_p.h 
    )
    list (APPEND liblivecore_qobject
    )
else (QNX)
    list (APPEND liblivecore_cpp
        audio.cpp
        midi.cpp 
    )
    list (APPEND liblivecore_qobject
        audiosystem_p.h 
        midisystem_p.h 
    )
endif (QNX)

########################## BUILD! ##########################

add_definitions(-Wall)

find_package (Qt4 REQUIRED)
set (QT_USE_QTNETWORK TRUE)
include (${QT_USE_FILE})

qt4_wrap_cpp (liblivecore_moc ${liblivecore_qobject})
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
