/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef FILTERFRAME_H
#define FILTERFRAME_H

#include "bpmapp.h"
#include <live/app>
#include <live/appinterface>
#include <live/variantbinding>
#include <live_widgets/appframe.h>
#include <live_widgets/rotatedlabel.h>
#include <live_widgets/toolbutton.h>

class BPMFrame : public live_widgets::AppFrame
{
    Q_OBJECT
public:
    BPMApp& app;
    live_widgets::RotatedLabel* ui_label;
    live_widgets::ToolButton* ui_tb;

public:
    explicit BPMFrame(BPMApp* backend,AbstractTrack *parent = 0);
    ~BPMFrame();

    bool expanding() const { return false; }

protected:
    void timerEvent(QTimerEvent *);
    void resizeEvent(QResizeEvent *);
};

class BPMCreator : public QObject, public live::AppInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AppInterface)
    Q_PLUGIN_METADATA(IID "ca.nettek.live.bpm")
public:
    BPMCreator()
    {
    }
    QString name() { return "ca.nettek.live.bpm"; }
    QString description() { return "Works with some four-on-the-floor music."; }
    live::ObjectPtr newBackend() { return new BPMApp(); }
    live::ObjectPtr loadBackend(const QByteArray &str) {
        return BPMApp::load(str);
    }
    live_widgets::AppFrame* newFrontend(live::ObjectPtr backend) { return new BPMFrame(static_cast<BPMApp*>(backend.data())); }
    QIcon icon() { return QIcon(":/icons/app_bpm.png"); }
    live::AppInterface* next() { return 0; }
};

#endif
