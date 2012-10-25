############################################################
# SoundTouch audio processing library v1.6.0               #
#                                                          #
# SoundTouch library Copyright © Olli Parviainen 2001-2011 #
############################################################

# This CMakeLists.txt (C) Joshua Netterfield 2012.

########################## COMMON ##########################

cmake_minimum_required(VERSION 2.8)

list (APPEND livesoundtouch_cpp
    source/SoundTouch/mmx_optimized.cpp
    source/SoundTouch/FIFOSampleBuffer.cpp
    source/SoundTouch/PeakFinder.cpp
    source/SoundTouch/AAFilter.cpp
    source/SoundTouch/SoundTouch.cpp
    source/SoundTouch/RateTransposer.cpp
    source/SoundTouch/FIRFilter.cpp
    source/SoundTouch/BPMDetect.cpp
    source/SoundTouch/sse_optimized.cpp
    source/SoundTouch/cpu_detect_x86.cpp
    source/SoundTouch/TDStretch.cpp
)

list (APPEND livesoundtouch_qobject
    # not needed.
)

include_directories (
    ./include
)

if (!QNX)
    include_directories ( SYSTEM
        /usr/include # Avoid ugly warnings.
        /usr/include/QtGui # Avoid ugly warnings.
        /usr/include/QtCore # Avoid ugly warnings.
    )
endif (!QNX)

################## PLATFORM-SPECIFIC CODE ##################

if (QNX)
    list (APPEND livesoundtouch_cpp
    )
    list (APPEND livesoundtouch
    )
    list (APPEND livesoundtouch_qobject
    )
else (QNX)
    list (APPEND livesoundtouch_cpp
    )
    list (APPEND livesoundtouch_qobject
    )
endif (QNX)

########################## BUILD! ##########################

add_definitions(-Wall)

find_package (Qt4 REQUIRED)
set (QT_USE_QTNETWORK TRUE)
include (${QT_USE_FILE})

qt4_wrap_cpp (livesoundtouch_moc ${livesoundtouch_qobject})
include_directories (${cmake_binary_dir}
    .
)
add_library (livesoundtouch SHARED
    ${livesoundtouch_cpp}
    ${livesoundtouch_moc}
)

if (NOT WIN32)
set_target_properties(livesoundtouch PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
else (NOT WIN32)
set_target_properties(livesoundtouch PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
endif (NOT WIN32)

# no warnings - it's third party

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIVEST_LIBRARY=1")

