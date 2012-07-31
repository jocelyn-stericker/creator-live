/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef TRACKGROUP_H
#define TRACKGROUP_H

#include "track.h"
#include "live_widgets/vscrollcontainer.h"
#include "live_widgets/bindableparent.h"

class TrackGroup : public QFrame, public live_widgets::BindableParent
{
    Q_OBJECT
protected:                      /*IN CHILDREN*/
    live::ObjectPtr  s_input;             /*CONSTRUCT*/
public:
    live_widgets::VScrollContainer* s_hathorView;   /*003*/
    int s_id;                   /*004*/
    static int s_lastId;

    explicit TrackGroup(live::ObjectPtr  cinput, QWidget *parent = 0) :
        QFrame(parent), BindableParent(this), s_input(cinput), s_hathorView(0), s_id(++s_lastId)
    {
        setFrameStyle(QFrame::NoFrame);
        setLineWidth(0);
        setObjectName("TrackGroup_"+QString::number(s_id));
    }

    void resizeEvent(QResizeEvent *);

signals:
public slots:
    void newHathor(live::ObjectPtr coutput)
    {
        s_hathorView->insert(s_hathorView->count()-1,new Track(s_input,coutput));
        s_hathorView->updateItems();
    }
};

#endif // METAHATHOR_H
