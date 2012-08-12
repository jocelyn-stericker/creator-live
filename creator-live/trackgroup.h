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

    explicit TrackGroup(live::ObjectPtr  cinput, QWidget *parent = 0)
      : QFrame(parent)
      , BindableParent(this)
      , s_input(cinput)
      , s_hathorView(0)
      , s_id(++s_lastId)
      , ui_selectWidget(0)
    {
        setFrameStyle(QFrame::NoFrame);
        setLineWidth(0);
        setObjectName("TrackGroup_"+QString::number(s_id));
    }

    QWidget* ui_selectWidget;

signals:
    void outputSelected();

public slots:
    void newHathor(live::ObjectPtr coutput)
    {
        Track* t = 0;
        s_hathorView->insert(s_hathorView->count()-1,t = new Track(s_input,coutput));
        s_hathorView->updateItems();
        connect(t, SIGNAL(outputSelected()), this, SLOT(onFirstOutputSelected()));
    }

    void onFirstOutputSelected()
    {
        qDebug() << "OS!";
        disconnect(sender(),SIGNAL(outputSelected()),this,SLOT(onFirstOutputSelected()));
    }
    void clearSelect();
    void resizeEvent(QResizeEvent* e=0);
    void setLastOutput(live::ObjectPtr obj, live::ObjectPtr = live::ObjectPtr(0));
};

#endif // METAHATHOR_H
