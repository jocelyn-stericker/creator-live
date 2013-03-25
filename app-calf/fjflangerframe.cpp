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

#include <QPropertyAnimation>
#include <QTimer>

using namespace live_widgets;

FJFlangerFrame::FJFlangerFrame(FJFlangerApp *backend, AbstractTrack *parent)
    : AppFrame(parent)
    , m_app(*backend)
    , ui(new Ui::FJFlangerFrame)
{
    ui->setupUi(this);

    onDelay(m_app.getDelay()*1000.0f);
    onDepth(m_app.getDepth()*1000.0f);
    onRate(m_app.getRate()*1000.0f);
    onFB(m_app.getFB()*1000.0f);
    onStereo(m_app.getStereo());

    if (m_app.getAmount() + m_app.getDryAmount() < 0.1f)
    {
        onWetDryBalance(m_app.getAmount() * 50.0f / 6.0f + 50.0f);
    }
    else
    {
        onWetDryBalance(50.0f);
    }

    setDesiredWidth(255);

    connect(ui->dial_depth,SIGNAL(valueChanged(int)),this,SLOT(onDepth(int)));
    connect(ui->dial_fb,SIGNAL(valueChanged(int)),this,SLOT(onFB(int)));
    connect(ui->dial_minDelay,SIGNAL(valueChanged(int)),this,SLOT(onDelay(int)));
    connect(ui->dial_phase,SIGNAL(valueChanged(int)),this,SLOT(onStereo(int)));
    connect(ui->dial_rate,SIGNAL(valueChanged(int)),this,SLOT(onRate(int)));
    connect(ui->verticalSlider_amount,SIGNAL(valueChanged(int)),this,SLOT(onWetDryBalance(int)));
    connect(ui->pushButton_reset,SIGNAL(pressed()),this,SLOT(onReset()));

    connect(ui->toolButton_more, SIGNAL(toggled(bool)), this, SLOT(setMore(bool)));

    binding::addWidget(this);
}

FJFlangerFrame::~FJFlangerFrame()
{
    m_app.deleteLater();
    delete ui;
}

void FJFlangerFrame::onDelay(int z) { // 0.1...10, log, 1000
    ui->dial_minDelay->setValue(z);
    m_app.setDelay(float(z)/1000.0f);
}

void FJFlangerFrame::onDepth(int z) { // 0.1...10, log, 1000
    ui->dial_depth->setValue(z);
    m_app.setDepth(float(z)/1000.0f);
}

void FJFlangerFrame::onRate(int z) { // 0.01...20, log, 1000
    ui->dial_rate->setValue(z);
    m_app.setRate(float(z)/1000.0f);
}

void FJFlangerFrame::onFB(int z) { // -0.99...0.99, log, 1000
    ui->dial_fb->setValue(z);
    m_app.setRate(float(z)/1000.0f);
}

void FJFlangerFrame::onStereo(int z) { // 0, 360, -, 1
    ui->dial_phase->setValue(z);
    m_app.setStereo(z);
}

void FJFlangerFrame::onReset() { // bool
    m_app.setReset(1.0f);   // I think...
}

void FJFlangerFrame::onWetDryBalance(int z) { // 0...100,
    ui->verticalSlider_amount->setValue(z);
    m_app.setWetDryBalance(100 - z);
}

void FJFlangerFrame::setMore(bool more)
{
    QPropertyAnimation* paFixed = new QPropertyAnimation(this, "desiredWidth");
    paFixed->setStartValue(width());
    if (more) {
        paFixed->setEndValue(255);
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

void FJFlangerFrame::addRounding()
{
    ui->frame->hide();
    QString style = ui->pushButton_menu->styleSheet();
    style.replace("border-top-right-radius: 0px;", "border-top-right-radius: 4px;");
    ui->pushButton_menu->setStyleSheet(style);

    style = ui->toolButton_more->styleSheet();
    style.replace("border-bottom-right-radius: 0px;", "border-bottom-right-radius: 4px;");
    ui->toolButton_more->setStyleSheet(style);
}

void FJFlangerFrame::removeRounding()
{
    ui->frame->show();
    QString style = ui->pushButton_menu->styleSheet();
    style.replace("border-top-right-radius: 4px;", "border-top-right-radius: 0px;");
    ui->pushButton_menu->setStyleSheet(style);

    style = ui->toolButton_more->styleSheet();
    style.replace("border-bottom-right-radius: 4px;", "border-bottom-right-radius: 0px;");
    ui->toolButton_more->setStyleSheet(style);
}

