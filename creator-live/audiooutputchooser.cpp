/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "audiooutputchooser.h"
#include "ui_audiooutputchooser.h"
#include "live_widgets/midibindingqt.h"
#include "live/object.h"
#include "live/audio.h"
#include <cmath>
#include <math.h>

using namespace live;
using namespace live_widgets;

AudioOutputChooser::AudioOutputChooser(QWidget *parent) :
    QWidget(parent),
    s_ui(new Ui::AudioOutputChooser)
{
    s_ui->setupUi(this);

    connect(s_ui->Bwidget,SIGNAL(clicked(QModelIndex)),s_ui->Bwidget,SLOT(setCurrentIndex(QModelIndex)));
    connect(s_ui->Bwidget,SIGNAL(clicked(QModelIndex)),s_ui->pushButton_create,SLOT(click()));
    connect(s_ui->Bwidget,SIGNAL(doubleClicked(QModelIndex)),s_ui->pushButton_create,SLOT(animateClick()));
    connect(s_ui->Bwidget,SIGNAL(currentRowChanged(int)),this,SLOT(rowChangedEvent()));
    connect(s_ui->pushButton_create,SIGNAL(clicked()),this,SLOT(go()));
    connect(s_ui->pushButton_refresh,SIGNAL(clicked()),this,SLOT(refresh()));

    internalStockChangedEvent();

    MidiBindingQtSys::addWidget(this);

    connect(object::singleton(),SIGNAL(stockChanged()),this,SLOT(internalStockChangedEvent()));
}

AudioOutputChooser::~AudioOutputChooser()
{
    delete s_ui;
}

void AudioOutputChooser::resizeEvent(QResizeEvent *e)
{
     int w=width()-s_ui->line_4->width();
     s_ui->Awidget->setFixedWidth(w/2);
     float phi=(1+sqrt(5.0))/2.0;
     s_ui->Bwidget->setFixedWidth(w/2.00/phi);
     s_ui->Cwidget->setFixedWidth(w/2.00/phi/phi);

     if(e) QWidget::resizeEvent(e);
}

void AudioOutputChooser::go()
{
    emit outputChosen(object::get(OutputOnly|AudioOnly|NoRefresh)[s_ui->Bwidget->currentRow()]);
    deleteLater();
}

void AudioOutputChooser::refresh()
{
    midi::refresh();
    audio::refresh();
    internalStockChangedEvent();
}

void AudioOutputChooser::rowChangedEvent() {
    s_ui->pushButton_create->setEnabled(s_ui->Bwidget->currentRow()!=-1);
}

void AudioOutputChooser::internalStockChangedEvent()
{
    QList<ObjectPtr> list = object::get(OutputOnly|AudioOnly|NoRefresh);
    QStringList v;
    v<<list;
    if(all!=v)
    {
        all.clear();
        all+=v;
        s_ui->Bwidget->clear();
        s_ui->Bwidget->insertItems(0,all);
    }
    s_ui->Bwidget->setFixedHeight(v.size()*38+(v.size()-1)*8+5);
}
