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

list (APPEND app-pitch_cpp
    pitchapp.cpp
    pitchframe.cpp
    pitch_dotlive.cpp
)

list (APPEND app-pitch
)

list (APPEND app-pitch_qobject
    pitchapp.h
    pitchapp_p.h
    pitchframe.h
)

list (APPEND app-pitch_ui
    pitchframe.ui
)

list (APPEND app-pitch_qrc
    pitch_icons.qrc
)

list (APPEND app-pitch_libs
    livesoundtouch 
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
else (QNX)
endif (QNX)

file (INSTALL app_pitch.png DESTINATION ${CMAKE_BINARY_DIR}/plugins/icons)  

########################## BUILD! ##########################

add_definitions(-Wall)

set(CMAKE_AUTOMOC ON)
find_package (Qt5Widgets REQUIRED)

include_directories (${cmake_binary_dir}
    ${CMAKE_BINARY_DIR}/app-pitch
    ../liblivecore/
    ../liblivewidgets/
    ../3rd-soundtouch/include
)

qt5_wrap_ui (app-pitch_moc ${app-pitch_ui}) 
qt5_add_resources (app-pitch_rcc ${app-pitch_qrc})

add_library (app-pitch SHARED
    ${app-pitch_cpp}
    ${app-pitch_moc}
    ${app-pitch_rcc}
)

if (NOT WIN32)
set_target_properties(app-pitch PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
else (NOT WIN32)
set_target_properties(app-pitch PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
endif (NOT WIN32)

target_link_libraries (app-pitch
    ${app-pitch_libs}
)

if (!WIN32)
    set_target_properties(app-pitchPROPERTIES
        LINK_FLAGS "-Wl,-rpath=\'\$\$ORIGIN\'/../lib"
    )
endif (!WIN32)

if (WIN32)
target_link_libraries (app-pitch
    livecore
    livewidgets
    ${QT_LIBRARIES}
)
endif (WIN32)

qt5_use_modules(app-pitch Widgets)
