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
    resizeEvent(0);
}

IntroWizard::~IntroWizard() {
    delete ui;
}

void IntroWizard::resizeEvent(QResizeEvent *e) {
    int w=width()-ui->line_3->width();
    ui->Alabel_create->setFixedWidth(w/2);
    float phi=(1+sqrt(5.0))/2.0;
    ui->Bframe->setFixedWidth(w/2.00/phi);
    ui->CFrame->setFixedWidth(w/2.00/phi/phi);

    if (e) QWidget::resizeEvent(e);
}
