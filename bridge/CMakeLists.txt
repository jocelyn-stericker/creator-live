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
if (!WIN32)
    list (APPEND bridge_libs
        portmidi
        jack
        sndfile
    )
endif (!WIN32)
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
    ${CMAKE_BINARY_DIR}/plugins/    # tmp
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

if (NOT WIN32)
    set_target_properties(bridge PROPERTIES
        LINK_FLAGS "-Wl,-rpath=\'\$\$ORIGIN\'/../lib"
    )
else (NOT WIN32)
    set_target_properties(bridge PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif (NOT WIN32)


if (WIN32)
target_link_libraries (bridge
    livecore
    livewidgets
    ${QT_LIBRARIES}
)
endif (WIN32)
