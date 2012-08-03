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

#include "fjfilterframe.h"
#include "ui_fjfilterframe.h"

using namespace live_widgets;

FJFilterFrame::FJFilterFrame(FJFilterApp *s_backend, AbstractTrack *parent)
    : AppFrame(parent)
    , s_app(*s_backend)
    , ui(new Ui::FJFilterFrame)
{
    ui->setupUi(this);

    onCutoff(s_app.getCutoff());
    onResonance(s_app.getResonance()*1000.0f);
    onMode(s_app.getMode());
    onIntertia(s_app.getInertia());

    connect(ui->verticalSlider_freq,SIGNAL(valueChanged(int)),this,SLOT(onCutoff(int)));
    connect(ui->verticalSlider_iner,SIGNAL(valueChanged(int)),this,SLOT(onIntertia(int)));
    connect(ui->verticalSlider_res,SIGNAL(valueChanged(int)),this,SLOT(onResonance(int)));
    connect(ui->comboBox_mode1,SIGNAL(currentIndexChanged(int)),this,SLOT(onMode()));
}

FJFilterFrame::~FJFilterFrame()
{
    s_app.deleteLater();
    delete ui;
}

void FJFilterFrame::onCutoff(int f) { // 10...20000, 2000, log
    s_app.setCutoff(f);
    ui->verticalSlider_freq->setValue(f);
}

void FJFilterFrame::onResonance(int f) { // 0.707...32, 0.707, 1000
    s_app.setResonance(float(f)/1000.0f);
    ui->verticalSlider_res->setValue(f);
}

void FJFilterFrame::onMode(int f) { // Filter
    if (f != -1) {
        ui->comboBox_mode1->setCurrentIndex(f/3);
        ui->comboBox_mode2->setCurrentIndex(f%3);
    }

    s_app.setMode(static_cast<FJFilterApp::Filter>
        ( ui->comboBox_mode1->currentIndex() * 3
        + ui->comboBox_mode2->currentIndex() / 3));
}

void FJFilterFrame::onIntertia(int f) { // 5...100, 20
    s_app.setIntertia(f);
    ui->verticalSlider_iner->setValue(f);
}
