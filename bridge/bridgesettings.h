/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef BRIDGESETTINGS_H
#define BRIDGESETTINGS_H

#include <QWidget>

#include <live/object>
#include <live/midi>
#include "live/../audiosystem_p.h"
#include <live_widgets/bindableparent.h>

namespace Ui {
class BridgeSettings;
}

class QTcpSocket;

class BridgeSettings : public QWidget, public live::Object, public live_widgets::BindableParent
{
    Q_OBJECT
    LIVE_MIDI
    LIVE_INPUT
    QList<live::ObjectPtr> m_in;
    QList<live::ObjectPtr> m_out;
    QList<QTcpSocket*> m_sockets;
    QList<live::Connection> m_connections;

public:
    explicit BridgeSettings(QWidget *parent = 0);
    ~BridgeSettings();

    void mIn(const live::Event *data, live::ObjectChain* p);
    
public slots:
    void startTalking();
    void listen();

private:
    Ui::BridgeSettings *ui;
};

#endif // BRIDGESETTINGS_H
