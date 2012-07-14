#-------------------------------------------------
#
# Project created by QtCreator 2012-06-01T16:44:30
#
#-------------------------------------------------

TARGET = livewidgets
TEMPLATE = lib

DESTDIR += ../lib

DEFINES += LIBLIVEWIDGETS_LIBRARY

SOURCES += chaintypewidget.cpp \
    draglabel.cpp \
    draglistframe.cpp \
    dropframe.cpp \
    iodragwidget.cpp \
    combobox.cpp \
    metrospinbox.cpp \
    pushbutton.cpp \
    slider.cpp \
    spinbox.cpp \
    toolbutton.cpp \
    pianokey.cpp \
    rotatedlabel.cpp \
    appframe.cpp \
    bindableparent.cpp \
    midibindingqt.cpp \
    newinput.cpp \
    trackhint.cpp \
    introwizard.cpp

HEADERS +=\
        live_widgets/liblivewidgets_global.h \
    live_widgets/draglabel.h \
    live_widgets/chaintypewidget.h \
    live_widgets/dropframe.h \
    live_widgets/iodragwidget.h \
    live_widgets/draglistframe.h \
    live_widgets/combobox.h \
    live_widgets/metrospinbox.h \
    live_widgets/pushbutton.h \
    live_widgets/slider.h \
    live_widgets/spinbox.h \
    live_widgets/toolbutton.h \
    live_widgets/pianokey.h \
    live_widgets/rotatedlabel.h \
    live_widgets/appframe.h \
    live_widgets/bindableparent.h \
    live_widgets/midibindingqt.h \
    live_widgets/vscrollcontainer.h \
    live_widgets/newinput.h \
    live_widgets/trackhint.h \
    live_widgets/introwizard.h

INCLUDEPATH += ../liblivecore/

{
    target.path = /home/joshua/bin
    target.files = liblivewidgets.so.1.0.0
    INSTALLS += target
    build_package.depends += install_target
}

FORMS += \
    inputdragwidget.ui \
    newinput.ui \
    trackhint.ui \
    introwizard.ui
