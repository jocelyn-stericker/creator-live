/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef PITCHFRAME_H
#define PITCHFRAME_H

#include <QVBoxLayout>
#include "pitchapp.h"
#include <live/app>
#include <live/appinterface>
#include <live_widgets/appframe.h>
#include <live_widgets/spinbox.h>

namespace Ui {
    class PitchFrame;
}

class PitchFrame : public live_widgets::AppFrame
{
    Q_OBJECT
    PitchApp* app;
public:
    explicit PitchFrame(PitchApp* backend, AbstractTrack *parent = 0);

    bool expanding() const { return false; }

signals:

public slots:
    void syncState();
    void setMore(bool more);
    void addRounding();
    void removeRounding();

private:
    Ui::PitchFrame* ui;
};

class PitchCreator : public QObject, public live::AppInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AppInterface)
    Q_PLUGIN_METADATA(IID "ca.nettek.live.pitch")
public:
    PitchCreator()
    {
    }
    QString name() { return "ca.nettek.live.pitch"; }
    QString description() { return "Ooh! I sound like Micky Mouse..."; }
    live::ObjectPtr newBackend() { return new PitchApp(); }
    live::ObjectPtr loadBackend(const QByteArray &str) {
        return PitchApp::load(str);
    }
    live_widgets::AppFrame* newFrontend(live::ObjectPtr backend) { return new PitchFrame(static_cast<PitchApp*>(backend.data())); }
    QIcon icon() { return QIcon(":/icons/app_pitch.png"); }
    live::AppInterface* next() { return 0; }
};

#endif // PITCHFRAME_H
