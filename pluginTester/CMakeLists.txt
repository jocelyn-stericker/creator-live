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
enable_testing()

list (APPEND test_cpp
    apptester.cpp
    instrumenttester.cpp
    main.cpp
)
#list (APPEND test_cpp
#    gtest-src/gtest-all.cc
#    gtest-src/gtest_main.cc
#)

list (APPEND test_h_qobject
    apptester.h
    instrumenttester.h
)

list (APPEND test_ui
    apptester.ui
    instrumenttester.ui
)

list (APPEND test_qrc
    lite.qrc
)

list (APPEND test_libs
    livecore
    livewidgets
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
    list (APPEND test_libs
        asound
    )
else (QNX)
if (NOT WIN32)
    list (APPEND test_libs
        portmidi
        jack
        pthread
        m
        sndfile
    )
endif (NOT WIN32)
endif (QNX)

########################## BUILD! ##########################

add_definitions(-Wall)

set(CMAKE_AUTOMOC ON)
find_package (Qt5Widgets REQUIRED)
find_package (Qt5Qml REQUIRED)
find_package (Qt5Quick REQUIRED)

include_directories (
    ${CMAKE_BINARY_DIR}/pluginTester
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

qt5_wrap_ui (test_moc ${test_ui})
qt5_add_resources (test_rcc ${test_qrc})


add_executable(pluginTest
    ${test_cpp}
    ${test_moc}
    ${test_rcc}
)

target_link_libraries (pluginTest
    ${test_libs}
    ${QT_LIBRARIES}
)
if (WIN32)
    set_target_properties(pluginTest PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif (WIN32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
qt5_use_modules(pluginTest Widgets)

add_custom_target(run_pluginTest)
macro(run_test2 test_target)
  add_custom_target(${test_target}_runtest2
      COMMAND ${test_target} #cmake 2.6 required
      DEPENDS ${test_target}
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
  add_dependencies(run_pluginTest ${test_target}_runtest2)
endmacro()

run_test2(test)
