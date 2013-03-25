/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/midifilter>

live::MidiFilter::MidiFilter() : m_id(++sm_lastId), b_filterForNote() {
    _u.push_back(this);
    for (int i=0;i<200;i++) {
        b_filterForNote[i]=0;
    }
}

live::MidiFilter::~MidiFilter() {
}

