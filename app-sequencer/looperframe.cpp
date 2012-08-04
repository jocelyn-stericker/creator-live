/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "looperapp.h"
#include "looperframe.h"
#include "ui_sequencerframe.h"

#include <live/asyncconnect>
#include <live_widgets/midibindingqt.h>
#include <live_widgets/toolbutton.h>

using namespace live;
using namespace live_widgets;

LooperFrame::LooperFrame(LooperApp*backend,AbstractTrack *parent) :
    AppFrame(parent),
    app(*backend),
    graph(*(new SequencerGraph(this,backend))),
    ui(new Ui::SequencerFrame)
{
    ui->setupUi(this);

    async_connect(&app.s_midiTrack->b_mute,SIGNAL(changeObserved(bool,bool)),ui->mute,SLOT(setChecked(bool)));
    ui->play->hide();
    ui->record->hide();

    async_connect(ui->mute,SIGNAL(clicked(bool)),this,SLOT(logicMute(bool)));
    ui->play->hide();
    ui->record->hide();

    // LOOPER SPECIFIC...

    QString style="border-top-left-radius: 0px;"
            "border-bottom-left-radius: 0px;"
            "border-top-right-radius: 0px;"
            "border-bottom-right-radius: 0px;";

    QButtonGroup* bg;
    bg=new QButtonGroup();
    bg->setExclusive(1);
    ui->horizontalLayout_2->insertSpacing(4,8);
    // SUBSTITUE
    loopSub = new ToolButton(ui->horizontalFrame);
    loopSub->setText("SUBSTITUE");
    loopSub->setMinimumSize(QSize(0, 21));
    loopSub->setMaximumHeight(23);
    loopSub->setStyleSheet(style);
    loopSub->setCheckable(true);
    loopSub->setObjectName("loopSub");
    ui->horizontalLayout_2->insertWidget(4,loopSub);
    bg->addButton(loopSub);
    async_connect(loopSub,SIGNAL(clicked(bool)),this,SLOT(logicSub(bool)));
    // REPLACE
    loopReplace = new ToolButton(ui->horizontalFrame);
    loopReplace->setText("REPLACE");
    loopReplace->setMinimumSize(QSize(0, 21));
    loopReplace->setMaximumHeight(23);
    loopReplace->setStyleSheet(style);
    loopReplace->setCheckable(true);
    loopReplace->setObjectName("loopReplace");
    ui->horizontalLayout_2->insertWidget(4,loopReplace);
    bg->addButton(loopReplace);
    async_connect(loopReplace,SIGNAL(clicked(bool)),this,SLOT(logicReplace(bool)));
    // OVERDUB
    loopOverdub = new ToolButton(ui->horizontalFrame);
    loopOverdub->setText("OVERDUB");
    loopOverdub->setMinimumSize(QSize(0, 21));
    loopOverdub->setMaximumHeight(23);
    loopOverdub->setStyleSheet(style);
    loopOverdub->setCheckable(true);
    loopOverdub->setObjectName("loopOverdub");
    ui->horizontalLayout_2->insertWidget(4,loopOverdub);
    bg->addButton(loopOverdub);
    async_connect(loopOverdub,SIGNAL(clicked(bool)),this,SLOT(logicOverdub(bool)));
    // RECORD
    loopRecord = new ToolButton(ui->horizontalFrame);
    loopRecord->setText("RECORD");
    loopRecord->setMinimumSize(QSize(0, 21));
    loopRecord->setMaximumHeight(23);
    loopRecord->setStyleSheet(style);
    loopRecord->setCheckable(true);
    loopRecord->setObjectName("loopRecord");
    ui->horizontalLayout_2->insertWidget(4,loopRecord);
    bg->addButton(loopRecord);
    async_connect(loopRecord,SIGNAL(clicked(bool)),this,SLOT(logicRecord(bool)));
    // PLAY
    loopPlay = new ToolButton(ui->horizontalFrame);
    loopPlay->setText("PLAY");
    loopPlay->setMinimumSize(QSize(0, 21));
    loopPlay->setMaximumHeight(23);
    loopPlay->setStyleSheet(style);
    loopPlay->setCheckable(true);
    loopPlay->setObjectName("loopPlay");
    ui->horizontalLayout_2->insertWidget(4,loopPlay);
    bg->addButton(loopPlay);
    async_connect(loopPlay,SIGNAL(clicked(bool)),this,SLOT(logicPlay(bool)));
    // OFF
    loopOff = new ToolButton(ui->horizontalFrame);
    loopOff->setText("OFF");
    loopOff->setMinimumSize(QSize(0, 21));
    loopOff->setMaximumHeight(23);
    loopOff->setStyleSheet(style);
    loopOff->setCheckable(true);
    loopOff->setObjectName("loopOff");
    ui->mute->setStyleSheet(ui->record->styleSheet());
    ui->horizontalLayout_2->insertWidget(4,loopOff);
    bg->addButton(loopOff);
    ui->horizontalLayout_2->insertSpacing(4,8);
    async_connect(loopOff,SIGNAL(clicked(bool)),this,SLOT(logicOff(bool)));

    graph.show();
    ui->visLayout->addWidget(&graph);
    MidiBindingQtSys::addWidget(this);

    syncState();

    setObjectName("LooperFrame_"+QString::number(app.s_id_looper));
}

LooperFrame::~LooperFrame()
{
    graph.deleteLater();
    app.deleteLater();
    delete ui;
}

void LooperFrame::logicMute(bool x)
{
    if (!app.isMute()&&x) app.startMute();
    else if (app.isMute()&&!x) app.stopMute();
}

void LooperFrame::logicSub(bool x)
{
    if (x)
        app.b_loopMode=LooperApp::Substituting;
}

void LooperFrame::logicReplace(bool x)
{
    if (x)
        app.b_loopMode=LooperApp::Replacing;
}

void LooperFrame::logicOverdub(bool x)
{
    if (x)
        app.b_loopMode=LooperApp::Overdubbing;
}

void LooperFrame::logicRecord(bool x)
{
    if (x)
    {
        app.b_loopMode=LooperApp::Recording;
    }
}

void LooperFrame::logicPlay(bool x)
{
    if (x)
    {
        app.b_loopMode=LooperApp::Playing;
    }
}

void LooperFrame::logicOff(bool x)
{
    if (x)
    {
        app.b_loopMode=LooperApp::Off;
    }
}

void LooperFrame::syncState()
{
    switch (app.b_loopMode.ref())
    {
    case LooperApp::Off:
        loopOff->setChecked(1);
        break;
    case LooperApp::Playing:
        loopPlay->setChecked(1);
        //[old]
        break;
    case LooperApp::Recording:
        loopRecord->setChecked(1);
        //starts over at beg.
        //[new]
        break;
    case LooperApp::Overdubbing:
        loopOverdub->setChecked(1);
        //[new]
        //[old]
        break;
    case LooperApp::Multiplying:
        Q_ASSERT(0);
        //[  new stuff here  ]
        //[old][old] old][old]
        break;
    case LooperApp::Replacing:
        loopReplace->setChecked(1);
        //same duration, ERASE old stuff
        break;
    case LooperApp::Substituting:
        loopSub->setChecked(1);
        break;
    }
}
