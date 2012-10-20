/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef DRAGBUTTON_H
#define DRAGBUTTON_H

#include "liblivewidgets_global.h"
#include <live/audio>
#include <live/object>

#include <QLabel>
#include <QDebug>

class QDrag;

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT DragLabel : public QLabel {
    Q_OBJECT
    QDrag* s_drag;
public:
    explicit DragLabel(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *e);
    virtual ~DragLabel() {
    }

signals:

public slots:
    void drag();

private:
    DragLabel(const DragLabel&)
      : QLabel()
      , s_drag(0)
      { TCRASH();
    }

    DragLabel& operator=(const DragLabel&) {
        TCRASH();
        return *this;
    }
};

}

#endif // DRAGBUTTON_H
