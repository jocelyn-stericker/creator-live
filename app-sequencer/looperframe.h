/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LOOPERFRAME_H
#define LOOPERFRAME_H

#include "looperapp.h"
#include "sequencergraph.h"

#include <live/appinterface>
#include <live_widgets/appframe.h>

namespace Ui
{
class LooperFrame;
}

class LooperFrame : public live_widgets::AppFrame
{
    Q_OBJECT
public:
    LooperApp& app;
    SequencerGraph& graph;

public:
    explicit LooperFrame(LooperApp* backend, AbstractTrack *parent = 0);
    ~LooperFrame();

    live::Bound<bool> b_growing;

    bool expanding() const { return b_growing ? (width() > 60) : false; }

public slots:
    void logicMute(bool x);
    void logicSub(bool x);
    void logicReplace(bool x);
    void logicOverdub(bool x);
    void logicRecord(bool x);
    void logicPlay(bool x);
    void logicOff(bool x);
    void syncState();

    void setMore(bool);

    void addRounding();
    void removeRounding();

private:
    Ui::LooperFrame *ui;
};

class LooperCreator : public QObject, public live::AppInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AppInterface)
public:
    LooperCreator()
    {
    }
    QString name() { return "ca.nettek.live.looper"; }
    QString description() { return "Record something, and then something on top of that..."; }
    live::ObjectPtr newBackend() { return new LooperApp(); }
    live::ObjectPtr loadBackend(const QByteArray &str) {
        return LooperApp::load(str);
    }
    live_widgets::AppFrame* newFrontend(live::ObjectPtr backend) { return new LooperFrame(static_cast<LooperApp*>(backend.data())); }
    QIcon icon() { return QIcon(":/icons/app_loop.png"); }
    live::AppInterface* next() { return 0; }
};

#endif // SEQUENCERFRAME_H
