/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "live_widgets/metrospinbox.h"
#include "live/metronome.h"
#include "live/songsystem.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

live_widgets::MetroSpinbox::MetroSpinbox(QWidget *parent) :
    live_widgets::SpinBox(parent), s_toggle(0)
{
    live::song::current()->metronome->registerSync(this);
}

live_widgets::MetroSpinbox::~MetroSpinbox()
{
    live::song::current()->metronome->unregisterSync(this);
}

void live_widgets::MetroSpinbox::sync(const live::SyncData &data)
{
    if(data.beat!=s_lastBeat) {
//        qDebug()<<"SYNC";
        s_lastBeat=data.beat;
        s_toggle=1;
        update();
    }
}

void live_widgets::MetroSpinbox::paintEvent(QPaintEvent *event)
{
    SpinBox::paintEvent(event);
    if(s_toggle) {
        s_toggle=0;
        QPainter p(this);
        QRect r(event->rect());
        r.setTop(5);
//        if(s_lastBeat==1) {
            p.fillRect(r,QColor(255,0,0,255));
//        }
//        else {
//            p.fillRect(r,QColor(0,0,0,255));
//        }
        QTimer::singleShot(50,this,SLOT(update()));
    }
}
