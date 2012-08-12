/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "midioutputchooser.h"
#include "ui_midioutputchooser.h"

#include <live/instrument>
#include <live/instrumentinterface>
#include <live_widgets/midibindingqt.h>

#include <QPropertyAnimation>
#include <QButtonGroup>
#include <QTimer>

using namespace live;
using namespace live_widgets;

MidiOutputChooser::MidiOutputChooser(QWidget *parent)
  : QWidget(parent)
  , s_busy(false)
  , s_ui(new Ui::MidiOutputChooser)
{
    s_ui->setupUi(this);

    s_ui->Bwidget->clear();
    for (int i = 0; i < instrument::interfaces().size(); ++i) {
        s_ui->Bwidget->insertItem(i, instrument::names()[i]);
    }

    connect(s_ui->Bwidget,SIGNAL(clicked(QModelIndex)),s_ui->Bwidget,SLOT(setCurrentIndex(QModelIndex)));
    connect(s_ui->Bwidget,SIGNAL(clicked(QModelIndex)),this,SLOT(step2()));
    connect(s_ui->Bwidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(step2()));

    MidiBindingQtSys::addWidget(this);

    setFixedWidth(55);
    maximize();

    connect(s_ui->inputType, SIGNAL(toggled(bool)), this, SLOT(minimize(bool)));
    connect(s_ui->toolButton_settings, SIGNAL(toggled(bool)), this, SLOT(minimize(bool)));
    s_ui->toolButton_settings->hide();

    QButtonGroup* bg = new QButtonGroup(this);
    bg->setExclusive(true);
    bg->addButton(s_ui->inputType);
    bg->addButton(s_ui->toolButton_settings);
}

MidiOutputChooser::~MidiOutputChooser()
{
    delete s_ui;
}

void MidiOutputChooser::resizeEvent(QResizeEvent *e) {
    emit resized();
    if (e) QWidget::resizeEvent(e);
}

void MidiOutputChooser::step1()
{
    s_busy = true;
    if (width() != 55) {
        minimize();
        QTimer::singleShot(200, this, SLOT(step1()));
    } else {
        s_ui->stackedWidget->setCurrentIndex(0);
        maximize();
        s_ui->inputType->setChecked(true);
        s_busy = false;
    }
}

void MidiOutputChooser::step2()
{
    if (s_ui->Bwidget->currentRow() == -1)
        return;

    s_busy = true;
    if (width() != 55) {
        minimize();
        QTimer::singleShot(200, this, SLOT(step2()));
    } else {
        s_ui->stackedWidget->setCurrentIndex(1);
        maximize();
        s_ui->inputType->setChecked(false);
        s_ui->toolButton_settings->show();
        s_ui->toolButton_settings->setChecked(true);
        if (!s_ui->customPage->layout())
            s_ui->customPage->setLayout(new QHBoxLayout(s_ui->customPage));
        QObjectList l = s_ui->customPage->children();
        while (l.size()) {
            if (!qobject_cast<QLayout*>(l.first()))
                delete l.takeFirst();
            else
                l.removeFirst();
        }

        s_ui->customPage->layout()->setContentsMargins(0, 0, 0, 0);
        QWidget* w;
        s_ui->customPage->layout()->addWidget(w = instrument::interfaces()[s_ui->Bwidget->currentRow()]->selectionWidget(ObjectPtr(), ObjectPtr()));
        w->setParent(s_ui->customPage);
        connect(w, SIGNAL(instrumentUpdated(live::ObjectPtr, live::ObjectPtr)), this, SIGNAL(outputChosen(live::ObjectPtr, live::ObjectPtr)));
        connect(w, SIGNAL(instrumentUpdated(live::ObjectPtr, live::ObjectPtr)), this, SLOT(deleteLater()));

        s_busy = false;
    }
}

void MidiOutputChooser::minimize(bool reverse, QObject *senderObject) {
    if (!senderObject && !s_busy)
        senderObject = sender();

    if (senderObject == s_ui->inputType || senderObject == s_ui->toolButton_settings) {
        if ((senderObject == s_ui->inputType && !reverse)
         || (senderObject == s_ui->toolButton_settings && reverse))
            step2();

        if ((senderObject == s_ui->inputType && reverse)
         || (senderObject == s_ui->toolButton_settings && !reverse))
            step1();

        return;
    }

    QPropertyAnimation* qaa[2];
    qaa[0] = new QPropertyAnimation(this, "minimumWidth");
    qaa[1] = new QPropertyAnimation(this, "maximumWidth");
    for (int i = 0; i < 2; ++i) {
        qaa[i]->setStartValue(width());
        qaa[i]->setEndValue(reverse ? (s_ui->stackedWidget->currentIndex() ? 750 : 400) : 55);
        qaa[i]->setDuration(200);
        qaa[i]->setEasingCurve(QEasingCurve::InQuad);
        qaa[i]->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

