/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                ( ( ( MIDI PLUGIN ) ) )

*******************************************************/

#include "midiwizard.h"
#include "ui_midiwizard.h"

#include "midiplugin.h"

#include <live/audio>
#include <live/midievent>
#include <live/midi>

#include <QFile>

MidiWizard::MidiWizard(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::MidiWizard)
{
    ui->setupUi(this);
    setCurrentIndex(0);

    QStringList insts;
    QFile f(":/sf2/programs");
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << f.errorString();
    }
    while (!f.atEnd()) {
        insts.push_back(f.readLine());
    }
    ui->listWidget_programs->insertItems(0,insts);

    connect(live::object::singleton(), SIGNAL(stockChanged()), this, SLOT(updateDevices()));
    updateDevices();

    connect(ui->toolButton_back, SIGNAL(clicked()), this, SLOT(prev()));
    connect(ui->toolButton_back_2, SIGNAL(clicked()), this, SLOT(prev()));
    connect(ui->toolButton_back_3, SIGNAL(clicked()), this, SLOT(prev()));
    connect(ui->toolButton_back_4, SIGNAL(clicked()), this, SLOT(prev()));

    connect(ui->toolButton_next, SIGNAL(clicked()), this, SLOT(next()));
    connect(ui->toolButton_next_2, SIGNAL(clicked()), this, SLOT(next()));
    connect(ui->toolButton_next_3, SIGNAL(clicked()), this, SLOT(next()));
    connect(ui->toolButton_next_4, SIGNAL(clicked()), this, SLOT(next()));

    connect(ui->listWidget_midi, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(next()));
    connect(ui->listWidget_midi, SIGNAL(clicked(QModelIndex)), this, SLOT(next()));
    connect(ui->listWidget_out, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(next()));
    connect(ui->listWidget_out, SIGNAL(clicked(QModelIndex)), this, SLOT(next()));
    connect(ui->listWidget_programs, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(next()));
    connect(ui->listWidget_programs, SIGNAL(clicked(QModelIndex)), this, SLOT(next()));
    connect(ui->listWidget_return, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(next()));
    connect(ui->listWidget_return, SIGNAL(clicked(QModelIndex)), this, SLOT(next()));
}

MidiWizard::~MidiWizard()
{
    delete ui;
}

void MidiWizard::next()
{
    if (currentIndex() == 0 && ui->listWidget_midi->currentRow() == -1)
        return;

    if (currentIndex() == 1 && ui->listWidget_programs->currentRow() == -1)
        return;

    if (currentIndex() == 2 && ui->listWidget_return->currentRow() == -1)
        return;

    if (currentIndex() == 2 && !ui->listWidget_return->currentRow()) {
        emit instrumentUpdated(m_midiIn, m_aOut = live::midi::null());
        return;
    }

    if (currentIndex() == 3 && ui->listWidget_out->currentRow() == -1) {
        return;
    }

    if (currentIndex() == 0)
        m_midiIn = m_midiIns[ui->listWidget_midi->currentRow()];

    if (currentIndex() == 1) {
        live::ObjectChain p;
        p.push_back(live::midi::null());
        live::Event e;
        e.setChan(ui->comboBox_chan->currentIndex());
        e.setSimpleStatus(live::Event::PROGRAM_CHANGE);
        e.setNote(ui->listWidget_programs->currentRow());
        m_midiIn->mIn(&e, &p);
    }

    if (currentIndex() == 2) {
        m_return = m_returns[ui->listWidget_return->currentRow() - 1];
        // -1 because of "No Return"
    }

    if (currentIndex() == 3) {
        m_aOut = m_aOuts[ui->listWidget_out->currentRow()];
        emit instrumentUpdated(new MidiPluginObject(m_midiIn, m_return), m_aOut);
    }

    if (currentIndex() + 1 != count())
        setCurrentIndex(currentIndex() + 1);
}

void MidiWizard::prev()
{
    if (currentIndex())
        setCurrentIndex(currentIndex() - 1);
}

void MidiWizard::updateDevices() {
    m_aOuts = live::object::get(live::AudioOnly | live::OutputOnly);
    ui->listWidget_out->clear();
    for (int i = 0; i < m_aOuts.size(); ++i) {
        ui->listWidget_out->addItem(m_aOuts[i]->name());
    }

    m_midiIns = live::object::get(live::MidiOnly | live::OutputOnly);
    ui->listWidget_midi->clear();
    for (int i = 0; i < m_midiIns.size(); ++i) {
        ui->listWidget_midi->addItem(m_midiIns[i]->name());
    }

    m_returns = live::object::get(live::AudioOnly | live::InputOnly);
    ui->listWidget_return->clear();
    ui->listWidget_return->addItem("No Return");
    for (int i = 0; i < m_returns.size(); ++i) {
        ui->listWidget_return->addItem(m_returns[i]->name());
    }
}
