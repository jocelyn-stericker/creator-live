/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "live_widgets/pushbutton.h"
#include <QMouseEvent>
#include "live/midibinding.h"
#include <QPainter>
#include <QPaintEvent>
#include <QColor>
#include <QDebug>

live_widgets::PushButton::PushButton(QWidget* p) : QPushButton(p), s_bindMode(0)
{
    connect(live::bindings::me(),SIGNAL(showBindingsChanged(bool)),this,SLOT(setShowBindingsChanged(bool)));
}

live_widgets::PushButton::PushButton(QString t,QWidget* parent) : QPushButton(t,parent), s_bindMode(0)
{
    connect(live::bindings::me(),SIGNAL(showBindingsChanged(bool)),this,SLOT(setShowBindingsChanged(bool)));
}

void live_widgets::PushButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton) {
        if(s_bindMode) {
            emit customContextMenuRequested(e->pos());
        } else {
            animateClick();
        }
    }
}

void live_widgets::PushButton::mouseReleaseEvent(QMouseEvent *)
{
}

void live_widgets::PushButton::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);
    if(s_bindMode) {
        QPainter p(this);
        p.fillRect(e->rect(),QColor(0,0,255,80));
    }
}

void live_widgets::PushButton::setShowBindingsChanged(bool ean)
{
    s_bindMode=ean;
    update();
}
