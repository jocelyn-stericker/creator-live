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

list (APPEND pluginTester_cpp
    main.cpp
    apptester.cpp
    instrumenttester.cpp
)

list (APPEND pluginTester_h
)

list (APPEND pluginTester_h_qobject
    apptester.h
    instrumenttester.h
)

list (APPEND pluginTester_ui
    apptester.ui
    instrumenttester.ui
)

list (APPEND pluginTester_qrc
    lite.qrc
)

list (APPEND pluginTester_libs
    livecore
    livewidgets
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
    list (APPEND pluginTester_libs
        asound
    )
else (QNX)
if (NOT WIN32)
    list (APPEND pluginTester_libs
        portmidi
        jack
        sndfile
    )
endif (NOT WIN32)
endif (QNX)

########################## BUILD! ##########################

add_definitions(-Wall)

set(CMAKE_AUTOMOC ON)
find_package (Qt5Widgets REQUIRED)

include_directories (
    ${CMAKE_BINARY_DIR}/pluginTester
    .
    ../liblivecore/
    ../liblivewidgets/
)

link_directories (
    ${CMAKE_BINARY_DIR}/lib/
)

qt5_wrap_ui (pluginTester_moc ${pluginTester_ui}) 
qt5_add_resources (pluginTester_rcc ${pluginTester_qrc})

add_executable(pluginTester
    ${pluginTester_cpp}
    ${pluginTester_moc}
    ${pluginTester_rcc}
)

target_link_libraries (pluginTester
    ${pluginTester_libs}
    ${QT_LIBRARIES}
)

if (WIN32)
    set_target_properties(pluginTester PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif (WIN32)

if (WIN32)
target_link_libraries (pluginTester
    livecore
    livewidgets
    ${QT_LIBRARIES}
    C:/QtSDK/Desktop/Qt/4.8.1/msvc2010/bin/portmidi.lib
)
endif (WIN32)
qt5_use_modules(pluginTester Widgets)
