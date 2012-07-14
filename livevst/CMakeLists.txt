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

list (APPEND livevst_cpp
    vstWin.cpp
    vstlinux.cpp
    vstinterface.cpp
    vstselectionwidget.cpp
    vstsettingswidget.cpp
    vst_dotlive.cpp
    vstwizard.cpp
)

list (APPEND livevst_h
    vst.h
    vst_p.h
    vstsidekick.h
)

list (APPEND livevst_h_qobject
    vstlinux_p.h
    vstinterface.h
    vstselectionwidget.h
    vstsettingswidget.h
    vstwizard.h
    hackwidget.h
)

list (APPEND livevst_ui
    vstselectionwidget.ui 
    vstsettingswidget.ui
    vstwizard.ui
)

list (APPEND livevst_qrc
    icons.qrc
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
    .
    ${CMAKE_BINARY_DIR}/livevst
    ../liblivecore/
    ../liblivewidgets/
)

qt4_wrap_cpp (livevst_moc ${livevst_h_qobject})
qt4_wrap_ui (livevst_moc ${livevst_ui}) 
qt4_add_resources (livevst_rcc ${livevst_qrc})

add_library (livevst SHARED
    ${livevst_cpp}
    ${livevst_moc}
    ${livevst_qrc}
)

set_target_properties(livevst PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
