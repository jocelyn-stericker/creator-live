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

list (APPEND context-dj_cpp
    collectioncontext.cpp
    djcontext.cpp
    effectstab.cpp
    livebar.cpp
    sampletab.cpp
    samplerdj.cpp
    tracktab.cpp
)

list (APPEND context-dj_h
)

list (APPEND context-dj_h_qobject
    collectioncontext.h
    djcontext.h
    effectstab.h
    livebar.h
    sampletab.h
    samplerdj.h
    tracktab.h
)

list (APPEND context-dj_ui
    collectioncontext.ui
    djcontext.ui
    effectstab.ui
    sampletab.ui
    livebar.ui
    tracktab.ui
)

list (APPEND context-dj_qrc
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
    ${CMAKE_BINARY_DIR}/context-dj
    ../liblivecore/
    ../liblivewidgets/
)

qt4_wrap_cpp (context-dj_moc ${context-dj_h_qobject})
qt4_wrap_ui (context-dj_moc ${context-dj_ui}) 
qt4_add_resources (livevst_rcc ${livevst_qrc})

add_library (context-dj SHARED
    ${context-dj_cpp}
    ${context-dj_moc}
    ${livevst_qrc}
)

set_target_properties(context-dj PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
