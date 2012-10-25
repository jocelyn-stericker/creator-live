/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "tracktab.h"
#include "ui_tracktab.h"

TrackTab::TrackTab(QWidget *parent)
  : QWidget(parent)
  , live_widgets::BindableParent(this)
  , ui(new Ui::tracktab)
{
    ui->setupUi(this);
}

TrackTab::~TrackTab()
{
    delete ui;
}
