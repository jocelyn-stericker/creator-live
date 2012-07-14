/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LOOPERFRAME_H
#define LOOPERFRAME_H

#include "looperapp.h"
#include "live/appinterface.h"
#include "live_widgets/appframe.h"
#include "sequencergraph.h"

namespace Ui
{
class SequencerFrame;
}

class LooperFrame : public live_widgets::AppFrame
{
    Q_OBJECT
public:
    LooperApp& app;
    SequencerGraph& graph;

public:
    explicit LooperFrame(LooperApp* backend, QWidget *parent = 0);
    ~LooperFrame();

public slots:
    void logicMute(bool x);
    void logicSub(bool x);
    void logicReplace(bool x);
    void logicOverdub(bool x);
    void logicRecord(bool x);
    void logicPlay(bool x);
    void logicOff(bool x);
    void syncState();

private:
    Ui::SequencerFrame *ui;
    live_widgets::ToolButton*loopSub,*loopReplace,*loopOverdub,*loopRecord,*loopPlay,*loopOff;
};

class LooperCreator : public QObject, public live::AppInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AppInterface)
public:
    LooperCreator()
    {
    }
    QString name() { return "LOOPER"; }
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
