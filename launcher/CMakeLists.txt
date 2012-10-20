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

list (APPEND launcher_cpp
    main.cpp
    mainw.cpp
    audiosettingswidget.cpp
)

list (APPEND launcher_h
)

list (APPEND launcher_h_qobject
    mainw.h
    audiosettingswidget.h
)

list (APPEND launcher_ui
    audiosettingswidget.ui
    mainw.ui
)

list (APPEND launcher_qrc
    resource.qrc
)

list (APPEND launcher_libs
)

################## PLATFORM-SPECIFIC CODE ##################

if (WIN32)
    list (APPEND launcher_libs
        shell32
    )
endif (WIN32)

########################## BUILD! ##########################

add_definitions(-Wall)

find_package (Qt4 REQUIRED)
set (QT_USE_QTNETWORK TRUE)
include (${QT_USE_FILE})

include_directories (
    ${CMAKE_BINARY_DIR}/launcher
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

qt4_wrap_cpp (launcher_cpp ${launcher_h_qobject})
qt4_wrap_ui (launcher_moc ${launcher_ui}) 
qt4_add_resources (launcher_rcc ${launcher_qrc})

add_executable(launcher
    ${launcher_cpp}
    ${launcher_moc}
    ${launcher_rcc}
)

target_link_libraries (launcher
    ${launcher_libs}
    ${QT_LIBRARIES}
)

if (NOT WIN32)
    set_target_properties(launcherPROPERTIES
        LINK_FLAGS "-Wl,-rpath=\'\$\$ORIGIN\'/../lib"
    )
else (NOT WIN32)
    set_target_properties(launcher PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif (NOT WIN32)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif (WIN32)

if (NOT QNX AND NOT WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Weffc++ -pedantic -pedantic-errors -Wextra  -Wall -Wcast-align -Wcast-qual  -Wchar-subscripts  -Wcomment -Wconversion -Wdisabled-optimization -Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security  -Wformat-y2k -Wimport -Winit-self -Winline -Winvalid-pch -Wunsafe-loop-optimizations -Wlong-long -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point  -Wsign-compare  -Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch -Wtrigraphs  -Wuninitialized -Wunknown-pragmas  -Wunreachable-code -Wunused -Wunused-function  -Wunused-label  -Wunused-parameter -Wunused-value  -Wunused-variable  -Wvariadic-macros -Wvolatile-register-var  -Wwrite-strings")
endif (NOT QNX AND NOT WIN32)

if (WIN32)
target_link_libraries (launcher
    ${QT_LIBRARIES}
)
endif (WIN32)
