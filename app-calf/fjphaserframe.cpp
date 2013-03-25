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

#include "fjphaserframe.h"
#include "ui_fjphaserframe.h"
#include <QPropertyAnimation>
#include <QTimer>

using namespace live_widgets;

FJPhaserFrame::FJPhaserFrame(FJPhaserApp* app, AbstractTrack *parent)
    : AppFrame(parent)
    , m_app(*app)
    , ui(new Ui::FJPhaserFrame)
{
    ui->setupUi(this);

    onFreq(m_app.getFreq());
    onDepth(m_app.getDepth());
    onRate(m_app.getRate());
    onFB(m_app.getFB());
    onStages(m_app.getStages());

    setDesiredWidth(359);

    if (m_app.getAmount() + m_app.getDryAmount() < 0.1f)
    {
        onWetDryBalance(m_app.getAmount() * 50.0f / 6.0f + 50.0f);
    }
    else
    {
        onWetDryBalance(50.0f);
    }

    connect(ui->toolButton_more, SIGNAL(toggled(bool)), this, SLOT(setMore(bool)));

    binding::addWidget(this);
}

FJPhaserFrame::~FJPhaserFrame()
{
    m_app.deleteLater();
    delete ui;
}

void FJPhaserFrame::onFreq(int z)
{
    m_app.setFreq(z);
    ui->dial_center->setValue(z);
}

void FJPhaserFrame::onDepth(int z)
{
    m_app.setDepth(z);
    ui->dial_depth->setValue(z);
}

void FJPhaserFrame::onRate(int z)
{
    m_app.setRate(float(z)/1000.0f);
    ui->dial_rate->setValue(z);
}

void FJPhaserFrame::onFB(int z)
{
    m_app.setFB(float(z)/100.0f);
    ui->dial_feedback->setValue(z);
}

void FJPhaserFrame::onStages(int z)
{
    m_app.setStages(z);
    ui->dial_stages->setValue(z);
}

void FJPhaserFrame::onReset()
{
    m_app.setReset(1.0f);
}

void FJPhaserFrame::onWetDryBalance(int z)
{
    m_app.setWetDryBalance(z);
    ui->verticalSlider_amount->setValue(z);
}

void FJPhaserFrame::setMore(bool more)
{
    QPropertyAnimation* paFixed = new QPropertyAnimation(this, "desiredWidth");
    paFixed->setStartValue(width());
    if (more) {
        paFixed->setEndValue(359);
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

void FJPhaserFrame::addRounding()
{
    ui->frame->hide();
    QString style = ui->pushButton_menu->styleSheet();
    style.replace("border-top-right-radius: 0px;", "border-top-right-radius: 4px;");
    ui->pushButton_menu->setStyleSheet(style);

    style = ui->toolButton_more->styleSheet();
    style.replace("border-bottom-right-radius: 0px;", "border-bottom-right-radius: 4px;");
    ui->toolButton_more->setStyleSheet(style);
}

void FJPhaserFrame::removeRounding()
{
    ui->frame->show();
    QString style = ui->pushButton_menu->styleSheet();
    style.replace("border-top-right-radius: 4px;", "border-top-right-radius: 0px;");
    ui->pushButton_menu->setStyleSheet(style);

    style = ui->toolButton_more->styleSheet();
    style.replace("border-bottom-right-radius: 4px;", "border-bottom-right-radius: 0px;");
    ui->toolButton_more->setStyleSheet(style);
}
