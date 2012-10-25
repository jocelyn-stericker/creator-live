/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "trackgroupmidi.h"
#include "midioutputchooser.h"

#include <live_widgets/trackinputselect.h>
#include <live_widgets/pianokey.h>
#include <live_widgets/pushbutton.h>
#include <QGraphicsView>
#include <QButtonGroup>

using namespace live;
using namespace live_widgets;

TrackGroupMidi::TrackGroupMidi(ObjectPtr c_input, QWidget *c_parent, bool empty, ObjectChooser *oc)
  : TrackGroup(c_input,c_parent, oc)
  , ui_mainLayout(new QHBoxLayout)
  { ui_mainLayout->setContentsMargins(0, 0, 0, 0);
    parentWidget()->setUpdatesEnabled(false);
    s_hathorView = new VScrollContainer(0);
    s_hathorView->setObjectName("s_hathorView");
    s_hathorView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);


    Q_ASSERT(instLabel);
//    instLabel = new live_widgets::TrackInputSelect(this, false, true, false);
  //     TrackInputSelect* ni=new TrackInputSelect(ui->sac_contents, true, true, true);

    instLabel->showMidi(true);
    instLabel->showAudio(false);
    instLabel->setPopup(false);
    instLabel->b_trackName = c_input->name();
    instLabel->b_audio = false;
    instLabel->setMinimumHeight(350);
    instLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);
    connect(instLabel,SIGNAL(objectChosen(live::ObjectPtr)),this,SLOT(setInput(live::ObjectPtr)));
    connect(instLabel,SIGNAL(newOutputRequested()),this,SLOT(newHathorAuto()));
    ui_mainLayout->addWidget(instLabel,0, Qt::AlignTop | Qt::AlignLeft);
    instLabel->setObjectName("instLabel");
    parentWidget()->setUpdatesEnabled(true);

    ui_mainLayout->addWidget(s_hathorView);

    s_hathorView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    if (!empty) {
        Track* trk=new Track(c_input,midi::null());
        trk->s_ambition.setInputId(instLabel->inputId(),0);
        s_hathorView->push_back(trk);
    }
    s_hathorView->back()->setMinimumWidth(400);
    s_hathorView->compact=1;
    s_hathorView->updateItems();
    //mainLayout->setSpacing(0);

    this->setLayout( ui_mainLayout );
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    binding::addWidget(this);

    if (!empty) {
        MidiOutputChooser* moc=new MidiOutputChooser(this);
        connect(moc, SIGNAL(resized()), this, SLOT(resizeEvent()));

        ui_selectWidget = moc;
        moc->setGeometry(width() - moc->width(), 0, moc->width(), moc->height());
        connect(moc,SIGNAL(objectChosen(live::ObjectPtr, live::ObjectPtr)),this,SLOT(setLastOutput(live::ObjectPtr, live::ObjectPtr)));
        connect(moc, SIGNAL(objectChosen(live::ObjectPtr, live::ObjectPtr)), this, SLOT(clearSelect()));
    } else {
        clearSelect();
        s_hathorView->show();
        instLabel->enableAddTrackButton();
    }

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    adjustSize();
    s_hathorView->hide();
}

void TrackGroupMidi::newHathorAuto()
{
    newHathor(midi::null());
}
