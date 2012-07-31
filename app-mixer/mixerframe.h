/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef MIXERFRAME_H
#define MIXERFRAME_H

#include "mixerapp.h"

#include <live/app.h>
#include <live/appinterface.h>
#include <live_widgets/appframe.h>

namespace Ui {
    class MixerFrame;
}

class MixerFrame : public live_widgets::AppFrame
{
    Q_OBJECT
    MixerApp& app;
public:
    explicit MixerFrame(MixerApp*backend,QWidget *parent = 0);
    ~MixerFrame();

private:
    Ui::MixerFrame *ui;
};

class MixerCreator : public QObject, public live::AppInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AppInterface)
public:
    MixerCreator()
    {
    }
    QString name() { return "Mixer"; }
    QString description() { return "These things used to cost a lot."; }
    live::ObjectPtr newBackend() { return new MixerApp(); }
    live::ObjectPtr loadBackend(const QByteArray &str) {
        return MixerApp::load(str);
    }
    live_widgets::AppFrame* newFrontend(live::ObjectPtr backend) { return new MixerFrame(static_cast<MixerApp*>(backend.data())); }
    QIcon icon() { return QIcon(":/icons/app_mixer.png"); }
    live::AppInterface* next() { return 0; }
};

#endif // MIXERFRAME_H
