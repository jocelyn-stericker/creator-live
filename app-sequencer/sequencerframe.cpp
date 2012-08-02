/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "looperframe.h"
#include "sequencerframe.h"
#include "ui_sequencerframe.h"
#include "live_widgets/midibindingqt.h"
#include "live/asyncconnect.h"

#include <QtPlugin>
#include <QMenu>

using namespace live;
using namespace live_widgets;

SequencerFrame::SequencerFrame(SequencerApp* backend,QWidget *parent) :
    AppFrame(parent),
    app(*backend),
    graph(*(new SequencerGraph(this,&app))),
    ui(new Ui::SequencerFrame)
{
    ui->setupUi(this);

    async_connect(&app.s_midiTrack->b_mute,SIGNAL(changeObserved(bool,bool)),ui->mute,SLOT(setChecked(bool)));
    async_connect(&app.s_midiTrack->b_playback,SIGNAL(changeObserved(bool,bool)),ui->play,SLOT(setChecked(bool)));
    async_connect(&app.s_midiTrack->b_record,SIGNAL(changeObserved(bool,bool)),ui->record,SLOT(setChecked(bool)));
    async_connect(&app.b_clipped,SIGNAL(changeObserved(bool,bool)),ui->clip,SLOT(setChecked(bool)));

    async_connect(ui->mute,SIGNAL(clicked(bool)),this,SLOT(logicMute(bool)));
    async_connect(ui->play,SIGNAL(clicked(bool)),this,SLOT(logicPlay(bool)));
    async_connect(ui->record,SIGNAL(clicked(bool)),this,SLOT(logicRecord(bool)));
    async_connect(ui->clip,SIGNAL(clicked(bool)),&app,SLOT(setClipped(bool)));

    connect(ui->rightButton,SIGNAL(clicked()),&graph,SLOT(incrScroll()));
    connect(ui->leftButton,SIGNAL(clicked()),&graph,SLOT(decrScroll()));

    connect(ui->horizontalSlider_scale,SIGNAL(valueChanged(int)),&app,SLOT(setScale(int)));
    connect(&app,SIGNAL(scaleChanged(int)),&graph,SLOT(setScale(int)));

    ui->visLayout->addWidget(&graph);
    MidiBindingQtSys::addWidget(this);
    syncState();

    ui->menu->setText("Track "+QString::number(app.s_id));
    ui->menu->setMenu(action_menu=new QMenu(this));
    ui->menu->menu()->addAction(action_import=new QAction("Import",this));
    connect(action_import,SIGNAL(triggered()),this,SLOT(logicImport()));
    ui->menu->menu()->addAction(action_export=new QAction("Export",this));
    connect(action_export,SIGNAL(triggered()),this,SLOT(logicExport()));
    ui->menu->menu()->addAction(action_newToolchain=new QAction("Create new effect toolchain for track",this));

    setObjectName("SequencerFrame_"+QString::number(app.s_id));
    ui->clip->setObjectName("clip");
    ui->play->setObjectName("play");
    ui->mute->setObjectName("mute");
    ui->record->setObjectName("record");

    ui->leftButton->hide();
    ui->rightButton->hide();
    ui->horizontalFrame->hide();
}

SequencerFrame::~SequencerFrame()
{
    delete ui;
    app.deleteLater();
}

void SequencerFrame::logicMute(bool x)
{
    if(!app.isMute()&&x) app.startMute();
    else if(app.isMute()&&!x) app.stopMute();
}

void SequencerFrame::logicPlay(bool x)
{
    if(!app.isPlaying()&&x) app.startPlayback();
    else if(app.isPlaying()&&!x) app.stopPlayback();
}

void SequencerFrame::logicRecord(bool x)
{
    if(!app.isRecord()&&x) app.startRecord();
    else if(app.isRecord()&&!x) app.stopRecord();
}

void SequencerFrame::logicImport()
{
    QString file=QFileDialog::getOpenFileName(this,QString("Import file"),QString(),"Audio Files (*.wav *.flac *.ogg);;MIDI Files (*.MIDI *.midi *.MID *.mid)");
    if(QFile::exists(file)&&(file.endsWith("flac")||file.endsWith("ogg")||file.endsWith("wav")))
    {
        app.s_audioTrack->importFile(file);
    }
    else if(QFile::exists(file)&&(file.endsWith("midi")||file.endsWith("MIDI")||file.endsWith("mid")||file.endsWith("MID")))
    {
        app.s_midiTrack->importFile(file);
    }
}
void SequencerFrame::logicExport()
{
    QString file=QFileDialog::getSaveFileName(this,QString("Export file"),QString(),"Audio Files (*.wav *.flac *.ogg);;MIDI Files (*.MIDI *.midi *.MID *.mid)");
    if(file.endsWith("flac")||file.endsWith("ogg")||file.endsWith("wav"))
    {
        app.s_audioTrack->exportFile(file);
    }
    else if(file.endsWith("midi")||file.endsWith("MIDI")||file.endsWith("mid")||file.endsWith("MID"))
    {
        app.s_midiTrack->exportFile(file);
    }
}

void SequencerFrame::syncState()
{
    if(app.isMute())
    {
        ui->mute->setChecked(1);
    }
    if(app.isOverdub()||app.isRecord())
    {
        ui->record->setChecked(1);
    }
    if(app.isPlaying())
    {
        ui->play->setChecked(1);
    }
    ui->horizontalSlider_scale->setValue(app.scale());

    ui->clip->setObjectName("clip");
    ui->play->setObjectName("play");
    ui->mute->setObjectName("mute");
    ui->record->setObjectName("record");
}

void SequencerFrame::toggleMinimized()
{
    ui->frame->setVisible(!ui->frame->isVisible());
    graph.updateAudioData();
    graph.updateMidiData();
    AppFrame::toggleMinimized();
}

live::AppInterface* SequencerCreator::next()
{
    return s_looper=(s_looper?s_looper:new LooperCreator);
}

Q_EXPORT_PLUGIN2(live::AppInterface, SequencerCreator)
