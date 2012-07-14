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

list (APPEND bridge_cpp
    main.cpp
    bridgesettings.cpp
)

list (APPEND bridge_h
)

list (APPEND bridge_h_qobject
    bridgesettings.h
)

list (APPEND bridge_ui
    bridgesettings.ui
)

list (APPEND bridge_qrc
    lite.qrc
)

list (APPEND bridge_libs
    livecore
    livewidgets
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
    list (APPEND bridge_libs
        asound
    )
else (QNX)
    list (APPEND bridge_libs
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
    ${CMAKE_BINARY_DIR}/bridge
    .
    ../liblivecore/
    ../liblivewidgets/
)

link_directories (
    ${CMAKE_BINARY_DIR}/lib/
)

qt4_wrap_cpp (bridge_moc ${bridge_h_qobject})
qt4_wrap_ui (bridge_moc ${bridge_ui}) 
qt4_add_resources (bridge_rcc ${bridge_qrc})

add_executable(bridge
    ${bridge_cpp}
    ${bridge_moc}
    ${bridge_qrc}
)

target_link_libraries (bridge
    ${bridge_libs}
    ${QT_LIBRARIES}
)

if (!WIN32)
    set_target_properties(creator-live PROPERTIES
        LINK_FLAGS "-Wl,-rpath=\'\$\$ORIGIN\'/../lib"
    )
endif (!WIN32)
