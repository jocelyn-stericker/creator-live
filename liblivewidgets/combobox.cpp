/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/combobox.h>
#include <live/midibinding>
#include <QPainter>
#include <QMouseEvent>

live_widgets::ComboBox::ComboBox(QWidget *parent) :
    QComboBox(parent), m_bindMode(0) {
    connect(live::bindings::me(),SIGNAL(showBindingsChanged(bool)),this,SLOT(setShowBindingsChanged(bool)));
}

void live_widgets::ComboBox::mousePressEvent(QMouseEvent *e) {
    if (e->button()==Qt::LeftButton) {
        if (m_bindMode) {
            emit customContextMenuRequested(e->pos());
        } else {
            QComboBox::mousePressEvent(e);
        }
    } else {
        QComboBox::mousePressEvent(e);
    }
}

void live_widgets::ComboBox::mouseReleaseEvent(QMouseEvent *) {
}

void live_widgets::ComboBox::paintEvent(QPaintEvent *e) {
    QComboBox::paintEvent(e);
    if (m_bindMode) {
        QPainter p(this);
        p.fillRect(e->rect(),QColor(0,0,255,80));
    }
}

void live_widgets::ComboBox::setShowBindingsChanged(bool ean) {
    m_bindMode=ean;
    update();
}
