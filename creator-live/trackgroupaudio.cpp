/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "trackgroupaudio.h"

#include "audiooutputchooser.h"

#include <live/audio>

#include <live_widgets/pushbutton.h>

int TrackGroup::m_lastId = -1;
using namespace live;
using namespace live_widgets;

TrackGroupAudio::TrackGroupAudio(live::ObjectPtr  c_input, QWidget* c_parent, bool empty, ObjectChooser *oc)
  : TrackGroup(c_input,c_parent, oc)
  , mainLayout(new QHBoxLayout)
   {mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(3);
    m_hathorView = new VScrollContainer(0);
    m_hathorView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    Q_ASSERT(instLabel);

    instLabel->showMidi(false);
    instLabel->showAudio(true);
    instLabel->setPopup(false);
    instLabel->b_trackName = c_input->name();
    instLabel->b_audio = true;
    instLabel->setMinimumHeight(350);
    instLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);
    connect(instLabel,SIGNAL(objectChosen(live::ObjectPtr)),this,SLOT(setInput(live::ObjectPtr)));
    connect(instLabel,SIGNAL(newOutputRequested()),this,SLOT(newHathorAuto()));
    mainLayout->addWidget(instLabel,0, Qt::AlignTop | Qt::AlignLeft);
    instLabel->setObjectName("instLabel");

    m_hathorView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    if (!empty)
        m_hathorView->push_back(new Track(c_input,audio::null(2)));

    m_hathorView->compact = 1;
    m_hathorView->updateItems();

    mainLayout->addWidget(m_hathorView);
    m_hathorView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    this->setLayout( mainLayout );

    binding::addWidget(this);

    mainLayout->setObjectName("mainLayout");
    m_hathorView->setObjectName("m_hathorView");
    if (!empty) {
        AudioOutputChooser* aoo=new AudioOutputChooser(this);
        ui_selectWidget = aoo;
        aoo->setGeometry(width() - aoo->width(), 0, aoo->width(), aoo->height());

        connect(aoo, SIGNAL(resized()), this, SLOT(resizeEvent()));
        connect(aoo, SIGNAL(objectChosen(live::ObjectPtr)), this, SLOT(setLastOutput(live::ObjectPtr)));
        connect(aoo, SIGNAL(objectChosen(live::ObjectPtr)), this, SLOT(clearSelect()));
    } else {
        clearSelect();
        m_hathorView->show();
        instLabel->enableAddTrackButton();
    }
    mainLayout->addWidget(m_hathorView);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    adjustSize();

    if (!empty) m_hathorView->hide();

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
