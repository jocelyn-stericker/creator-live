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

list (APPEND creator-live_cpp
    main.cpp
    livewindow.cpp
    track.cpp
    trackgroup.cpp
    trackgroupaudio.cpp
    trackgroupmidi.cpp
    settingslinux.cpp
    settingstabwidget.cpp
    app_dotlive.cpp
    liveapplication.cpp
    audiooutputchooser.cpp
    midioutputchooser.cpp
)

list (APPEND creator-live_h
)

list (APPEND creator-live_h_qobject
    livewindow.h
    track.h
    trackgroup.h
    trackgroupaudio.h
    trackgroupmidi.h
    settingslinux.h
    settingstabwidget.h
    liveapplication.h
    audiooutputchooser.h
    midioutputchooser.h
)

list (APPEND creator-live_ui
    livewindow.ui
    settingslinux.ui
    settingstabwidget.ui
    audiooutputchooser.ui
    midioutputchooser.ui
)

list (APPEND creator-live_qrc
    icons/icons.qrc
    styleSheet.qrc
    programs.qrc
)

list (APPEND creator-live_libs
    livecore
    livewidgets
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
    list (APPEND creator-live_libs
        asound
    )
else (QNX)
if (NOT WIN32)
    list (APPEND creator-live_libs
        portmidi
        jack
        sndfile
    )
endif (NOT WIN32)
endif (QNX)

########################## BUILD! ##########################

add_definitions(-Wall)

find_package (Qt4 REQUIRED)
set (QT_USE_QTNETWORK TRUE)
include (${QT_USE_FILE})

include_directories (
    ${CMAKE_BINARY_DIR}/creator-live
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

qt4_wrap_cpp (creator-live_moc ${creator-live_h_qobject})
qt4_wrap_ui (creator-live_moc ${creator-live_ui}) 
qt4_add_resources (creator-live_rcc ${creator-live_qrc})

add_executable(creator-live
    ${creator-live_cpp}
    ${creator-live_moc}
    ${creator-live_rcc}
)

target_link_libraries (creator-live
    ${creator-live_libs}
    ${QT_LIBRARIES}
)

if (WIN32)
    set_target_properties(creator-live PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif (WIN32)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif (WIN32)

if (!QNX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Weffc++ -pedantic -pedantic-errors -Wextra  -Wall -Wcast-align -Wcast-qual  -Wchar-subscripts  -Wcomment -Wconversion -Wdisabled-optimization -Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security  -Wformat-y2k -Wimport -Winit-self -Winline -Winvalid-pch -Wunsafe-loop-optimizations -Wlong-long -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point  -Wsign-compare  -Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wtrigraphs  -Wuninitialized -Wunknown-pragmas  -Wunreachable-code -Wunused -Wunused-function  -Wunused-label  -Wunused-parameter -Wunused-value  -Wunused-variable  -Wvariadic-macros -Wvolatile-register-var  -Wwrite-strings")
endif (!QNX)

if (WIN32)
target_link_libraries (creator-live
    livecore
    livewidgets
    ${QT_LIBRARIES}
    C:/QtSDK/Desktop/Qt/4.8.1/msvc2010/bin/portmidi.lib
)
endif (WIN32)
