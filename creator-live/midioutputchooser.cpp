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
#include <QTimer>

using namespace live;
using namespace live_widgets;

MidiOutputChooser::MidiOutputChooser(QWidget *parent)
  : live_widgets::ObjectChooser(parent)
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
    connect(&b_trackName, SIGNAL(changeObserved(QString,QString)), s_ui->inputName, SLOT(setText(QString)));

    MidiBindingQtSys::addWidget(this);

    setFixedWidth(55);
    setMaximized();

    s_ui->toolButton_settings->hide();

    connect(s_ui->inputType, SIGNAL(toggled(bool)), this, SLOT(clickedLogic(bool)));
    connect(s_ui->toolButton_settings, SIGNAL(toggled(bool)), this, SLOT(clickedLogic(bool)));
}

MidiOutputChooser::~MidiOutputChooser()
{
    delete s_ui;
}

void MidiOutputChooser::step1()
{
    s_busy = true;
    if (width() != 55) {
        setMinimized();
        QTimer::singleShot(200, this, SLOT(step1()));
    } else {
        s_ui->stackedWidget->setCurrentIndex(0);
        setMaximized();
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
        setMinimized();
        QTimer::singleShot(200, this, SLOT(step2()));
    } else {
        s_ui->stackedWidget->setCurrentIndex(1);
        setMaximized();
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
        connect(w, SIGNAL(instrumentUpdated(live::ObjectPtr, live::ObjectPtr)), this, SIGNAL(objectChosen(live::ObjectPtr, live::ObjectPtr)));
        connect(w, SIGNAL(instrumentUpdated(live::ObjectPtr, live::ObjectPtr)), this, SLOT(setMinimized()));

        s_busy = false;
    }
}

void MidiOutputChooser::setMinimized(bool minimized) {
    QObject *senderObject = 0;
    if (!s_busy)
        senderObject = sender();

    if (senderObject == s_ui->inputType || senderObject == s_ui->toolButton_settings) {
        if ((senderObject == s_ui->inputType && minimized)
         || (senderObject == s_ui->toolButton_settings && !minimized))
            step2();

        if ((senderObject == s_ui->inputType && !minimized)
         || (senderObject == s_ui->toolButton_settings && minimized))
            step1();

        return;
    }

    QPropertyAnimation* qaa;
    qaa = new QPropertyAnimation(this, "fixedWidth");
    qaa->setStartValue(width());
    qaa->setEndValue(minimized ? 55 : (s_ui->stackedWidget->currentIndex() ? 750 : 400));
    qaa->setDuration(200);
    qaa->setEasingCurve(QEasingCurve::InQuad);
    qaa->start(QAbstractAnimation::DeleteWhenStopped);
    if (minimized) {
        s_ui->inputType->setChecked(false);
        s_ui->toolButton_settings->setChecked(false);
    }
}

void MidiOutputChooser::checkedLogic(bool) {
    if (sender() == s_ui->inputType) {
        if (s_ui->inputType->isChecked()) {
            s_ui->toolButton_settings->setChecked(false);
            setMinimized(false);
        }
        else setMinimized();
    }

    if (sender() == s_ui->toolButton_settings) {
        if (s_ui->toolButton_settings->isChecked()) {
            s_ui->inputType->setChecked(false);
            setMinimized(false);
        }
        else setMinimized();
    }
}

void MidiOutputChooser::updateObjects() {
    // interfaces don't change... yet...
}

