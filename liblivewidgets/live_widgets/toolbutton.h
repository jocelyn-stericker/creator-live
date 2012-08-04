/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LIVETOOLBUTTON_H
#define LIVETOOLBUTTON_H

#include <QToolButton>
#include "liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT ToolButton : public QToolButton {
    Q_OBJECT
    bool s_bindMode;
public:
    explicit ToolButton(QWidget *parent = 0);

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

signals:

public slots:
    void setShowBindingsChanged(bool ean);

};

}

#endif // LIVETOOLBUTTON_H
