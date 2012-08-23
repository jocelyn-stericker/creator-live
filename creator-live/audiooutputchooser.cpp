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
  : OutputChooser(parent)
  , all()
  , s_ui(new Ui::AudioOutputChooser)
  { s_ui->setupUi(this);

    connect(s_ui->Bwidget,SIGNAL(clicked(QModelIndex)),s_ui->Bwidget,SLOT(setCurrentIndex(QModelIndex)));
    connect(s_ui->Bwidget,SIGNAL(clicked(QModelIndex)),this,SLOT(go()));
    connect(s_ui->Bwidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(go()));
    connect(s_ui->Bwidget,SIGNAL(currentRowChanged(int)),this,SLOT(rowChangedEvent()));
    connect(&b_trackName, SIGNAL(changeObserved(QString,QString)), s_ui->inputName, SLOT(setText(QString)));

    internalStockChangedEvent();

    MidiBindingQtSys::addWidget(this);

    connect(object::singleton(),SIGNAL(stockChanged()),this,SLOT(internalStockChangedEvent()));

    setFixedWidth(55);
    maximize();

    connect(s_ui->inputType, SIGNAL(toggled(bool)), this, SLOT(minimize(bool)));
}

AudioOutputChooser::AudioOutputChooser(const AudioOutputChooser&)
  : OutputChooser(0)
  , all()
  , s_ui(0)
  {
}

AudioOutputChooser::~AudioOutputChooser()
{
    delete s_ui;
}

void AudioOutputChooser::resizeEvent(QResizeEvent *e)
{
//     int w=width()-s_ui->line_4->width();
//     s_ui->Awidget->setFixedWidth(w/2);
//     float phi=(1+sqrt(5.0))/2.0;
//     s_ui->Bwidget->setFixedWidth(w/2.00/phi);
//     s_ui->Cwidget->setFixedWidth(w/2.00/phi/phi);

    emit resized();
    if (e) QWidget::resizeEvent(e);
}

void AudioOutputChooser::go()
{
    b_trackName = object::get(OutputOnly|AudioOnly|NoRefresh)[s_ui->Bwidget->currentRow()]->name();
    if (width() != 55) {
        minimize();
        QTimer::singleShot(200, this, SLOT(go()));
    } else {
        emit outputChosen(object::get(OutputOnly|AudioOnly|NoRefresh)[s_ui->Bwidget->currentRow()]);
    }
}

void AudioOutputChooser::refresh()
{
    midi::refresh();
    audio::refresh();
    internalStockChangedEvent();
}

void AudioOutputChooser::rowChangedEvent() {
//    s_ui->pushButton_create->setEnabled(s_ui->Bwidget->currentRow()!=-1);
}

void AudioOutputChooser::internalStockChangedEvent()
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

void AudioOutputChooser::minimize(bool reverse) {
    s_ui->inputType->setChecked(reverse);
    QPropertyAnimation* qaa[2];
    qaa[0] = new QPropertyAnimation(this, "minimumWidth");
    qaa[1] = new QPropertyAnimation(this, "maximumWidth");
    for (int i = 0; i < 2; ++i) {
        qaa[i]->setStartValue(width());
        qaa[i]->setEndValue(reverse ? 400 : 55);
        qaa[i]->setDuration(200);
        qaa[i]->setEasingCurve(QEasingCurve::InQuad);
        qaa[i]->start(QAbstractAnimation::DeleteWhenStopped);
        connect(qaa[i], SIGNAL(destroyed()), this, SIGNAL(doneResizing())); // yes, twice.
    }
    if(!reverse)
        connect(qaa[1], SIGNAL(destroyed()), this, SLOT(roundCorners()));
    else
        squareCorners();
}

void AudioOutputChooser::roundCorners() {
    QString ss = s_ui->inputType->styleSheet();
    ss.replace("border-top-left-radius: 0px;", "border-top-left-radius: 4px;");
    s_ui->inputType->setStyleSheet(ss);

    ss = s_ui->frame_2->styleSheet();
    ss.replace("border-bottom-left-radius: 0px;", "border-bottom-left-radius: 4px;");
    s_ui->frame_2->setStyleSheet(ss);
}

void AudioOutputChooser::squareCorners() {
    QString ss = s_ui->inputType->styleSheet();
    ss.replace("border-top-left-radius: 4px;", "border-top-left-radius: 0px;");
    s_ui->inputType->setStyleSheet(ss);

    ss = s_ui->frame_2->styleSheet();
    ss.replace("border-bottom-left-radius: 4px;", "border-bottom-left-radius: 0px;");
    s_ui->frame_2->setStyleSheet(ss);
}
