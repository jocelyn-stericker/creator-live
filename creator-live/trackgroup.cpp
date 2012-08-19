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
    s_hathorView->show();
    bool ok=0;
    for (int i=s_hathorView->count()-1;(i!=-1)&&!ok;--i) {
        qDebug()<<s_hathorView->at(i);
        if (dynamic_cast<Track*>(s_hathorView->at(i))) {
            static_cast<Track*>(s_hathorView->at(i))->setOutput(obj, loop);
            static_cast<Track*>(s_hathorView->at(i))->setOutputChooser(dynamic_cast<AudioOutputChooser*>(sender()));
            ok=1;
        }
    }
    instLabel->enableAddTrackButton();
    emit outputSelected();
    Q_ASSERT(ok);
}

void TrackGroup::newHathor(live::ObjectPtr coutput)
{
    if (instLabel)
        instLabel->setFixedHeight(instLabel->height() + 360);

    Track* t = 0;
    s_hathorView->insert(s_hathorView->count(),t = new Track(s_input,coutput));
    s_hathorView->updateItems();
    t->setOutputChooser(coutput->isMidiObject() ? static_cast<OutputChooser*>(new MidiOutputChooser) : static_cast<OutputChooser*>(new AudioOutputChooser));
    t->show();
}
