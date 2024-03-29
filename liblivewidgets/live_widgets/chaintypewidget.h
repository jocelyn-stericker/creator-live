/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef CHAINTYPEWIDGET_H
#define CHAINTYPEWIDGET_H

#include <QPaintEvent>
#include <QWidget>

#include "liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT ChainTypeWidget : public QWidget {
    QList<int>m_positions;
    QList<bool>m_midi;
    QList<bool>m_audio;
public:
    ChainTypeWidget(QWidget* p);
    void reset();
    void setBack(int pos,bool midi,bool audio);
    void paintEvent(QPaintEvent *e);
};

}

#endif // CHAINTYPEWIDGET_H
