###########################################################
#                                                          #
#     Part of the Creator Live Music Production Suite.     #
# Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012 #
#                                                          #
#                   All rights reserved.                   #
#                                                          #
############################################################

########################## COMMON ##########################

cmake_minimum_required(VERSION 2.8)

list (APPEND test_cpp
    liblivecore.cpp

    gtest-src/gtest-all.cc
    gtest-src/gtest_main.cc
)

list (APPEND test_libs
    livecore
    pthread
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
    list (APPEND test_libs
        asound
    )
else (QNX)
    list (APPEND test_libs
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
    ${CMAKE_BINARY_DIR}/test
    .
    ../liblivecore/
    ../liblivewidgets/
)

include_directories ( SYSTEM
    /usr/include # Avoid ugly warnings.
    /usr/include/QtGui # Avoid ugly warnings.
    /usr/include/QtCore # Avoid ugly warnings.
)

link_directories (
    ${CMAKE_BINARY_DIR}/lib/
)

qt4_wrap_cpp (test_moc ${test_h_qobject})
qt4_wrap_ui (test_moc ${test_ui})
qt4_add_resources (test_rcc ${test_qrc})

add_executable(test
    ${test_cpp}
    ${test_moc}
    ${test_rcc}
)

target_link_libraries (test
    ${test_libs}
    ${QT_LIBRARIES}
)

if (!WIN32)
    set_target_properties(test PROPERTIES
        LINK_FLAGS "-Wl,-rpath=\'\$\$ORIGIN\'/../lib"
    )
endif (!WIN32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_custom_command(TARGET test POST_BUILD COMMAND test)
