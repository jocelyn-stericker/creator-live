/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/newinput.h>
#include <live/audio>
#include "ui_newinput.h"
#include <cmath>
#include <math.h>
#include <QPropertyAnimation>
#include <QTimer>

using namespace live_widgets;
using namespace live;

NewInput::NewInput(QWidget*parent)
  : QFrame(parent)
  , s_ui(new Ui_Frame) {
    s_ui->setupUi(this);

    connect(s_ui->input_combo,SIGNAL(clicked(QModelIndex)),s_ui->input_combo,SLOT(setCurrentIndex(QModelIndex)));
    connect(s_ui->input_combo,SIGNAL(clicked(QModelIndex)),this,SLOT(go()));
    connect(s_ui->input_combo,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(go()));
    connect(s_ui->input_combo,SIGNAL(currentRowChanged(int)),this,SLOT(rowChangedEvent()));

    all.push_back("0");
    internalStockChangedEvent();

    MidiBindingQtSys::addWidget(this);

    connect(object::singleton(),SIGNAL(stockChanged()),this,SLOT(internalStockChangedEvent()));
    rowChangedEvent();

    setFixedWidth(55);
    maximize();
    connect(s_ui->inputType, SIGNAL(toggled(bool)), this, SLOT(minimize(bool)));
}

void NewInput::internalStockChangedEvent() {
    QList<ObjectPtr> list = object::get(InputOnly|NoRefresh);
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

void NewInput::go() {
    if (width() != 55) {
        minimize();
        QTimer::singleShot(200, this, SLOT(go()));
    } else {
        emit created(object::get(InputOnly|NoRefresh)[s_ui->input_combo->currentRow()]);
        deleteLater();
    }
}

void NewInput::refresh() {
    midi::refresh();
    audio::refresh();
    internalStockChangedEvent();
}

void NewInput::rowChangedEvent() {
//    s_ui->pushButton_create->setEnabled(s_ui->input_combo->currentRow()!=-1);
}

void NewInput::resizeEvent(QResizeEvent *e) {
//     int w=width()-s_ui->line_4->width();
//     s_ui->Awidget->setFixedWidth(w/2);
//     float phi=(1+sqrt(5.0))/2.0;
//     s_ui->Bwidget->setFixedWidth(w/2.00/phi);
//     s_ui->Cwidget->setFixedWidth(w/2.00/phi/phi);

     if (e)
         QFrame::resizeEvent(e);
}

void NewInput::minimize(bool reverse) {
    QPropertyAnimation* qaa[2];
    qaa[0] = new QPropertyAnimation(this, "minimumWidth");
    qaa[1] = new QPropertyAnimation(this, "maximumWidth");
    for (int i = 0; i < 2; ++i) {
        qaa[i]->setStartValue(width());
        qaa[i]->setEndValue(reverse ? 400 : 55);
        qaa[i]->setDuration(200);
        qaa[i]->setEasingCurve(QEasingCurve::InQuad);
        qaa[i]->start(QAbstractAnimation::DeleteWhenStopped);
    }
}
