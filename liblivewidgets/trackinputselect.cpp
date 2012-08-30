/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/trackinputselect.h>
#include <live/audio>
#include "ui_trackinputselect.h"
#include <cmath>
#include <math.h>
#include <QPropertyAnimation>
#include <QTimer>

using namespace live_widgets;
using namespace live;

TrackInputSelect::TrackInputSelect(QWidget*parent, bool popout, bool allowMidi, bool allowAudio)
  : QFrame(parent)
  , s_ui(new Ui_Frame)
  , s_allowMidi(allowMidi)
  , s_allowAudio(allowAudio)
  , all()
  , b_trackName("NULL")
  , b_audio(allowAudio)
  { s_ui->setupUi(this);

    connect(s_ui->input_combo,SIGNAL(clicked(QModelIndex)),s_ui->input_combo,SLOT(setCurrentIndex(QModelIndex)));
    connect(s_ui->input_combo,SIGNAL(clicked(QModelIndex)),this,SLOT(go()));
    connect(s_ui->input_combo,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(go()));
    connect(s_ui->input_combo,SIGNAL(currentRowChanged(int)),this,SLOT(rowChangedEvent()));

    connect(s_ui->inputNew, SIGNAL(clicked()), this, SIGNAL(newOutputRequested()));

    all.push_back("0");
    internalStockChangedEvent();

    MidiBindingQtSys::addWidget(this);

    connect(object::singleton(),SIGNAL(stockChanged()),this,SLOT(internalStockChangedEvent()));
    rowChangedEvent();

    connect(s_ui->inputType, SIGNAL(toggled(bool)), this, SLOT(minimize(bool)));

    connect(&b_trackName, SIGNAL(changeObserved(QString,QString)), s_ui->inputName, SLOT(setText(QString)));
    connect(&b_audio, SIGNAL(changeObserved(bool,bool)), this, SLOT(onSetAudio(bool)));
    connect(&b_audio, SIGNAL(changeObserved(bool,bool)), s_ui->inputPaint, SLOT(setHidden(bool)));

    setFixedWidth(53);

    if (popout) {
        s_ui->inputPaint->hide();
        s_ui->inputNew->hide();
        maximize();
    } else {
        minimize();
    }
}

void TrackInputSelect::internalStockChangedEvent() {
    int flags = InputOnly|NoRefresh;

    if (s_allowAudio && !s_allowMidi)
        flags = flags|AudioOnly;
    if (!s_allowAudio && s_allowMidi)
        flags = flags|MidiOnly;

    QList<ObjectPtr> list = object::get(flags);
    QStringList v;
    v<<list;
    if (all!=v) {
        all.clear();
        all+=v;
        s_ui->input_combo->clear();
        s_ui->input_combo->insertItems(0,all);
    }
//    s_ui->input_combo->setFixedHeight(v.size()*38+(v.size()-1)*8);
}

void TrackInputSelect::go() {
    if (width() != 53) {
        minimize();
        QTimer::singleShot(200, this, SLOT(go()));
    } else {
        emit created(object::get(InputOnly|NoRefresh)[s_ui->input_combo->currentRow()]);
    }
}

void TrackInputSelect::refresh() {
    midi::refresh();
    audio::refresh();
    internalStockChangedEvent();
}

void TrackInputSelect::rowChangedEvent() {
//    s_ui->pushButton_create->setEnabled(s_ui->input_combo->currentRow()!=-1);
}

void TrackInputSelect::resizeEvent(QResizeEvent *e) {
//     int w=width()-s_ui->line_4->width();
//     s_ui->Awidget->setFixedWidth(w/2);
//     float phi=(1+sqrt(5.0))/2.0;
//     s_ui->Bwidget->setFixedWidth(w/2.00/phi);
//     s_ui->Cwidget->setFixedWidth(w/2.00/phi/phi);

     if (e)
         QFrame::resizeEvent(e);
}

void TrackInputSelect::minimize(bool reverse) {
    if (s_ui->inputType->isChecked() != reverse) {
        s_ui->inputType->setChecked(reverse);
        return;
    }

    if (width() == (reverse ? 400 : 53)) {
        if (!reverse)
            roundCorners();
        else
            squareCorners();
        return;
    }

    QPropertyAnimation* qaa[2];
    qaa[0] = new QPropertyAnimation(this, "minimumWidth");
    qaa[1] = new QPropertyAnimation(this, "maximumWidth");
    for (int i = 0; i < 2; ++i) {
        qaa[i]->setStartValue(width());
        qaa[i]->setEndValue(reverse ? 400 : 53);
        qaa[i]->setDuration(200);
        qaa[i]->setEasingCurve(QEasingCurve::InQuad);
        qaa[i]->start(QAbstractAnimation::DeleteWhenStopped);
    }
    if(!reverse)
        connect(qaa[1], SIGNAL(destroyed()), this, SLOT(roundCorners()));
    else
        squareCorners();
}

void TrackInputSelect::onSetAudio(bool b) {
    if (b) {
        s_ui->inputType->setIcon(QIcon(":/icons/microphone.png"));
    } else {
        s_ui->inputType->setIcon(QIcon(":/icons/midi.png"));
    }
}

void TrackInputSelect::roundCorners() {
    QString ss = s_ui->inputType->styleSheet();
    ss.replace("border-top-right-radius: 0px;", "border-top-right-radius: 4px;");
    s_ui->inputType->setStyleSheet(ss);

    ss = s_ui->missile->styleSheet();
    ss.replace("border-bottom-right-radius: 0px;", "border-bottom-right-radius: 4px;");
    s_ui->missile->setStyleSheet(ss);
}

void TrackInputSelect::squareCorners() {
    QString ss = s_ui->inputType->styleSheet();
    ss.replace("border-top-right-radius: 4px;", "border-top-right-radius: 0px;");
    s_ui->inputType->setStyleSheet(ss);

    ss = s_ui->missile->styleSheet();
    ss.replace("border-bottom-right-radius: 4px;", "border-bottom-right-radius: 0px;");
    s_ui->missile->setStyleSheet(ss);
}

void TrackInputSelect::enableAddTrackButton() {
    s_ui->inputNew->setEnabled(true);
}
