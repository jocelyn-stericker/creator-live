/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LIVEPUSHBUTTON_H
#define LIVEPUSHBUTTON_H

#include <QPushButton>
#include "liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT PushButton : public QPushButton {
    Q_OBJECT
    bool m_bindMode;
public:
    PushButton(QWidget* parent=0);
    PushButton(QString t,QWidget* parent=0);

    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
public slots:
    void setShowBindingsChanged(bool);
};

}

#endif // LIVEPUSHBUTTON_H
