/* This file is part of the Calf Creator Live plugin.
 * The Calf Creator Live plugin is a fork of the Calf DSP library.
 *
 * Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "fjflangerframe.h"
#include "ui_fjflangerframe.h"

using namespace live_widgets;

FJFlangerFrame::FJFlangerFrame(FJFlangerApp *backend, AbstractTrack *parent)
    : AppFrame(parent)
    , s_app(*backend)
    , ui(new Ui::FJFlangerFrame)
{
    ui->setupUi(this);

    onDelay(s_app.getDelay()*1000.0f);
    onDepth(s_app.getDepth()*1000.0f);
    onRate(s_app.getRate()*1000.0f);
    onFB(s_app.getFB()*1000.0f);
    onStereo(s_app.getStereo());

    if (s_app.getAmount() + s_app.getDryAmount() < 0.1f)
    {
        onWetDryBalance(s_app.getAmount() * 50.0f / 6.0f + 50.0f);
    }
    else
    {
        onWetDryBalance(50.0f);
    }

    connect(ui->dial_depth,SIGNAL(valueChanged(int)),this,SLOT(onDepth(int)));
    connect(ui->dial_fb,SIGNAL(valueChanged(int)),this,SLOT(onFB(int)));
    connect(ui->dial_minDelay,SIGNAL(valueChanged(int)),this,SLOT(onDelay(int)));
    connect(ui->dial_phase,SIGNAL(valueChanged(int)),this,SLOT(onStereo(int)));
    connect(ui->dial_rate,SIGNAL(valueChanged(int)),this,SLOT(onRate(int)));
    connect(ui->verticalSlider_amount,SIGNAL(valueChanged(int)),this,SLOT(onWetDryBalance(int)));
    connect(ui->pushButton_reset,SIGNAL(pressed()),this,SLOT(onReset()));
}

FJFlangerFrame::~FJFlangerFrame()
{
    s_app.deleteLater();
    delete ui;
}

void FJFlangerFrame::onDelay(int z) { // 0.1...10, log, 1000
    ui->dial_minDelay->setValue(z);
    s_app.setDelay(float(z)/1000.0f);
}

void FJFlangerFrame::onDepth(int z) { // 0.1...10, log, 1000
    ui->dial_depth->setValue(z);
    s_app.setDepth(float(z)/1000.0f);
}

void FJFlangerFrame::onRate(int z) { // 0.01...20, log, 1000
    ui->dial_rate->setValue(z);
    s_app.setRate(float(z)/1000.0f);
}

void FJFlangerFrame::onFB(int z) { // -0.99...0.99, log, 1000
    ui->dial_fb->setValue(z);
    s_app.setRate(float(z)/1000.0f);
}

void FJFlangerFrame::onStereo(int z) { // 0, 360, -, 1
    ui->dial_phase->setValue(z);
    s_app.setStereo(z);
}

void FJFlangerFrame::onReset() { // bool
    s_app.setReset(1.0f);   // I think...
}

void FJFlangerFrame::onWetDryBalance(int z) { // 0...100,
    ui->verticalSlider_amount->setValue(z);
    s_app.setWetDryBalance(100 - z);
}
