#-------------------------------------------------
#
# Project created by QtCreator 2012-06-23T20:35:34
#
#-------------------------------------------------

TARGET = livesf2
TEMPLATE = lib

DEFINES += LIVESF2_LIBRARY

SOURCES += livesf2wizard.cpp

HEADERS += livesf2wizard.h\
        livesf2_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE1C19632
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = livesf2.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

FORMS += \
    livesf2wizard.ui
