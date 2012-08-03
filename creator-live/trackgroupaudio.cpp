/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "trackgroupaudio.h"

#include "audiooutputchooser.h"

#include <live_widgets/pushbutton.h>

int TrackGroup::s_lastId = -1;
using namespace live;
using namespace live_widgets;

TrackGroupAudio::TrackGroupAudio(live::ObjectPtr  c_input, QWidget* c_parent, bool empty) :
    TrackGroup(c_input,c_parent)
{
    // init GUI
    mainLayout = new QHBoxLayout;
    s_hathorView = new VScrollContainer(0);
    s_hathorView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    instLabel = new RotatedLabel;
    instLabel->setText(c_input->name());
    instLabel->setMinimumHeight(220);
    instLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    s_hathorView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    if (!empty)
    {
        qDebug()<<"NOT_EMPTY";
        QList<live::ObjectPtr> list=object::get(OutputOnly|AudioOnly|NoRefresh);
        s_hathorView->push_back(new Track(c_input,list.front()));
    }
    PushButton* xpush=new PushButton("Insert new Output");
    xpush->setObjectName("xpush");
    s_hathorView->push_back(xpush);
    connect(xpush,SIGNAL(clicked()),this,SLOT(newHathorAuto()));
    s_hathorView->compact = 1;
    s_hathorView->updateItems();

    instLabel->setFixedWidth(15);
    mainLayout->addWidget(instLabel,0, Qt::AlignTop | Qt::AlignLeft);
    mainLayout->addWidget(s_hathorView);
    s_hathorView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    this->setLayout( mainLayout );

    MidiBindingQtSys::addWidget(this);
    mainLayout->setObjectName("mainLayout");
    instLabel->setObjectName("instLabel");
    s_hathorView->setObjectName("s_hathorView");
    AudioOutputChooser* aoo=new AudioOutputChooser(this);
    mainLayout->addWidget(aoo);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    s_hathorView->hide();
    connect(aoo,SIGNAL(outputChosen(live::ObjectPtr)),this,SLOT(setLastOutput(live::ObjectPtr)));
}

void TrackGroupAudio::setLastOutput(live::ObjectPtr obj)
{
    s_hathorView->show();
    bool ok=0;
    for (int i=s_hathorView->count()-1;(i!=-1)&&!ok;--i) {
        qDebug()<<s_hathorView->at(i);
        if (dynamic_cast<Track*>(s_hathorView->at(i))) {
            dynamic_cast<Track*>(s_hathorView->at(i))->setOutput(obj);
            ok=1;
        }
    }
    emit outputSelected();
    Q_ASSERT(ok);
}

TrackGroupAudio::~TrackGroupAudio()
{

}

void TrackGroupAudio::newHathorAuto()
{
    newHathor(object::get(OutputOnly|AudioOnly|NoRefresh).front());
}
