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

list (APPEND app-bpm_cpp
    bpmapp.cpp
    bpmframe.cpp
    bpm_dotlive.cpp
)

list (APPEND app-bpm_h
)

list (APPEND app-bpm_h_qobject
    bpmapp.h
    bpmframe.h
)

list (APPEND app-bpm_ui
#    bpmframe.ui
)

list (APPEND app-bpm_qrc
    bpm_icons.qrc
)

list (APPEND app-bpm_libs
    livesoundtouch 
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
else (QNX)
endif (QNX)

########################## BUILD! ##########################

add_definitions(-Wall)

find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})

include_directories (${cmake_binary_dir}
    ${CMAKE_BINARY_DIR}/app-bpm
    ../liblivecore/
    ../liblivewidgets/
    ../3rd-soundtouch/include
)

qt4_wrap_cpp (app-bpm_moc ${app-bpm_h_qobject})
qt4_wrap_ui (app-bpm_moc ${app-bpm_ui}) 
qt4_add_resources (app-bpm_rcc ${app-bpm_qrc})

add_library (app-bpm SHARED
    ${app-bpm_cpp}
    ${app-bpm_moc}
    ${app-bpm_rcc}
)

set_target_properties(app-bpm PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

target_link_libraries (app-bpm
    ${app-bpm_libs}
)

if (!WIN32)
    set_target_properties(app-bpmPROPERTIES
        LINK_FLAGS "-Wl,-rpath=\'\$\$ORIGIN\'/../lib"
    )
endif (!WIN32)

if (WIN32)
target_link_libraries (app-bpm
    livecore
    livewidgets
    ${QT_LIBRARIES}
)
endif (WIN32)
