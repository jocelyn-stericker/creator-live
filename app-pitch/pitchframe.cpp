/*****************************************************
                                           rev. 110720

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield  (trintith@gmail.com)

                 All rights reserved.
*****************************************************/

#include "pitchframe.h"
#include <QtPlugin>

PitchFrame::PitchFrame(PitchApp* backend, QWidget *parent) :
    AppFrame(parent), app(backend)
{
    setLayout(&ui_vbox);
    setSizePolicy(QSizePolicy::Maximum,QSizePolicy::MinimumExpanding);
    ui_spinBox.setMaximumWidth(40);
    ui_spinBox.setMinimum(-80);
    ui_spinBox.setMaximum(80);
    setMaximumWidth(100);
    ui_spinBox.setObjectName("ui_spinBox");
    connect(&ui_spinBox,SIGNAL(valueChanged(int)),backend,SLOT(setShift(int)));
    ui_vbox.addWidget(&ui_spinBox);
    syncState();

    setObjectName("PitchFrame_"+QString::number(app->s_id));
}

void PitchFrame::syncState()
{
    ui_spinBox.setValue(app->shiftAmount());
}

Q_EXPORT_PLUGIN2(live::AppInterface, PitchCreator)
