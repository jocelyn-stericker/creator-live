/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "mixerframe.h"
#include "ui_mixerframe.h"
#include <live_widgets/midibindingqt.h>
#include <QtPlugin>

using namespace live;
using namespace live_widgets;

MixerFrame::MixerFrame(MixerApp *backend, AbstractTrack *parent) :
    AppFrame(parent),
    app(*backend),
    ui(new Ui::MixerFrame)
{
    ui->setupUi(this);
    connect(ui->dial_pan,SIGNAL(valueChanged(int)),&app.b_pan,SLOT(set(int)));
    connect(&app.b_pan,SIGNAL(changeObserved(int,int)),ui->dial_pan,SLOT(setValue(int)));

    connect(ui->verticalSlider_vol,SIGNAL(valueChanged(int)),&app.b_vol,SLOT(set(int)));
    connect(&app.b_vol,SIGNAL(changeObserved(int,int)),ui->verticalSlider_vol,SLOT(setValue(int)));

    connect(ui->toolButton,SIGNAL(toggled(bool)),&app.b_mute,SLOT(set(bool)));
    connect(&app.b_mute,SIGNAL(changeObserved(bool,bool)),ui->toolButton,SLOT(setChecked(bool)));

    connect(ui->toolButton_2,SIGNAL(toggled(bool)),&app.b_solo,SLOT(set(bool)));
    connect(&app.b_solo,SIGNAL(changeObserved(bool,bool)),ui->toolButton_2,SLOT(setChecked(bool)));

    setDesiredWidth(56);

    binding::addWidget(this);
}

MixerFrame::~MixerFrame()
{
    kill_kitten {
        delete &app;
    }
    delete ui;
}
