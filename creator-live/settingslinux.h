/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef SETTINGSLINUX_H
#define SETTINGSLINUX_H

#include <live/object>
#include <live_widgets/bindableparent.h>
#include <QDialog>
#include <QHideEvent>

namespace Ui {
    class SettingsLinux;
}

class LiveAudioSettingsWidget : public QDialog, public live_widgets::BindableParent
{
    Q_OBJECT

public:
    explicit LiveAudioSettingsWidget(QWidget *parent = 0,bool automatic=0);
    ~LiveAudioSettingsWidget();
    void hideEvent(QHideEvent *);

private:
    QList<QVariant> m_inputMappings;
    QStringList m_inputMappingNames;
    QList<QVariant> m_outputMappings;
    QStringList m_outputMappingNames;
    Ui::SettingsLinux *ui;
    bool m_automatic;
    static bool started;
    static QString m_in,m_out;
    static QStringList m_inl, m_outl;

public slots:
    void verify();
    void applyAndQuit();

    static bool apply();

private:
    LiveAudioSettingsWidget(const LiveAudioSettingsWidget&);
    LiveAudioSettingsWidget& operator=(const LiveAudioSettingsWidget&);
};

#endif // SETTINGSLINUX_H
