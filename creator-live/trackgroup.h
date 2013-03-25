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
#include "live_widgets/trackinputselect.h"

class TrackGroup : public QFrame, public live_widgets::BindableParent
{
    Q_OBJECT
protected:                      /*IN CHILDREN*/
    live::ObjectPtr  m_input;             /*CONSTRUCT*/
    live_widgets::TrackInputSelect* instLabel;
public:
    live_widgets::VScrollContainer* m_hathorView;   /*003*/
    int m_id;                   /*004*/
    static int m_lastId;

    explicit TrackGroup(live::ObjectPtr  cinput, QWidget *parent = 0, live_widgets::ObjectChooser* oc = 0)
      : QFrame(parent)
      , BindableParent(this)
      , m_input(cinput)
      , instLabel(0)
      , m_hathorView(0)
      , m_id(++m_lastId)
      , ui_selectWidget(0)
    {
        if (! (instLabel = dynamic_cast<live_widgets::TrackInputSelect*>(oc))) {
            instLabel = new live_widgets::TrackInputSelect(this, false, true, true); // will be corrected
        }
        setFrameStyle(QFrame::NoFrame);
        setLineWidth(0);
        setObjectName("TrackGroup_"+QString::number(m_id));
    }

    QWidget* ui_selectWidget;

signals:
    void outputSelected();

public slots:
    void newHathor(live::ObjectPtr coutput);

    void setInput(live::ObjectPtr in)
    {
        for (unsigned i = 0; i < m_hathorView->count(); ++i) {
            Track* t = qobject_cast<Track*>((*m_hathorView)[i]);
            if (t)
                t->setInput(in);
        }
        m_input = in;
    }

    void clearSelect();
    void resizeEvent(QResizeEvent* e=0);
    void setLastOutput(live::ObjectPtr obj, live::ObjectPtr = live::ObjectPtr(0));

private:
    TrackGroup(const TrackGroup&)
      : QFrame()
      , BindableParent(this)
      , m_input()
      , instLabel(0)
      , m_hathorView(0)
      , m_id(-1)
      , ui_selectWidget(0)
      { TCRASH();
    }

    TrackGroup& operator=(const TrackGroup&) {
        TCRASH();
        return *this;
    }

};

#endif // METAHATHOR_H
