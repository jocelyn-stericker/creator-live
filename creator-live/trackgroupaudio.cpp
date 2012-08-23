/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "trackgroupaudio.h"

#include "audiooutputchooser.h"

#include <live/audio>

#include <live_widgets/pushbutton.h>

int TrackGroup::s_lastId = -1;
using namespace live;
using namespace live_widgets;

TrackGroupAudio::TrackGroupAudio(live::ObjectPtr  c_input, QWidget* c_parent, bool empty)
  : TrackGroup(c_input,c_parent)
  , mainLayout(new QHBoxLayout)
   {mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);
    s_hathorView = new VScrollContainer(0);
    s_hathorView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    instLabel = new live_widgets::TrackInputSelect(this, false, false, true);
    instLabel->b_trackName = c_input->name();
    instLabel->b_audio = true;
    instLabel->setMinimumHeight(350);
    instLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);
    connect(instLabel,SIGNAL(created(live::ObjectPtr )),this,SLOT(setInput(live::ObjectPtr)));
    connect(instLabel,SIGNAL(newOutputRequested()),this,SLOT(newHathorAuto()));
    mainLayout->addWidget(instLabel,0, Qt::AlignTop | Qt::AlignLeft);
    instLabel->setObjectName("instLabel");

    s_hathorView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    if (!empty)
        s_hathorView->push_back(new Track(c_input,audio::null(2)));

    s_hathorView->compact = 1;
    s_hathorView->updateItems();

    mainLayout->addWidget(s_hathorView);
    s_hathorView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    this->setLayout( mainLayout );

    MidiBindingQtSys::addWidget(this);
    mainLayout->setObjectName("mainLayout");
    s_hathorView->setObjectName("s_hathorView");
    AudioOutputChooser* aoo=new AudioOutputChooser(this);
    mainLayout->addWidget(s_hathorView);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    adjustSize();
    s_hathorView->hide();
    ui_selectWidget = aoo;
    aoo->setGeometry(width() - aoo->width(), 0, aoo->width(), aoo->height());

    connect(aoo, SIGNAL(resized()), this, SLOT(resizeEvent()));
    connect(aoo, SIGNAL(outputChosen(live::ObjectPtr)), this, SLOT(setLastOutput(live::ObjectPtr)));
    connect(aoo, SIGNAL(outputChosen(live::ObjectPtr)), this, SLOT(clearSelect()));
}

TrackGroupAudio::TrackGroupAudio(const TrackGroupAudio&)
  : TrackGroup(ObjectPtr(), 0)
  , mainLayout(0)
  { TCRASH();
}

TrackGroupAudio::~TrackGroupAudio()
{

}

void TrackGroupAudio::newHathorAuto()
{
    newHathor(audio::null(2));
}
