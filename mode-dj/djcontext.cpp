/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "djcontext.h"
#include "ui_djcontext.h"

#include "collectioncontext.h"
#include "effectstab.h"
#include "sampletab.h"
#include "livebar.h"
#include "tracktab.h"

#include <QtPlugin>

using namespace live;

DJContext::DJContext(QWidget *parent)
    : QWidget(parent)
    , s_et(new EffectsTab)
    , s_st(new SampleTab)
    , s_tt(new TrackTab)
    , s_lb(new LiveBar)
    , s_cc(new CollectionContext(this))
    , ui(new Ui::Ui_DJContext)
{
    ui->setupUi(this);
    ui->tmpWidget->setLayout(new QHBoxLayout);  // tmp
    ui->tmpWidget->layout()->setContentsMargins(0,0,0,0);   // tmp
    ui->tmpWidget->layout()->addWidget(s_lb);    //tmp

    ui->modeContainer->layout()->addWidget(s_et);
    ui->modeContainer->layout()->addWidget(s_st);
    ui->modeContainer->layout()->addWidget(s_tt);
    ui->modeContainer->layout()->addWidget(s_cc);

    connect(s_lb, SIGNAL(noMode()), this, SLOT(showSnarky()));
    connect(s_lb, SIGNAL(djMode()), this, SLOT(showEffects()));
    connect(s_lb, SIGNAL(sampleMode()), this, SLOT(showSample()));
    connect(s_lb, SIGNAL(trackMode()), this, SLOT(showTrack()));
    connect(s_lb, SIGNAL(collectionMode()), this, SLOT(showCollection()));

    showSnarky();
}

DJContext::~DJContext()
{
    delete ui;
}

void DJContext::showSnarky()
{
    s_et->hide();
    s_st->hide();
    s_tt->hide();
    s_cc->hide();
    ui->label_snarky->show();
}

void DJContext::showEffects()
{
    s_et->show();
    s_st->hide();
    s_tt->hide();
    s_cc->hide();
    ui->label_snarky->hide();
}

void DJContext::showSample()
{
    s_et->hide();
    s_st->show();
    s_tt->hide();
    s_cc->hide();
    ui->label_snarky->hide();
}

void DJContext::showTrack()
{
    s_et->hide();
    s_st->hide();
    s_tt->show();
    s_cc->hide();
    ui->label_snarky->hide();
}

void DJContext::showCollection()
{
    s_et->hide();
    s_st->hide();
    s_tt->hide();
    s_cc->show();
    ui->label_snarky->hide();
}

void DJContext::audioanged_logic(QString s)
{
    s_out = object::fetch(s, AudioOnly | OutputOnly);
}

void DJContext::monitorChanged_logic(QString)
{
    // FIXME
}

Q_EXPORT_PLUGIN2(live::ModeInterface, DJModeInterface)
