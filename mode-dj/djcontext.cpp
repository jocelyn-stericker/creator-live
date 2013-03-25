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
  , BindableParent(this)
  , m_et(new EffectsTab)
  , m_st(new SampleTab)
  , m_tt(new TrackTab)
  , m_lb(new LiveBar)
  , m_cc(new CollectionContext(this))
  , ui(new Ui::Ui_DJContext)
{
    ui->setupUi(this);
    ui->tmpWidget->setLayout(new QHBoxLayout);  // tmp
    ui->tmpWidget->layout()->setContentsMargins(0,0,0,0);   // tmp
    ui->tmpWidget->layout()->addWidget(m_lb);    //tmp

    ui->modeContainer->layout()->addWidget(m_et);
    ui->modeContainer->layout()->addWidget(m_st);
    ui->modeContainer->layout()->addWidget(m_tt);
    ui->modeContainer->layout()->addWidget(m_cc);

    connect(m_lb, SIGNAL(noMode()), this, SLOT(showSnarky()));
    connect(m_lb, SIGNAL(djMode()), this, SLOT(showEffects()));
    connect(m_lb, SIGNAL(sampleMode()), this, SLOT(showSample()));
    connect(m_lb, SIGNAL(trackMode()), this, SLOT(showTrack()));
    connect(m_lb, SIGNAL(collectionMode()), this, SLOT(showCollection()));

    showSnarky();
}

DJContext::~DJContext()
{
    delete ui;
}

void DJContext::showSnarky()
{
    m_et->hide();
    m_st->hide();
    m_tt->hide();
    m_cc->hide();
    ui->label_snarky->show();
}

void DJContext::showEffects()
{
    m_et->show();
    m_st->hide();
    m_tt->hide();
    m_cc->hide();
    ui->label_snarky->hide();
}

void DJContext::showSample()
{
    m_et->hide();
    m_st->show();
    m_tt->hide();
    m_cc->hide();
    ui->label_snarky->hide();
}

void DJContext::showTrack()
{
    m_et->hide();
    m_st->hide();
    m_tt->show();
    m_cc->hide();
    ui->label_snarky->hide();
}

void DJContext::showCollection()
{
    m_et->hide();
    m_st->hide();
    m_tt->hide();
    m_cc->show();
    ui->label_snarky->hide();
}

void DJContext::audioanged_logic(QString s)
{
    m_out = object::fetch(s, AudioOnly | OutputOnly);
}

void DJContext::monitorChanged_logic(QString)
{
    // FIXME
}
