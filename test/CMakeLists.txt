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
    liblivecore.cpp
)
list (APPEND test_cpp
    gtest-src/gtest-all.cc
    gtest-src/gtest_main.cc
)

list (APPEND test_libs
    livecore
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

qt5_wrap_ui (test_moc ${test_ui})
qt5_add_resources (test_rcc ${test_qrc})

add_executable(test
    ${test_cpp}
    ${test_moc}
    ${test_rcc}
)

target_link_libraries (test
    ${test_libs}
    ${QT_LIBRARIES}
)
if (WIN32)
    set_target_properties(test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif (WIN32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
qt5_use_modules(test Widgets)

add_custom_command(TARGET test POST_BUILD DEPENDS test COMMAND test)

add_custom_target(run_test)
macro(run_test test_target depend2)
  add_custom_target(${test_target}_runtest
      COMMAND ${test_target} #cmake 2.6 required
      DEPENDS ${depend2}
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
  add_dependencies(run_test ${test_target}_runtest)
endmacro()

run_test(test test)
run_test(pluginTest test_runtest)
