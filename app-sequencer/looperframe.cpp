/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "looperframe.h"
#include "ui_looperframe.h"

#include "looperapp.h"

#include <live/asyncconnect>
#include <live_widgets/midibindingqt.h>
#include <live_widgets/toolbutton.h>

#include <QPropertyAnimation>
#include <QTimer>

using namespace live;
using namespace live_widgets;

LooperFrame::LooperFrame(LooperApp*backend,AbstractTrack *parent) :
    AppFrame(parent),
    app(*backend),
    graph(*(new SequencerGraph(this,backend))),
    ui(new Ui::LooperFrame)
{
    ui->setupUi(this);

    // LOOPER SPECIFIC...

    QButtonGroup* bg;
    bg=new QButtonGroup();
    bg->setExclusive(1);
    // SUBSTITUE
    bg->addButton(ui->loopSub);
    connect(ui->loopSub,SIGNAL(clicked(bool)),this,SLOT(logicSub(bool)));
    // REPLACE
    bg->addButton(ui->loopReplace);
    connect(ui->loopReplace,SIGNAL(clicked(bool)),this,SLOT(logicReplace(bool)));
    // OVERDUB
    bg->addButton(ui->loopOverdub);
    connect(ui->loopOverdub,SIGNAL(clicked(bool)),this,SLOT(logicOverdub(bool)));
    // RECORD
    bg->addButton(ui->loopRecord);
    connect(ui->loopRecord,SIGNAL(clicked(bool)),this,SLOT(logicRecord(bool)));
    // PLAY
    bg->addButton(ui->loopPlay);
    connect(ui->loopPlay,SIGNAL(clicked(bool)),this,SLOT(logicPlay(bool)));
    // OFF
    bg->addButton(ui->loopOff);
    connect(ui->loopOff,SIGNAL(clicked(bool)),this,SLOT(logicOff(bool)));

    graph.show();
    ui->visLayout->addWidget(&graph);
    MidiBindingQtSys::addWidget(this);

    syncState();

    setObjectName("LooperFrame_"+QString::number(app.s_id_looper));

    connect(&b_growing, SIGNAL(changeObserved()), this, SIGNAL(desiredWidthChanged()));
    connect(&b_resizing, SIGNAL(changeObserved(bool,bool)), &graph, SLOT(setUpdatesDisabled(bool)));

    connect(ui->toolButton_more, SIGNAL(toggled(bool)), this, SLOT(setMore(bool)));
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
        ui->loopOff->setChecked(1);
        break;
    case LooperApp::Playing:
        ui->loopPlay->setChecked(1);
        //[old]
        break;
    case LooperApp::Recording:
        ui->loopRecord->setChecked(1);
        //starts over at beg.
        //[new]
        break;
    case LooperApp::Overdubbing:
        ui->loopOverdub->setChecked(1);
        //[new]
        //[old]
        break;
    case LooperApp::Multiplying:
        Q_ASSERT(0);
        //[  new stuff here  ]
        //[old][old] old][old]
        break;
    case LooperApp::Replacing:
        ui->loopReplace->setChecked(1);
        //same duration, ERASE old stuff
        break;
    case LooperApp::Substituting:
        ui->loopSub->setChecked(1);
        break;
    }
}


void LooperFrame::setMore(bool more)
{
    int sugWidth = 300;
    AbstractTrack* t = dynamic_cast<AbstractTrack*>(parent());
    if (t) {
        sugWidth = t->getMaximumWidthFor(this);
    }
    QPropertyAnimation* paMin = new QPropertyAnimation(ui->frame_2, "minimumWidth");
    QPropertyAnimation* paMax = new QPropertyAnimation(ui->frame_2, "maximumWidth");
    QPropertyAnimation* paMinThis = new QPropertyAnimation(this, "desiredWidth");

    paMin->setStartValue(ui->frame_2->width());
    paMax->setStartValue(ui->frame_2->width());
    paMinThis->setStartValue(width());;
    if (more) {
        paMin->setEndValue(sugWidth - 55);
        paMax->setEndValue(sugWidth - 55);

        paMinThis->setEndValue(sugWidth);
        removeRounding();
    } else {
        paMin->setEndValue(0);
        paMax->setEndValue(0);
        paMinThis->setEndValue(55);

        connect(paMinThis, SIGNAL(finished()), this, SLOT(addRounding()));
    }

    b_growing = false;

    paMin->setDuration(500);
    paMax->setDuration(500);
    paMinThis->setDuration(500);

    paMin->setEasingCurve(QEasingCurve::InQuad);
    paMax->setEasingCurve(QEasingCurve::InQuad);
    paMin->start(QAbstractAnimation::DeleteWhenStopped);
    paMax->start(QAbstractAnimation::DeleteWhenStopped);
    paMinThis->setEasingCurve(QEasingCurve::InQuad);
    paMinThis->start(QAbstractAnimation::DeleteWhenStopped);

    connect(paMinThis, SIGNAL(finished()), &b_growing, SLOT(setTrue()));

    QTimer::singleShot(600, parent(), SLOT(updateGeometriesOrDie()));
}

void LooperFrame::addRounding()
{
    ui->frame_2->hide();
    QString style = ui->menu->styleSheet();
    style.replace("border-top-right-radius: 0px;", "border-top-right-radius: 4px;");
    ui->menu->setStyleSheet(style);

    style = ui->toolButton_more->styleSheet();
    style.replace("border-bottom-right-radius: 0px;", "border-bottom-right-radius: 4px;");
    ui->toolButton_more->setStyleSheet(style);
}

void LooperFrame::removeRounding()
{
    ui->frame_2->show();
    QString style = ui->menu->styleSheet();
    style.replace("border-top-right-radius: 4px;", "border-top-right-radius: 0px;");
    ui->menu->setStyleSheet(style);

    style = ui->toolButton_more->styleSheet();
    style.replace("border-bottom-right-radius: 4px;", "border-bottom-right-radius: 0px;");
    ui->toolButton_more->setStyleSheet(style);
}
