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

list (APPEND liblivewidgets_cpp
    chaintypewidget.cpp 
    draglabel.cpp 
    draglistframe.cpp 
    dropframe.cpp 
    iodragwidget.cpp 
    combobox.cpp 
    metrospinbox.cpp 
    pushbutton.cpp 
    slider.cpp 
    spinbox.cpp 
    toolbutton.cpp 
    objectchooser.cpp
    pianokey.cpp 
    rotatedlabel.cpp 
    appframe.cpp 
    bindableparent.cpp 
    midibindingqt.cpp 
    trackinputselect.cpp 
    trackhint.cpp 
    introwizard.cpp
    track.cpp
)

list (APPEND liblivewidgets_h
    live_widgets/chaintypewidget.h 
    live_widgets/bindableparent.h 
    live_widgets/draglistframe.h 
    live_widgets/liblivewidgets_global.h
)

list (APPEND liblivewidgets_h_qobject
    live_widgets/draglabel.h 
    live_widgets/dropframe.h 
    live_widgets/iodragwidget.h 
    live_widgets/combobox.h 
    live_widgets/metrospinbox.h 
    live_widgets/pushbutton.h 
    live_widgets/slider.h 
    live_widgets/spinbox.h 
    live_widgets/toolbutton.h 
    live_widgets/pianokey.h 
    live_widgets/rotatedlabel.h 
    live_widgets/appframe.h 
    live_widgets/midibindingqt.h 
    live_widgets/objectchooser.h
    live_widgets/vscrollcontainer.h 
    live_widgets/trackinputselect.h 
    live_widgets/trackhint.h 
    live_widgets/introwizard.h
    live_widgets/track.h
)

list (APPEND liblivewidgets_ui
    inputdragwidget.ui
    trackinputselect.ui
    trackhint.ui
    introwizard.ui
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
else (QNX)
endif (QNX)

########################## BUILD! ##########################

add_definitions(-Wall)

find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})

include_directories (
    ${CMAKE_BINARY_DIR}/liblivewidgets
    ../liblivecore
    .
)

include_directories ( SYSTEM
    /usr/include # Avoid ugly warnings.
    /usr/include/QtGui # Avoid ugly warnings.
    /usr/include/QtCore # Avoid ugly warnings.
)

qt4_wrap_cpp (liblivewidgets_moc ${liblivewidgets_h_qobject})
qt4_wrap_ui (liblivewidgets_moc ${liblivewidgets_ui})

add_library (livewidgets SHARED
    ${liblivewidgets_cpp}
    ${liblivewidgets_moc}
)

set_target_properties(livewidgets PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if (!QNX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Weffc++ -pedantic -pedantic-errors -Wextra  -Wall -Wcast-align -Wcast-qual  -Wchar-subscripts  -Wcomment -Wconversion -Wdisabled-optimization -Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security  -Wformat-y2k -Wimport -Winit-self -Winline -Winvalid-pch -Wunsafe-loop-optimizations -Wlong-long -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point  -Wsign-compare  -Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wtrigraphs  -Wuninitialized -Wunknown-pragmas  -Wunreachable-code -Wunused -Wunused-function  -Wunused-label  -Wunused-parameter -Wunused-value  -Wunused-variable  -Wvariadic-macros -Wvolatile-register-var  -Wwrite-strings")
endif (!QNX)
