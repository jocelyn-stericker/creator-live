/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "bpmframe.h"
#include <QHBoxLayout>
#include <QtPlugin>

using namespace live_widgets;

BPMFrame::BPMFrame(BPMApp*backend,AbstractTrack *parent) :
    AppFrame(parent), app(*backend), ui_label(new RotatedLabel(this)),  ui_tb(new ToolButton(this))
{
    setLayout(new QVBoxLayout);
    setMinimumWidth(50);
    setMaximumWidth(50);
    startTimer(200);
    setObjectName("BPMDetectFrame_"+QString::number(app.s_id));
    ui_label->setParent(this);
    ui_tb->setIcon(QIcon(":/icons/clip.png"));
    layout()->addWidget(ui_tb);
    layout()->addWidget(ui_label);
    ui_tb->setCheckable(1);
    connect(ui_tb,SIGNAL(toggled(bool)),&app.b_sync,SLOT(set(bool)));
    connect(&app.b_sync,SIGNAL(changeObserved(bool,bool)),ui_tb,SLOT(setChecked(bool)));
    ui_tb->setChecked(app.b_sync);
}

BPMFrame::~BPMFrame()
{
    app.deleteLater();
}

void BPMFrame::timerEvent(QTimerEvent *)
{
    if(app.getBPM()>20) ui_label->setText(" "+QString::number(app.getBPM())+" BPM");
    else ui_label->setText("Detecting pulse...");
}

void BPMFrame::resizeEvent(QResizeEvent *)
{
    if(width()>50)
    {
        setMinimumWidth(50);
        setMaximumWidth(50);
    }
}

Q_EXPORT_PLUGIN2(live::AppInterface, BPMCreator)
