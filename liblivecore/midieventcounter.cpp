/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/midieventcounter>

using namespace live;

MidiEventCounter::MidiEventCounter() : Object("Midi Event Counter",false,false), shift(0) {
    setTemporary(0);
    for (int i=0;i<200;i++)
        on[i]=0;
}

void MidiEventCounter::mIn(const Event *data, ObjectChain*) {
    if (data->simpleStatus()==Event::NOTE_ON&&data->velocity()) {
        on[data->note()]=data->velocity();
    } else if (data->simpleStatus()==Event::NOTE_OFF||(data->simpleStatus()==Event::NOTE_ON&&!data->velocity())) {
        if (!on[data->note()]&&on[data->note()-shift]) {
            on[data->note()-shift]=0;
            Event td=*data;
            td.setNote((qint16)(data->note()-shift));
            return;
        }
        on[data->note()]=0;
    }
}

bool MidiEventCounter::isOn(int note) {
    return on[note];
}
int MidiEventCounter::velocity(int note) {
    return on[note];
}

QList<Event> MidiEventCounter::flush() {
    QList<Event> ret;
    for (qint16 i = 0; i < 127; ++i) {
        if (on[i]) {
            Event evoff(0x90, i, 0);
            evoff.time = live::midi::getTime();
            ret.push_back(evoff);
            on[i] = 0;
        }
    }
    return ret;

}

void MidiEventCounter::panic() {
    QList<Event> on = flush();

    ObjectChain p;
    p.push_back(this);
    for (int i = 0; i < on.size(); ++i) {
        mOut(&on[i], &p);
    }
}

MidiEventCounter::~MidiEventCounter() {}
