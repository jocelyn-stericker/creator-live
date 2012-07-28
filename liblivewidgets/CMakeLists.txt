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
    pianokey.cpp 
    rotatedlabel.cpp 
    appframe.cpp 
    bindableparent.cpp 
    midibindingqt.cpp 
    newinput.cpp 
    trackhint.cpp 
    introwizard.cpp
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
    live_widgets/vscrollcontainer.h 
    live_widgets/newinput.h 
    live_widgets/trackhint.h 
    live_widgets/introwizard.h
)

list (APPEND liblivewidgets_ui
    inputdragwidget.ui
    newinput.ui
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
qt4_wrap_cpp (liblivewidgets_moc ${liblivewidgets_h_qobject})
qt4_wrap_ui (liblivewidgets_moc ${liblivewidgets_ui})

add_library (livewidgets SHARED
    ${liblivewidgets_cpp}
    ${liblivewidgets_moc}
)

set_target_properties(livewidgets PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)