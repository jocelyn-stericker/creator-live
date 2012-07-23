#include "djcontext.h"
#include "ui_djcontext.h"
#include "livebar.h"

#include "effectstab.h"
#include "sampletab.h"
#include "tracktab.h"

djcontext::djcontext(QWidget *parent)
    : QWidget(parent)
    , s_et(new effectstab)
    , s_st(new sampletab)
    , s_tt(new tracktab)
    , s_lb(new LiveBar)
    , ui(new Ui::Ui_DJContext)
{
    ui->setupUi(this);
    ui->tmpWidget->setLayout(new QHBoxLayout);  // tmp
    ui->tmpWidget->layout()->setContentsMargins(0,0,0,0);   // tmp
    ui->tmpWidget->layout()->addWidget(s_lb);    //tmp

    ui->modeContainer->layout()->addWidget(s_et);
    ui->modeContainer->layout()->addWidget(s_st);
    ui->modeContainer->layout()->addWidget(s_tt);

    connect(s_lb, SIGNAL(djMode()), this, SLOT(showEffects()));
    connect(s_lb, SIGNAL(sampleMode()), this, SLOT(showSample()));
    connect(s_lb, SIGNAL(trackMode()), this, SLOT(showTrack()));

    showSnarky();
}

void djcontext::showSnarky()
{
    s_et->hide();
    s_st->hide();
    s_tt->hide();
    ui->label_snarky->show();
}

void djcontext::showEffects()
{
    s_et->show();
    s_st->hide();
    s_tt->hide();
    ui->label_snarky->hide();
}

void djcontext::showSample()
{
    s_et->hide();
    s_st->show();
    s_tt->hide();
    ui->label_snarky->hide();
}

void djcontext::showTrack()
{
    s_et->hide();
    s_st->hide();
    s_tt->show();
    ui->label_snarky->hide();
}

djcontext::~djcontext()
{
    delete ui;
}
