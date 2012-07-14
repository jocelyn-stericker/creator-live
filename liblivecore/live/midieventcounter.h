/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "live/object.h"
#include "live/midi.h"
#include "live/midifilter.h"
#include "live/midievent.h"
#include <QtConcurrentRun>

#ifndef MIDIEVENTCOUNTER_H
#define MIDIEVENTCOUNTER_H

namespace live {

// This class keeps track of whether a note is "on" or "off". Commands can be sent
// to turn all off all "on" events.
class LIBLIVECORESHARED_EXPORT MidiEventCounter : public Object
{
    int on[200];       /*003*/
public:
    LIVE_MIDI
    LIVE_INPUT
    qint16 shift;
    bool remit;
    bool mOn() const{ return 1; }
    bool aOn() const { return 0; }
    MidiEventCounter();
    void mIn(const Event *data, ObjectChain&);
    bool isOn(int note);
    int velocity(int note);

    void panic();
public:
    virtual ~MidiEventCounter();

    static MidiEventCounter* load(const QByteArray&str);
    QByteArray save();
};

}

#endif // MIDIEVENTCOUNTER_H
