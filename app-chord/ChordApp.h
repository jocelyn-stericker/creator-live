/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef CHORD_H
#define CHORD_H

#include "src/Framework/Sound.h"

class ChordApp : public LObject, public QHash<Event, QList<Event> >    /*003*/
{
public:
    LIVE_HYBRID
    LIVE_EFFECT
    RELOADABLE(ChordApp)
    ChordApp();
    bool mOn() const{ return 0; } bool aOn() const { return 0; }   //FIXME!!!FIXME!!!FIXME!!! FIX ME!!! BUG!!!

    void aIn(const float *data, int chan, ObjectChain*p);
    void mIn(const Event *data, ObjectChain*p);
};

#endif // CHORD_H
