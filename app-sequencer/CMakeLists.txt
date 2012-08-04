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

list (APPEND app-sequencer_cpp
    sequencerapp.cpp
    sequencersys.cpp
    sequencergraph.cpp
    sequencerframe.cpp
    looperapp.cpp
    looperframe.cpp
    seq_dotlive.cpp
)

list (APPEND app-sequencer_h
)

list (APPEND app-sequencer_h_qobject
    sequencerapp.h
    sequencersys.h
    sequencergraph.h
    sequencerframe.h
    looperapp.h
    looperframe.h
)

list (APPEND app-sequencer_ui
    sequencerframe.ui
    looperframe.ui
)

list (APPEND app-sequencer_qrc
    sequencer.qrc
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
    ${CMAKE_BINARY_DIR}/app-sequencer
    ../liblivecore/
    ../liblivewidgets/
)

qt4_wrap_cpp (app-sequencer_moc ${app-sequencer_h_qobject})
qt4_wrap_ui (app-sequencer_moc ${app-sequencer_ui}) 
qt4_add_resources (livevst_rcc ${livevst_qrc})

add_library (app-sequencer SHARED
    ${app-sequencer_cpp}
    ${app-sequencer_moc}
    ${livevst_qrc}
)

set_target_properties(app-sequencer PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
