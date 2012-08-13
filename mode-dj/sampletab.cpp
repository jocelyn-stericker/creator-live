/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "sampletab.h"
#include "ui_sampletab.h"

SampleTab::SampleTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sampletab)
{
    ui->setupUi(this);
}

SampleTab::~SampleTab()
{
    delete ui;
}
