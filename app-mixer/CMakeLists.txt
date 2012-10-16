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

list (APPEND app-mixer_cpp
    mixerapp.cpp
    mixerframe.cpp
    mixer_dotlive.cpp
)

list (APPEND app-mixer_h
)

list (APPEND app-mixer_h_qobject
    mixerapp.h
    mixerframe.h
)

list (APPEND app-mixer_ui
    mixerframe.ui
)

list (APPEND app-mixer_qrc
    mixer.qrc
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
    ${CMAKE_BINARY_DIR}/app-mixer
    ../liblivecore/
    ../liblivewidgets/
)

qt4_wrap_cpp (app-mixer_moc ${app-mixer_h_qobject})
qt4_wrap_ui (app-mixer_moc ${app-mixer_ui}) 
qt4_add_resources (app-mixer_rcc ${app-mixer_qrc})

add_library (app-mixer SHARED
    ${app-mixer_cpp}
    ${app-mixer_moc}
    ${app-mixer_rcc}
)

if (NOT WIN32)
set_target_properties(app-mixer PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
else (NOT WIN32)
set_target_properties(app-mixer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
endif (NOT WIN32)

if (WIN32)
target_link_libraries (app-mixer
    livecore
    livewidgets
    ${QT_LIBRARIES}
)
endif (WIN32)
