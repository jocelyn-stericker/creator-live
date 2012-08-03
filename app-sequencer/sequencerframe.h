/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef SEQUENCERFRAME_H
#define SEQUENCERFRAME_H

#include "sequencergraph.h"
#include "live/appinterface.h"
#include "live_widgets/appframe.h"

#include <QFileDialog>

namespace Ui
{
class SequencerFrame;
}

class QMenu;

class SequencerFrame : public live_widgets::AppFrame
{
    Q_OBJECT
public:
    SequencerApp& app;
    SequencerGraph& graph;

public:
    explicit SequencerFrame(SequencerApp*backend, AbstractTrack *parent = 0);
    ~SequencerFrame();

    QMenu* action_menu;
    QAction* action_import;
    QAction* action_export;
    QAction* action_newToolchain;

    bool expanding() const { return width() > 60; }

public slots:
    void logicMute(bool x);
    void logicPlay(bool x);
    void logicRecord(bool x);
    void logicImport();
    void logicExport();
    void syncState();
    virtual void toggleMinimized();

    void setMore(bool);

    void addRounding();
    void removeRounding();

protected:
    void keyPressEvent(QKeyEvent *e) { graph.keyPressEvent(e); AppFrame::keyPressEvent(e); }

private:
    Ui::SequencerFrame *ui;
};

class LooperCreator;

class SequencerCreator : public QObject, public live::AppInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AppInterface)
    LooperCreator* s_looper;
public:
    SequencerCreator() : s_looper(0)
    {
    }
    QString name() { return "SEQUENCER"; }
    QString description() { return "A really fancy tape recorder"; }
    live::ObjectPtr newBackend() {return new SequencerApp(); }
    QObject* newFrontend(live::ObjectPtr backend) { return new SequencerFrame(live::cast_to_cptr<SequencerApp*>(backend)); }
    live::AppInterface* next();
    live::ObjectPtr loadBackend(const QByteArray &str) {
        return SequencerApp::load(str);
    }

    QIcon icon() { return QIcon(":/icons/app_seq.png"); }
};

#endif // SEQUENCERFRAME_H
