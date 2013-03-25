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
  , m_busy(false)
  , m_ui(new Ui::MidiOutputChooser)
{
    m_ui->setupUi(this);

    m_ui->Bwidget->clear();
    for (int i = 0; i < instrument::interfaces().size(); ++i) {
        m_ui->Bwidget->insertItem(i, instrument::names()[i]);
    }

    connect(m_ui->Bwidget,SIGNAL(clicked(QModelIndex)),m_ui->Bwidget,SLOT(setCurrentIndex(QModelIndex)));
    connect(m_ui->Bwidget,SIGNAL(clicked(QModelIndex)),this,SLOT(step2()));
    connect(m_ui->Bwidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(step2()));
    connect(&b_trackName, SIGNAL(changeObserved(QString,QString)), m_ui->inputName, SLOT(setText(QString)));

    binding::addWidget(this);

    setFixedWidth(55);
    setMaximized();

    m_ui->toolButton_settings->hide();

    connect(m_ui->inputType, SIGNAL(clicked(bool)), this, SLOT(checkedLogic(bool)));
    connect(m_ui->toolButton_settings, SIGNAL(clicked(bool)), this, SLOT(checkedLogic(bool)));
}

MidiOutputChooser::~MidiOutputChooser()
{
    delete m_ui;
}

void MidiOutputChooser::step1()
{
    m_busy = true;
    if (width() != 55) {
        setMinimized();
        QTimer::singleShot(200, this, SLOT(step1()));
    } else {
        m_ui->stackedWidget->setCurrentIndex(0);
        setMaximized();
        m_ui->inputType->setChecked(true);
        m_busy = false;
    }
}

void MidiOutputChooser::step2()
{
    if (m_ui->Bwidget->currentRow() == -1)
        return;

    m_busy = true;
    if (width() != 55) {
        setMinimized();
        QTimer::singleShot(200, this, SLOT(step2()));
    } else {
        m_ui->stackedWidget->setCurrentIndex(1);
        setMaximized();
        m_ui->inputType->setChecked(false);
        m_ui->toolButton_settings->show();
        m_ui->toolButton_settings->setChecked(true);
        if (!m_ui->customPage->layout())
            m_ui->customPage->setLayout(new QHBoxLayout(m_ui->customPage));
        QObjectList l = m_ui->customPage->children();
        while (l.size()) {
            if (!qobject_cast<QLayout*>(l.first()))
                delete l.takeFirst();
            else
                l.removeFirst();
        }

        m_ui->customPage->layout()->setContentsMargins(0, 0, 0, 0);
        QWidget* w;
        m_ui->customPage->layout()->addWidget(w = instrument::interfaces()[m_ui->Bwidget->currentRow()]->selectionWidget(ObjectPtr(), ObjectPtr()));
        w->setParent(m_ui->customPage);
        connect(w, SIGNAL(instrumentUpdated(live::ObjectPtr, live::ObjectPtr)), this, SIGNAL(objectChosen(live::ObjectPtr, live::ObjectPtr)));
        connect(w, SIGNAL(instrumentUpdated(live::ObjectPtr, live::ObjectPtr)), this, SLOT(setMinimized()));

        m_busy = false;
    }
}

void MidiOutputChooser::setMinimized(bool minimized) {
    QObject *senderObject = 0;
    if (!m_busy)
        senderObject = sender();

    if (( m_ui->inputType && senderObject == m_ui->inputType) || (m_ui->toolButton_settings && senderObject == m_ui->toolButton_settings)) {
        if ((senderObject == m_ui->inputType && minimized)
         || (senderObject == m_ui->toolButton_settings && !minimized))
            step2();

        if ((senderObject == m_ui->inputType && !minimized)
         || (senderObject == m_ui->toolButton_settings && minimized))
            step1();

        return;
    }

    QPropertyAnimation* qaa;
    qaa = new QPropertyAnimation(this, "fixedWidth");
    qaa->setStartValue(width());
    qaa->setEndValue(minimized ? 55 : (m_ui->stackedWidget->currentIndex() ? 750 : 400));
    qaa->setDuration(200);
    qaa->setEasingCurve(QEasingCurve::InQuad);
    qaa->start(QAbstractAnimation::DeleteWhenStopped);
    if (minimized) {
        m_ui->inputType->setChecked(false);
        m_ui->toolButton_settings->setChecked(false);
    }
}

void MidiOutputChooser::checkedLogic(bool b) {
    m_busy = true;
    if (!b) {
        setMinimized();
        m_busy = false;
        return;
    }
    if (sender() == m_ui->inputType) {
        if (m_ui->inputType->isChecked()) {
            m_ui->toolButton_settings->setChecked(false);
            setMinimized(false);
        }
        else setMinimized();
    }

    if (sender() == m_ui->toolButton_settings) {
        if (m_ui->toolButton_settings->isChecked()) {
            m_ui->inputType->setChecked(false);
            setMinimized(false);
        }
        else setMinimized();
    }
    m_busy = false;
}

void MidiOutputChooser::updateObjects() {
    // interfaces don't change... yet...
}

