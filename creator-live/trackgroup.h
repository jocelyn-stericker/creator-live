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

namespace live_widgets {
    class TrackInputSelect;
}

class TrackGroup : public QFrame, public live_widgets::BindableParent
{
    Q_OBJECT
protected:                      /*IN CHILDREN*/
    live::ObjectPtr  s_input;             /*CONSTRUCT*/
    live_widgets::TrackInputSelect* instLabel;
public:
    live_widgets::VScrollContainer* s_hathorView;   /*003*/
    int s_id;                   /*004*/
    static int s_lastId;

    explicit TrackGroup(live::ObjectPtr  cinput, QWidget *parent = 0)
      : QFrame(parent)
      , BindableParent(this)
      , s_input(cinput)
      , instLabel(0)
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
    void newHathor(live::ObjectPtr coutput);

    void setInput(live::ObjectPtr in)
    {
        for (unsigned i = 0; i < s_hathorView->count(); ++i) {
            Track* t = qobject_cast<Track*>((*s_hathorView)[i]);
            if (t)
                t->setInput(in);
        }
        s_input = in;
    }

    void clearSelect();
    void resizeEvent(QResizeEvent* e=0);
    void setLastOutput(live::ObjectPtr obj, live::ObjectPtr = live::ObjectPtr(0));

private:
    TrackGroup(const TrackGroup&)
      : QFrame()
      , BindableParent(this)
      , s_input()
      , instLabel(0)
      , s_hathorView(0)
      , s_id(-1)
      , ui_selectWidget(0)
      { TCRASH();
    }

    TrackGroup& operator=(const TrackGroup&) {
        TCRASH();
        return *this;
    }

};

#endif // METAHATHOR_H
