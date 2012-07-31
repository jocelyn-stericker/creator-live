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

list (APPEND creator-live_cpp
    main.cpp
    livewindow.cpp
    ambition.cpp
    track.cpp
    trackgroup.cpp
    trackgroupaudio.cpp
    trackgroupmidi.cpp
    settingslinux.cpp
    settingstabwidget.cpp
    app_dotlive.cpp
    liveapplication.cpp
    audiooutputchooser.cpp
)

list (APPEND creator-live_h
    ambition.h
)

list (APPEND creator-live_h_qobject
    livewindow.h
    track.h
    trackgroup.h
    trackgroupaudio.h
    trackgroupmidi.h
    settingslinux.h
    settingstabwidget.h
    liveapplication.h
    audiooutputchooser.h
)

list (APPEND creator-live_ui
    livewindow.ui
    settingslinux.ui
    settingstabwidget.ui
    audiooutputchooser.ui
)

list (APPEND creator-live_qrc
    Icons/Icons.qrc
    styleSheet.qrc
)

list (APPEND creator-live_libs
    livecore
    livewidgets
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
    list (APPEND creator-live_libs
        asound
    )
else (QNX)
    list (APPEND creator-live_libs
        portmidi
        jack
        sndfile
    )
endif (QNX)

########################## BUILD! ##########################

add_definitions(-Wall)

find_package (Qt4 REQUIRED)
set (QT_USE_QTNETWORK TRUE)
include (${QT_USE_FILE})

include_directories (
    ${CMAKE_BINARY_DIR}/creator-live
    .
    ../liblivecore/
    ../liblivewidgets/
)

link_directories (
    ${CMAKE_BINARY_DIR}/lib/
)

qt4_wrap_cpp (creator-live_moc ${creator-live_h_qobject})
qt4_wrap_ui (creator-live_moc ${creator-live_ui}) 
qt4_add_resources (creator-live_rcc ${creator-live_qrc})

add_executable(creator-live
    ${creator-live_cpp}
    ${creator-live_moc}
    ${creator-live_rcc}
)

target_link_libraries (creator-live
    ${creator-live_libs}
    ${QT_LIBRARIES}
)

if (!WIN32)
    set_target_properties(creator-live PROPERTIES
        LINK_FLAGS "-Wl,-rpath=\'\$\$ORIGIN\'/../lib"
    )
endif (!WIN32)
