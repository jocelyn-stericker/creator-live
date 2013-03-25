/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LIVESPINBOX_H
#define LIVESPINBOX_H

#include <QSpinBox>
#include "liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT SpinBox : public QSpinBox {
    Q_OBJECT
    bool m_bindMode;
public:
    explicit SpinBox(QWidget *parent = 0);

    virtual void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
public slots:
    void setShowBindingsChanged(bool);

signals:

public slots:

};

}

#endif // LIVESPINBOX_H
