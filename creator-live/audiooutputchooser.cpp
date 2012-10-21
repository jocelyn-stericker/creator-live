/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "audiooutputchooser.h"
#include "ui_audiooutputchooser.h"
#include <live_widgets/midibindingqt.h>
#include <live/object>
#include <live/audio>
#include <cmath>
#include <math.h>

#include <QPropertyAnimation>
#include <QTimer>

using namespace live;
using namespace live_widgets;

AudioOutputChooser::AudioOutputChooser(QWidget *parent)
  : live_widgets::ObjectChooser(parent)
  , all()
  , s_ui(new Ui::AudioOutputChooser)
  { s_ui->setupUi(this);
    setMinimizedButton(s_ui->inputType);
    setTopFrame(s_ui->inputType);
    setBottomFrame(s_ui->frame_2);

    connect(s_ui->Bwidget,SIGNAL(clicked(QModelIndex)),s_ui->Bwidget,SLOT(setCurrentIndex(QModelIndex)));
    connect(s_ui->Bwidget,SIGNAL(clicked(QModelIndex)),this,SLOT(activateSelected()));
    connect(s_ui->Bwidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(activateSelected()));
    connect(&b_trackName, SIGNAL(changeObserved(QString,QString)), s_ui->inputName, SLOT(setText(QString)));

    updateObjects();

    binding::addWidget(this);

    setFixedWidth(55);
    setMaximized(true);

    connect(s_ui->inputType, SIGNAL(toggled(bool)), this, SLOT(setMaximized(bool)));
}

AudioOutputChooser::AudioOutputChooser(const AudioOutputChooser&)
  : live_widgets::ObjectChooser(0)
  , all()
  , s_ui(0)
  {
}

AudioOutputChooser::~AudioOutputChooser()
{
    delete s_ui;
}

void AudioOutputChooser::activateSelected()
{
    b_trackName = object::get(OutputOnly|AudioOnly|NoRefresh)[s_ui->Bwidget->currentRow()]->name();
    if (width() != 55) {
        setMinimized();
        QTimer::singleShot(200, this, SLOT(activateSelected()));
    } else {
        emit objectChosen(object::get(OutputOnly|AudioOnly|NoRefresh)[s_ui->Bwidget->currentRow()]);
    }
}

void AudioOutputChooser::updateObjects()
{
    QList<ObjectPtr> list = object::get(OutputOnly|AudioOnly|NoRefresh);
    QStringList v;
    v<<list;
    if (all!=v)
    {
        all.clear();
        all+=v;
        s_ui->Bwidget->clear();
        s_ui->Bwidget->insertItems(0,all);
    }
}

