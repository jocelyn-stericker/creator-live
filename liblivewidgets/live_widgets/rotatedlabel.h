/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef ROTATEDLABEL_H
#define ROTATEDLABEL_H

/* GUI */
#include <QLabel>
#include <QPainter>
#include "liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT RotatedLabel : public QLabel {
    Q_OBJECT
public:
    explicit RotatedLabel( QWidget *parent = 0 );
    void paintEvent( QPaintEvent * );
    void mousePressEvent(QMouseEvent *ev);

signals:
    void clicked();
};

}

#endif // ROTATEDLABEL_H
