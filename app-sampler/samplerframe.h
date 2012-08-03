/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/
#ifndef SAMPLERFRAME_H
#define SAMPLERFRAME_H

#include "samplerapp.h"

#include <live/app.h>
#include <live/appinterface.h>
#include <live_widgets/appframe.h>
#include <QMenu>

namespace Ui {
    class Sampler;
}

class SamplerFrame : public live_widgets::AppFrame
{
    Q_OBJECT
    SamplerApp& s_app;
    int s_times[16];
    QMenu* currentCM;
    int s_activeButton;
public:
    explicit SamplerFrame(SamplerApp* backend, AbstractTrack *parent = 0);
    ~SamplerFrame();

    bool expanding() const { return false; }

public slots:
    void buttonLogic();
    void contextLogic(QPoint);

    void importLogic();
    void exportLogic();
    void bindingLogic();

private:
    Ui::Sampler *ui;
};

class SamplerCreator : public QObject, public live::AppInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AppInterface)
public:
    SamplerCreator()
    {
    }
    QString name() { return "SAMPLER"; }
    QString description() { return "Make your own instrument..."; }
    live::ObjectPtr newBackend() { return new SamplerApp(); }
    live::ObjectPtr loadBackend(const QByteArray &str) {
        return SamplerApp::load(str);
    }
    live_widgets::AppFrame* newFrontend(live::ObjectPtr backend) { return new SamplerFrame(static_cast<SamplerApp*>(backend.data())); }
    QIcon icon() { return QIcon(":/icons/app_sampler.png"); }
    live::AppInterface* next() { return 0; }
};

#endif // SAMPLERFRAME_H
