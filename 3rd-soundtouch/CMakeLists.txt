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

if (NOT WIN32)
if (NOT QNX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Weffc++ -pedantic -pedantic-errors -Wextra  -Wall -Wcast-align -Wcast-qual  -Wchar-subscripts  -Wcomment -Wconversion -Wdisabled-optimization -Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security  -Wformat-y2k -Wimport -Winit-self -Winline -Winvalid-pch -Wunsafe-loop-optimizations -Wlong-long -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point  -Wsign-compare  -Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wtrigraphs  -Wuninitialized -Wunknown-pragmas  -Wunreachable-code -Wunused -Wunused-function  -Wunused-label  -Wunused-parameter -Wunused-value  -Wunused-variable  -Wvariadic-macros -Wvolatile-register-var  -Wwrite-strings")
endif (NOT QNX)
endif (NOT WIN32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIVEST_LIBRARY=1")

