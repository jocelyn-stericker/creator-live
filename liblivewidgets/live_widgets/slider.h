/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LIVESLIDER_H
#define LIVESLIDER_H

#include <QSlider>
#include "liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT Slider : public QSlider {
    Q_OBJECT
    bool s_bindMode;
public:
    explicit Slider(QWidget *parent = 0);

    virtual void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
public slots:
    void setShowBindingsChanged(bool);

signals:

public slots:

};

}

#endif // LIVESLIDER_H
