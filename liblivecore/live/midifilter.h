/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef MIDIFILTER_H
#define MIDIFILTER_H

#include "live/variantbinding.h"
#include "liblivecore_global.h"

namespace live {

class LIBLIVECORESHARED_EXPORT MidiFilter
{
    int s_id;
public:
    static int ss_lastId;       //Midi.cpp
    static QList<MidiFilter*> _u;
    static int ss_loadState;
public:
    friend class song;
    Bound<int> b_filterForNote[200];    /*003*/
    MidiFilter();
    ~MidiFilter();

    static MidiFilter* load(const QByteArray&str);
    QByteArray save();
};

}

#endif // MIDIFILTER_H
