/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "livebar.h"
#include "ui_livebar.h"

LiveBar::LiveBar(QWidget *parent)
  : QWidget(parent)
  , live_widgets::BindableParent(this)
  , ui(new Ui::LiveBar)
{
    ui->setupUi(this);
    connect(ui->comboBox_mode,SIGNAL(activated(int)),this,SLOT(modeChosen(int)));
}

LiveBar::~LiveBar()
{
    delete ui;
}

void LiveBar::modeChosen(int z)
{
    switch (z)
    {
    case 0:
        emit noMode();
        break;
    case 1:
        emit djMode();
        break;
    case 2:
        emit trackMode();
        break;
    case 3:
        emit sampleMode();
        break;
    case 4:
        emit collectionMode();
        break;
    }
}
