/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef SETTINGSLINUX_H
#define SETTINGSLINUX_H

#include "live/object.h"
#include <QDialog>
#include <QHideEvent>

namespace Ui {
    class SettingsLinux;
}

class LiveAudioSettingsWidget : public QDialog
{
    Q_OBJECT

public:
    explicit LiveAudioSettingsWidget(QWidget *parent = 0,bool automatic=0);
    ~LiveAudioSettingsWidget();
    void hideEvent(QHideEvent *);

private:
    QList<QVariant> s_inputMappings;
    QStringList s_inputMappingNames;
    QList<QVariant> s_outputMappings;
    QStringList s_outputMappingNames;
    Ui::SettingsLinux *ui;
    bool s_automatic;
    static bool started;
    static QString s_in,s_out;
    static QStringList s_inl, s_outl;

public slots:
    void verify();
    void applyAndQuit();

    static bool apply();
};

#endif // SETTINGSLINUX_H
