/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/draglabel.h>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>

live_widgets::DragLabel::DragLabel(QWidget *parent) :
    QLabel(parent),
    s_drag(0)
{
//    setAutoFillBackground(true);
//    setFrameShape(QFrame::Panel);
//    setFrameShadow(QFrame::Raised);
}

void live_widgets::DragLabel::mousePressEvent(QMouseEvent *e)
{
//    QLabel::mousePressEvent(e);
    if (e->buttons() & Qt::LeftButton)
    {
        QTimer::singleShot(0,this,SLOT(drag()));
    }
}

void live_widgets::DragLabel::drag()
{
    if (s_drag) return;
    s_drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    if (property("dragID").isNull())
    {
        mimeData->setText(text());
    }
    else
    {
        mimeData->setText(property("dragID").toString());
    }

    if (pixmap()) {
        s_drag->setPixmap(*pixmap());
    }

    s_drag->setMimeData(mimeData);
    s_drag->exec(Qt::CopyAction);
    s_drag=0;
}
