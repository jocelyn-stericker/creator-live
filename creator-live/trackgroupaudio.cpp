/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "trackgroupaudio.h"

#include "audiooutputchooser.h"

#include <live_widgets/pushbutton.h>
#include <live_widgets/trackinputwidget.h>

int TrackGroup::s_lastId = -1;
using namespace live;
using namespace live_widgets;

TrackGroupAudio::TrackGroupAudio(live::ObjectPtr  c_input, QWidget* c_parent, bool empty)
  : TrackGroup(c_input,c_parent)
{
    // init GUI
    mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    s_hathorView = new VScrollContainer(0);
    s_hathorView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    instLabel = new live_widgets::TrackInputWidget;
    instLabel->b_trackName = c_input->name();
    instLabel->b_audio = true;
    instLabel->setMinimumHeight(350);
    instLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);

    s_hathorView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    if (!empty)
    {
        qDebug() << "NOT_EMPTY";
        QList<live::ObjectPtr> list=object::get(OutputOnly|AudioOnly|NoRefresh);
        s_hathorView->push_back(new Track(c_input,list.front()));
    }
    PushButton* xpush=new PushButton("Insert new Output");
    xpush->setObjectName("xpush");
    s_hathorView->push_back(xpush);
    connect(xpush,SIGNAL(clicked()),this,SLOT(newHathorAuto()));
    s_hathorView->compact = 1;
    s_hathorView->updateItems();

    instLabel->setFixedWidth(64);
    mainLayout->addWidget(instLabel,0, Qt::AlignTop | Qt::AlignLeft);
    mainLayout->addWidget(s_hathorView);
    s_hathorView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    this->setLayout( mainLayout );

    MidiBindingQtSys::addWidget(this);
    mainLayout->setObjectName("mainLayout");
    instLabel->setObjectName("instLabel");
    s_hathorView->setObjectName("s_hathorView");
    AudioOutputChooser* aoo=new AudioOutputChooser(this);
    connect(aoo, SIGNAL(resized()), this, SLOT(resizeEvent()));
    mainLayout->addWidget(s_hathorView);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    adjustSize();
    s_hathorView->hide();
    ui_selectWidget = aoo;
    aoo->setGeometry(width() - aoo->width(), 0, aoo->width(), aoo->height());
    connect(aoo, SIGNAL(outputChosen(live::ObjectPtr)), this, SLOT(setLastOutput(live::ObjectPtr)));
    connect(aoo, SIGNAL(outputChosen(live::ObjectPtr)), this, SLOT(clearSelect()));
}

TrackGroupAudio::~TrackGroupAudio()
{

}

void TrackGroupAudio::newHathorAuto()
{
    newHathor(object::get(OutputOnly|AudioOnly|NoRefresh).front());
}
