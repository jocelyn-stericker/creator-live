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

list (APPEND livemidi_cpp
    midiplugin.cpp
    midiwizard.cpp
)

list (APPEND livemidi
)

list (APPEND livemidi_qobject
    midiplugin.h 
    midiwizard.h
)

list (APPEND livemidi_ui
    midiwizard.ui
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
    ${CMAKE_BINARY_DIR}/livemidi
    ../liblivecore/
    ../liblivewidgets/
)

qt4_wrap_cpp (livemidi_moc ${livemidi_qobject})
qt4_wrap_ui (livemidi_moc ${livemidi_ui}) 

add_library (livemidi SHARED
    ${livemidi_cpp}
    ${livemidi_moc}
)

set_target_properties(livemidi PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

if (WIN32)
target_link_libraries (livemidi
    livecore
    livewidgets
    ${QT_LIBRARIES}
)
endif (WIN32)
