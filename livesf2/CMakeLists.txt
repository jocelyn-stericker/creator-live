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

list (APPEND livesf2_cpp
    livesf2wizard.cpp
    sf2interface.cpp
    soundfont.cpp
)

list (APPEND livesf2_h
    livesf2_global.h
)

list (APPEND livesf2_h_qobject
    livesf2wizard.h
    sf2interface.h
    soundfont.h
)

list (APPEND livesf2_ui
    livesf2wizard.ui
)

list (APPEND livesf2_qrc
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
else (QNX)
endif (QNX)

########################## FILES! ##########################
file (INSTALL grand.sf2 mustheory2.sf2 DESTINATION ${CMAKE_BINARY_DIR}/plugins)  

########################## BUILD! ##########################

add_definitions(-Wall)

find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})

include_directories (${cmake_binary_dir}
    ${CMAKE_BINARY_DIR}/livesf2
    ../liblivecore/
    ../liblivewidgets/
)

qt4_wrap_cpp (livesf2_moc ${livesf2_h_qobject})
qt4_wrap_ui (livesf2_moc ${livesf2_ui}) 
qt4_add_resources (livesf2_rcc ${livesf2_qrc})

add_library (livesf2 SHARED
    ${livesf2_cpp}
    ${livesf2_moc}
    ${livesf2_qrc}
)

set_target_properties(livesf2 PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

target_link_libraries (livesf2
    C:/QtSDK/Desktop/Qt/4.8.1/msvc2010/lib/fluidsynth.lib
    livecore
    ${QT_LIBRARIES}
)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIVESF2_LIBRARY=1")
endif (WIN32)
