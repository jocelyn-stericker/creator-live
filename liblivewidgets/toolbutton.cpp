/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "live_widgets/toolbutton.h"
#include <QMouseEvent>
#include <QPainter>
#include "live/midibinding.h"

live_widgets::ToolButton::ToolButton(QWidget *parent) :
    QToolButton(parent), s_bindMode(0)
{
    connect(live::bindings::me(),SIGNAL(showBindingsChanged(bool)),this,SLOT(setShowBindingsChanged(bool)));
}

void live_widgets::ToolButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton) {
        if(s_bindMode) {
            emit customContextMenuRequested(e->pos());
        } else {
            animateClick();
        }
    }
}

void live_widgets::ToolButton::mouseReleaseEvent(QMouseEvent *)
{

}

void live_widgets::ToolButton::paintEvent(QPaintEvent *e)
{
    QToolButton::paintEvent(e);
    if(s_bindMode) {
        QPainter p(this);
        p.fillRect(e->rect(),QColor(0,0,255,80));
    }
}

void live_widgets::ToolButton::setShowBindingsChanged(bool ean)
{
    s_bindMode=ean;
    update();
}
