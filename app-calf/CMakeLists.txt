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

list (APPEND app-calf_cpp
    giface.cpp
    modules.cpp
    modules_dsp.cpp
    utils.cpp


    # Bridge
    fjapp.cpp

    fjreverbapp.cpp
    fjreverbframe.cpp

    fjflangerapp.cpp
    fjflangerframe.cpp

    fjfilterapp.cpp
    fjfilterframe.cpp

    fjphaserapp.cpp
    fjphaserframe.cpp

    calf_dotlive.cpp
)

list (APPEND app-calf_h
    audio_fx.h  
    delay.h        
    fjapp.h 
    inertia.h   
    loudness.h  
    modules_dev.h  
    multichorus.h  
    preset.h      
    utils.h
    biquad.h    
    fixed_point.h  
    giface.h  
    metadata.h  
    modules.h      
    onepole.h      
    primitives.h
)

list (APPEND app-calf_h_qobject
    fjreverbapp.h
    fjreverbframe.h

    fjflangerapp.h
    fjflangerframe.h

    fjfilterapp.h
    fjfilterframe.h

    fjphaserapp.h
    fjphaserframe.h
)

list (APPEND app-calf_ui
    fjreverbframe.ui
    fjflangerframe.ui
    fjfilterframe.ui
    fjphaserframe.ui
)

list (APPEND app-calf_qrc
    calf-icons.qrc
)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
else (QNX)
endif (QNX)

########################## BUILD! ##########################

add_definitions(-Wall)

find_package (Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)

include_directories (${cmake_binary_dir}
    ${CMAKE_BINARY_DIR}/app-calf
    ../
    ../liblivecore/
    ../liblivewidgets/
    ../3rd-soundtouch/include
)

qt5_wrap_ui (app-calf_moc ${app-calf_ui}) 
qt5_add_resources (app-calf_rcc ${app-calf_qrc})

add_library (app-calf SHARED
    ${app-calf_cpp}
    ${app-calf_moc}
    ${app-calf_rcc}
)

if (WIN32)
target_link_libraries (app-calf
    livecore
    livewidgets
    ${QT_LIBRARIES}
)
endif (WIN32)

if (NOT WIN32)
set_target_properties(app-calf PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
else (NOT WIN32)
set_target_properties(app-calf PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
endif (NOT WIN32)

qt5_use_modules(app-calf Widgets)
