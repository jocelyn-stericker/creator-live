/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef DRAGBUTTON_H
#define DRAGBUTTON_H

#include <QLabel>
#include <QDebug>

#include "liblivewidgets_global.h"

class QDrag;

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT DragLabel : public QLabel {
    Q_OBJECT
    QDrag* s_drag;
public:
    explicit DragLabel(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *e);
    ~DragLabel() {
        qDebug() << "LABEL IS BEING DESTROYED!";
    }

signals:

public slots:
    void drag();
};

}

#endif // DRAGBUTTON_H
