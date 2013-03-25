/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "trackgroup.h"
#include "midioutputchooser.h"

#include <live_widgets/trackinputselect.h>

#include <QResizeEvent>

void TrackGroup::resizeEvent(QResizeEvent *) {
    if(ui_selectWidget)
        ui_selectWidget->setGeometry(width() - ui_selectWidget->width(), 0, ui_selectWidget->width(), ui_selectWidget->height());
}

void TrackGroup::clearSelect() {
    disconnect(ui_selectWidget, 0, this, 0);
    ui_selectWidget = 0;
}

void TrackGroup::setLastOutput(live::ObjectPtr obj, live::ObjectPtr loop)
{
    m_hathorView->show();
    bool ok=0;
    for (int i=m_hathorView->count()-1;(i!=-1)&&!ok;--i) {
        if (dynamic_cast<const Track*>(m_hathorView->at(i))) {
            static_cast<Track*>((*m_hathorView)[i])->setOutput(obj, loop);
            static_cast<Track*>((*m_hathorView)[i])->setOutputChooser(dynamic_cast<live_widgets::ObjectChooser*>(sender()));
            ok=1;
        }
    }
    instLabel->enableAddTrackButton();
    emit outputSelected();
    Q_ASSERT(ok);
}

void TrackGroup::newHathor(live::ObjectPtr coutput)
{
    if (instLabel) {
        instLabel->incrHeight(356);
    }

    Track* t = 0;
    m_hathorView->insert(m_hathorView->count(),t = new Track(m_input,coutput));

    t->m_ambition.setInputId(instLabel->inputId(), m_hathorView->count()-1);

    m_hathorView->updateItems();
    if (coutput->isMidiObject())
        t->setOutputChooser(new MidiOutputChooser);
    else {
        t->setOutputChooser(new AudioOutputChooser);
    }
    t->show();
}
