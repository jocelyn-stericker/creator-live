/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef SEQUENCERSYS_H
#define SEQUENCERSYS_H

#include "looperapp.h"

class SequencerSys : public QObject
{
    Q_OBJECT
    QList<SequencerApp*> _u;
    bool ignoreLock;
    QMutex csMutex;
    int _lastId;
    static SequencerSys* self;
public:
    friend QByteArray SequencerApp::save();
    friend live::ObjectPtr SequencerApp::load(const QByteArray &str);

    friend QByteArray LooperApp::save();
    friend live::ObjectPtr LooperApp::load(const QByteArray &str);

    static void registerClippedSeq(SequencerApp* that);
    static void deregisterClippedSeq(SequencerApp *that);
    static int newIdForTrack();

protected:
    SequencerSys() : ignoreLock(0),csMutex(QMutex::Recursive),_lastId(0) {Q_ASSERT(!self);self=this;}
    void registerSeq_real(SequencerApp* that);
    void deregisterSeq_real(SequencerApp* that);
    ~SequencerSys();

public slots:
    void startPlayback();
    void stopPlayback();
    void setPos(int pos);
};

#endif // SEQUENCERSYS_H
