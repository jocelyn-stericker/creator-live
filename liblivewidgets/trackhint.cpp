/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/trackhint.h>
#include "ui_trackhint.h"

using namespace live_widgets;

TrackHint::TrackHint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackHint) {
    ui->setupUi(this);
}

TrackHint::~TrackHint() {
    delete ui;
}
