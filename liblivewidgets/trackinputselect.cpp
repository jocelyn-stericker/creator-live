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
  : live_widgets::ObjectChooser(parent)
  , s_ui(new Ui_Frame)
  , s_allowMidi(allowMidi)
  , s_allowAudio(allowAudio)
  , all()
  , b_trackName("No Input")
  , b_audio(allowAudio)
  { s_ui->setupUi(this);

    setMinimizedButton(s_ui->inputType);
    setTopFrame(s_ui->inputType);
    setBottomFrame(s_ui->missile);
    setAlignedLeft(true);

    connect(s_ui->input_combo,SIGNAL(clicked(QModelIndex)),s_ui->input_combo,SLOT(setCurrentIndex(QModelIndex)));
    connect(s_ui->input_combo,SIGNAL(clicked(QModelIndex)),this,SLOT(activateSelected()));
    connect(s_ui->input_combo,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(activateSelected()));

    connect(s_ui->inputNew, SIGNAL(clicked()), this, SIGNAL(newOutputRequested()));

    all.push_back("0");
    updateObjects();

    MidiBindingQtSys::addWidget(this);

    connect(s_ui->inputType, SIGNAL(toggled(bool)), this, SLOT(setMaximized(bool)));

    s_ui->inputName->setText(b_trackName);
    connect(&b_trackName, SIGNAL(changeObserved(QString,QString)), s_ui->inputName, SLOT(setText(QString)));
    connect(&b_audio, SIGNAL(changeObserved(bool,bool)), this, SLOT(onSetAudio(bool)));
    connect(&b_audio, SIGNAL(changeObserved(bool,bool)), s_ui->inputPaint, SLOT(setHidden(bool)));

    setFixedWidth(53);

    if (popout) {
        s_ui->inputPaint->hide();
        s_ui->inputNew->hide();
        setMaximized();
    } else {
        setMinimized();
    }
}

void TrackInputSelect::updateObjects() {
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
}

void TrackInputSelect::activateSelected() {
    b_trackName = object::get(InputOnly|NoRefresh)[s_ui->input_combo->currentRow()]->name();
    if (width() != 55) {
        setMinimized();
        QTimer::singleShot(205, this, SLOT(activateSelected()));
    } else {
        emit objectChosen(object::get(InputOnly|NoRefresh)[s_ui->input_combo->currentRow()]);
    }
}

void TrackInputSelect::onSetAudio(bool b) {
    if (b) {
        s_ui->inputType->setIcon(QIcon(":/icons/microphone.png"));
    } else {
        s_ui->inputType->setIcon(QIcon(":/icons/midi.png"));
    }
}

void TrackInputSelect::enableAddTrackButton() {
    s_ui->inputNew->setEnabled(true);
}
