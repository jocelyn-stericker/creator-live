/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "effectstab.h"
#include "ui_effectstab.h"

#include <live/audio>
#include <live/midi>
#include "samplerdj.h"

using namespace live;

EffectsTab::EffectsTab(QWidget *parent)
  : QWidget(parent)
  , live_widgets::BindableParent(this)
  , s_samplerL(*new SamplerDJ)
  , s_samplerR(*new SamplerDJ)
  , ui(new Ui::effectstab)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            s_times[i][j] = 0;
            s_rec[i][j] = 0;
        }
    }
    s_samplerL.setMultiMode(true);
    s_samplerR.setMultiMode(true);
    s_samplerL.setRecordMode(false);
    s_samplerR.setRecordMode(false);

    ui->setupUi(this);

    connect(ui->toolButton_L1,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L2,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L3,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L4,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L5,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L6,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L7,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L8,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R1,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R2,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R3,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R4,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R5,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R6,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R7,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R8,SIGNAL(pressed()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L1,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L2,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L3,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L4,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L5,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L6,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L7,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_L8,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R1,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R2,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R3,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R4,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R5,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R6,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R7,SIGNAL(released()),this,SLOT(buttonLogic()));
    connect(ui->toolButton_R8,SIGNAL(released()),this,SLOT(buttonLogic())); //ROTFLOL
}

EffectsTab::~EffectsTab()
{
    delete ui;
}

static void removeBackground(QString& style)
{
    if (style.contains("background-color"))
    {
        int start=style.indexOf("background-color");
        int end=style.indexOf(";",start);
        if (start!=end&&start!=-1) style.remove(start,end-start);
    }
}

void EffectsTab::buttonLogic()
{
    Q_ASSERT(sender());
    QToolButton* button=qobject_cast<QToolButton*>(sender());
    Q_ASSERT(button);

    QString name=sender()->objectName();
    name.remove("toolButton_");
    Q_ASSERT(name.size());
    Q_ASSERT(name[0] == 'L' || name[0] == 'R');
    int chan = (name[0] == 'L' ? 1 : 0); // CHAN refers to either L or R screen, not physical chan
    name.remove(0, 1);
    bool ok=1;
    int num=name.toInt(&ok);
    Q_ASSERT(ok);
    if (qobject_cast<QToolButton*>(sender())->isDown())
    {
        QString style=button->styleSheet();

        removeBackground(style);
        style=style+"background-color:"+(s_rec[num - 1][chan]?"red":"green")+";";
        button->setStyleSheet(style);

        (chan ? s_samplerL : s_samplerR).hit(num-1);
        s_times[num-1][chan]=midi::getTime_msec();
    }
    else if ((chan ? s_samplerL : s_samplerR).isPlayMode()&&midi::getTime_msec()-s_times[num-1][chan]<100)
    {
        // play it until completion
        // FIXME : add timer
//        (offset ? s_samplerL : s_samplerR).s_audioTracks[num-1][offset]->length();
    }
    else
    {
        QString style=button->styleSheet();
        removeBackground(style);
        button->setStyleSheet(style);
        s_times[num-1][chan]=0;
        (chan ? s_samplerL : s_samplerR).release(num-1);
    }
}
