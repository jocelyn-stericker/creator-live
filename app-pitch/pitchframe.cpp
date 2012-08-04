/*****************************************************
                                           rev. 110720

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield  (trintith@gmail.com)

                 All rights reserved.
*****************************************************/

#include "pitchframe.h"
#include "ui_pitchframe.h"

#include <QtPlugin>
#include <QPropertyAnimation>
#include <QTimer>

PitchFrame::PitchFrame(PitchApp* backend, AbstractTrack *parent)
  : AppFrame(parent), app(backend)
  , ui(new Ui::PitchFrame)
{
    ui->setupUi(this);

    connect(ui->verticalSlider_shift, SIGNAL(valueChanged(int)), app, SLOT(setShift(int)));
    connect(ui->toolButton_more, SIGNAL(toggled(bool)), this, SLOT(setMore(bool)));
    connect(ui->toolButton_aa, SIGNAL(toggled(bool)), ui->verticalSlider_aa, SLOT(setEnabled(bool)));

    syncState();

    setDesiredWidth(307);

    setObjectName("PitchFrame_"+QString::number(app->s_id));
}

void PitchFrame::syncState()
{
    ui->verticalSlider_shift->setValue(app->shiftAmount());
}


void PitchFrame::setMore(bool more)
{
    QPropertyAnimation* paFixed = new QPropertyAnimation(this, "desiredWidth");
    paFixed->setStartValue(width());
    if (more) {
        paFixed->setEndValue(307);
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

void PitchFrame::addRounding()
{
    ui->frame->hide();
    QString style = ui->pushButton_menu->styleSheet();
    style.replace("border-top-right-radius: 0px;", "border-top-right-radius: 4px;");
    ui->pushButton_menu->setStyleSheet(style);

    style = ui->toolButton_more->styleSheet();
    style.replace("border-bottom-right-radius: 0px;", "border-bottom-right-radius: 4px;");
    ui->toolButton_more->setStyleSheet(style);
}

void PitchFrame::removeRounding()
{
    ui->frame->show();
    QString style = ui->pushButton_menu->styleSheet();
    style.replace("border-top-right-radius: 4px;", "border-top-right-radius: 0px;");
    ui->pushButton_menu->setStyleSheet(style);

    style = ui->toolButton_more->styleSheet();
    style.replace("border-bottom-right-radius: 4px;", "border-bottom-right-radius: 0px;");
    ui->toolButton_more->setStyleSheet(style);
}


Q_EXPORT_PLUGIN2(live::AppInterface, PitchCreator)
