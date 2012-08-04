/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LIVECOMBOBOX_H
#define LIVECOMBOBOX_H

#include <QComboBox>
#include "live_widgets/liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT ComboBox : public QComboBox {
    Q_OBJECT
    bool s_bindMode;
public:
    explicit ComboBox(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
public slots:
    void setShowBindingsChanged(bool);

signals:

public slots:

};

}

#endif // LIVECOMBOBOX_H
