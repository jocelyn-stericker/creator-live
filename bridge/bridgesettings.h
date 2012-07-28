/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef BRIDGESETTINGS_H
#define BRIDGESETTINGS_H

#include <QWidget>

#include <live/object.h>
#include <live/midi.h>
#include "live/../audiosystem_p.h"

namespace Ui {
class BridgeSettings;
}

class QTcpSocket;

class BridgeSettings : public QWidget, public live::Object
{
    Q_OBJECT
    LIVE_MIDI
    LIVE_INPUT
    QList<live::ObjectPtr> s_in;
    QList<live::ObjectPtr> s_out;
    QList<QTcpSocket*> s_sockets;

public:
    explicit BridgeSettings(QWidget *parent = 0);
    ~BridgeSettings();

    void mIn(const live::Event *data, live::ObjectChain &p);
    
public slots:
    void startTalking();
    void listen();

private:
    Ui::BridgeSettings *ui;
};

#endif // BRIDGESETTINGS_H
