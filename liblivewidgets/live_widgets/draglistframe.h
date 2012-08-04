/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef DRAGLISTFRAME_H
#define DRAGLISTFRAME_H

#include "live_widgets/draglabel.h"
#include "live/audio"
#include <QGridLayout>
#include <QFrame>

#include "liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT DragListFrame : public QFrame {
    QList<DragLabel*> s_list;
    QGridLayout* s_layout;
public:
    DragListFrame(QStringList,QWidget*);
};

class InputDragListFrame : public DragListFrame {
public:
    InputDragListFrame(QWidget*p=0) : DragListFrame(live::audio::getInputChanStringList(),p) {}
};

class OutputDragListFrame : public DragListFrame {
public:
    OutputDragListFrame(QWidget*p=0) : DragListFrame(live::audio::getOutputChanStringList(),p) {}
};

}

#endif // DRAGLISTFRAME_H
