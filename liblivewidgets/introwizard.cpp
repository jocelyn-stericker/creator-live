/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/introwizard.h>
#include "ui_introwizard.h"
#include <cmath>
#include <math.h>
#include <QDebug>

using namespace live_widgets;

IntroWizard::IntroWizard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IntroWizard) {
    ui->setupUi(this);
    connect(ui->commandLinkButton_live,SIGNAL(clicked()),this,SIGNAL(standardRequested()));
    connect(ui->pushButton_quit,SIGNAL(clicked()),this,SIGNAL(quitRequested()));
    connect(ui->pushButton_continue,SIGNAL(clicked()),this,SIGNAL(openRequested()));
    resizeEvent(0);
}

IntroWizard::~IntroWizard() {
    delete ui;
}

void IntroWizard::resizeEvent(QResizeEvent *e) {
    float w=float(width())-float(ui->line_3->width());
    ui->Alabel_create->setFixedWidth(int(w/2.0f));
    float phi=(1.0f+float(sqrt(5.0f)))/2.0f;
    ui->Bframe->setFixedWidth(int(w/2.00f/phi));
    ui->CFrame->setFixedWidth(int(w/2.00f/phi/phi));

    if (e) QWidget::resizeEvent(e);
}
