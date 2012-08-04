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

#include "fjreverbframe.h"
#include "./build/3rd-calf/ui_fjreverbframe.h"
#include <QtPlugin>
#include <QPropertyAnimation>
#include <QTimer>

using namespace live_widgets;

FJReverbFrame::FJReverbFrame(FJReverbApp *backend, AbstractTrack *parent)
    : AppFrame(parent)
    , s_app(*backend)
    , ui(new Ui::FJReverbFrame)
{
    ui->setupUi(this);
    onRoomSize(s_app.getRoomSize());
    onDiffusion(s_app.getDiffusion()*100.0f);
    onDecayTime(s_app.getDecayTime()*1000.0f);
    onHFDamp(s_app.getHFDamp());
    onTrebelCut(s_app.getTrebleCut());
    onBassCut(s_app.getBassCut());
    onPredelay(s_app.getPredelay()*2.0f);

    if (s_app.getWet() + s_app.getDry() < 0.1f)
    {
        onWetDryBalance(s_app.getWet() * 50.0f / 6.0f + 50.0f);
    }
    else
    {
        onWetDryBalance(50.0f);
    }

    connect(ui->dial_bass,SIGNAL(valueChanged(int)),this,SLOT(onBassCut(int)));
    connect(ui->dial_diffusion,SIGNAL(valueChanged(int)),this,SLOT(onDiffusion(int)));
    connect(ui->dial_treble,SIGNAL(valueChanged(int)),this,SLOT(onHFDamp(int)));
    connect(ui->dial_trebleCut,SIGNAL(valueChanged(int)),this,SLOT(onTrebelCut(int)));
    connect(ui->verticalSlider_decayTime,SIGNAL(valueChanged(int)),this,SLOT(onDecayTime(int)));
    connect(ui->verticalSlider_dryWet,SIGNAL(valueChanged(int)),this,SLOT(onWetDryBalance(int)));
    connect(ui->verticalSlider_preDelay,SIGNAL(valueChanged(int)),this,SLOT(onPredelay(int)));
    connect(ui->verticalSlider_roomSize,SIGNAL(valueChanged(int)),this,SLOT(onRoomSize(int)));

    connect(ui->toolButton_more, SIGNAL(toggled(bool)), this, SLOT(setMore(bool)));

    setDesiredWidth(330);
}

FJReverbFrame::~FJReverbFrame()
{
    s_app.deleteLater();
    delete ui;
}

void FJReverbFrame::onRoomSize(int f)
{
    s_app.setRoomSize((FJReverbApp::RoomSize)f);
    ui->verticalSlider_roomSize->setValue(f);
}

void FJReverbFrame::onDiffusion(int f)
{
    s_app.setDiffusion(float(f)/100.0f);
    ui->dial_diffusion->setValue(f);
}

void FJReverbFrame::onDecayTime(int f)
{
    s_app.setDecayTime(float(f)/1000.0f);
    ui->verticalSlider_decayTime->setValue(f);
}

void FJReverbFrame::onHFDamp(int f)
{
    s_app.setHFDamp(f);
    ui->dial_treble->setValue(f);
}

void FJReverbFrame::onTrebelCut(int f)
{
    s_app.setTrebleCut(f);
    ui->dial_trebleCut->setValue(f);
}

void FJReverbFrame::onBassCut(int f)
{
    s_app.setBassCut(f);
    ui->dial_bass->setValue(f);
}

void FJReverbFrame::onPredelay(int f)
{
    s_app.setPredelay(float(f)/2.0f);
    ui->verticalSlider_preDelay->setValue(f);
}

void FJReverbFrame::onWetDryBalance(int f)
{
    s_app.setWetDryBalance(f);
}

void FJReverbFrame::setMore(bool more)
{
    QPropertyAnimation* paFixed = new QPropertyAnimation(this, "desiredWidth");
    paFixed->setStartValue(width());
    if (more) {
        paFixed->setEndValue(330);
        removeRounding();
    } else {
        paFixed->setEndValue(56);
        connect(paFixed, SIGNAL(finished()), this, SLOT(addRounding()));
    }
    paFixed->setDuration(500);
    paFixed->setEasingCurve(QEasingCurve::InQuad);
    paFixed->start(QAbstractAnimation::DeleteWhenStopped);

    QTimer::singleShot(600, parent(), SLOT(updateGeometriesOrDie()));
}

void FJReverbFrame::addRounding()
{
    ui->frame->hide();
    QString style = ui->pushButton_menu->styleSheet();
    style.replace("border-top-right-radius: 0px;", "border-top-right-radius: 4px;");
    ui->pushButton_menu->setStyleSheet(style);

    style = ui->toolButton_more->styleSheet();
    style.replace("border-bottom-right-radius: 0px;", "border-bottom-right-radius: 4px;");
    ui->toolButton_more->setStyleSheet(style);
}

void FJReverbFrame::removeRounding()
{
    ui->frame->show();
    QString style = ui->pushButton_menu->styleSheet();
    style.replace("border-top-right-radius: 4px;", "border-top-right-radius: 0px;");
    ui->pushButton_menu->setStyleSheet(style);

    style = ui->toolButton_more->styleSheet();
    style.replace("border-bottom-right-radius: 4px;", "border-bottom-right-radius: 0px;");
    ui->toolButton_more->setStyleSheet(style);
}

Q_EXPORT_PLUGIN2(live::AppInterface, FJReverbCreator)
