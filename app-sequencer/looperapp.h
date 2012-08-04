/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LOOPER_H
#define LOOPER_H

#include "sequencerapp.h"

class LooperApp : protected SequencerApp
{
    Q_OBJECT
public:
    LIVE_HYBRID

    QByteArray save();
    static live::ObjectPtr load(const QByteArray&str);

    friend class LooperFrame;
    friend class LooperGraph;
    friend class LooperCreator;
    friend class Hathor;
    friend class AppSys;

    enum LoopMode {
        Off=0,
        Playing=1,
            //[old]
        Recording=2,
            //starts over at beg.
            //[new]
        Overdubbing=3,
            //[new]
            //[old]
        Multiplying=4,
            //[  new stuff here  ]
            //[old][old][old][old]
        Replacing=5,
            //same duration, ERASE old stuff
        Substituting=6
            //same duration, keep old stuff while recording, then erase.
    };
    /*003-005 SequencerApp*/
    live::Bound<int> b_loopMode; /*i.e., LoopMode*/ /*009*/
    live::Bound<int> b_loopLength;    /*009*/
    int s_id_looper;            /*010*/
    static int s_lastId;
    QList<live::Event> s_missedList;

    LooperApp(live::MidiTrack*mt=0,live::AudioTrack*at=0,live::MidiEventCounter*mc=0);

    const bool& isMute() const;
    int pos() const;

    QObject* qoThis() { return this;}

public slots:
    void modeChanged(int now);
    void looperLogic();
    void aIn(const float *data, int chan, live::ObjectChain*p);
    void mIn(const live::Event *data, live::ObjectChain*p);
};

#endif // LOOPER_H
