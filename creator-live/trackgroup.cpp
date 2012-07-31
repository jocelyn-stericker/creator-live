/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "trackgroup.h"

#include <QResizeEvent>

void TrackGroup::resizeEvent(QResizeEvent* e)
{
    qDebug()<<"TrackGroup width:"<<width()<<this;
    qDebug()<<"Child:"<<s_hathorView->width()<<s_hathorView;
//    if (s_hathorView)
//        s_hathorView->setFixedWidth(width() - 70);

    QFrame::resizeEvent(e);
}
