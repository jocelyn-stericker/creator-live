/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "live/midi"
#include "midisystem_p.h"
#include "live/midifilter"
#include "live/midibinding"
#include "live/audio"
#include "live/midievent"
#include <QtConcurrentRun>
#include <live/time>

using namespace live_private;

int live::MidiFilter::ss_lastId=-1;
QList<live::MidiFilter*> live::MidiFilter::_u;
int live::MidiFilter::ss_loadState=-1;

/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
SecretMidi
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////

SecretMidi* SecretMidi::me=0;

void SecretMidi::init() {
    this->start();
}

void SecretMidi::refresh() {
}

void SecretMidi::run() { // [MIDI THREAD]
    forever {
        msleep(12);
    live_mutex(x_midi) {
        live::Object::beginProc();

        for(int i=0;i<withheld.size();i++) {
            if(withheld[i].ev->time.toTime_ms()-live::midi::getTime_msec()<5) {
                Withheld w = withheld.takeAt(i--);

                live::ObjectChain p=w.p;
                w.obj->mOut(w.ev, &p, w.reverse);
                delete w.ev;
            }
        }
        live::Object::endProc();
}
    }
}

void SecretMidi::queue(const live::Event* ev, int device, live::ObjectChain from) { // NOT MIDI THREAD

}

void SecretMidi::cancel(live::ObjectPtr from) {
    live_mutex(x_midi) {
    for(int i=0; i<fromqueue.size(); i++) {
        bool ok=0;
        for(int j=0;j<fromqueue[i].size();j++) {
            if(fromqueue[i][j].data()==from.data()) {ok=1;break;}
        }
        if(ok) {
            idqueue.removeAt(i);
            fromqueue.removeAt(i);
            i--;
        }
    }

    for(int i=0; i<withheld.size(); i++) {
        bool ok=0;
        for(int j=0;j<withheld[i].p.size();j++) {
            if(withheld[i].p[j].data()==from.data()) {ok=1;break;}
        }
        if(ok) {
            Withheld w = withheld.takeAt(i--);
            delete w.ev;
        }
    }
}
}

void SecretMidi::mWithhold(live::Event* x,live::ObjectChain p,live::ObjectPtr obj, bool reverse) {
    return;
    live_mutex(x_midi) {
    x->buddy=0;
    withheld.push_back(Withheld(x, p, obj, reverse));
    }
}

void SecretMidi::mRemoveWithheld(live::ObjectPtr obj) {
    return;
    live_mutex(x_midi) {
    for(int i=0;i<withheld.size();i++) {
        if(withheld[i].obj==obj) {
            Withheld w = withheld.takeAt(i--);
            delete w.ev;
        }
    }
}
}

void SecretMidi::mClearEventsTo(live::Object* obj) {
    return;
    live_mutex(x_midi) {
    for(int i=0;i<withheld.size();i++) {
        if(withheld[i].obj.data()==obj) {
            Withheld w = withheld.takeAt(i--);
            delete w.ev;
        }
    }
}
}

/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
Physical Ports
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////

MidiIn::MidiIn(QString ccname,int devId)
  : live::Object(ccname,true,false,2)
  , valid(1)
  , deviceId(devId)
  ,m_null(live::audio::null(2))
  ,m_connection(s_null, this, live::HybridConnection) 
 {
    setTemporary(0);

    live::ObjectPtr*x=new live::ObjectPtr[200];
    for(int i=0;i<200;i++) {
        x[i]=0;
    }
    live::MidiBinding::customKey->insert(this,x);
}

MidiNull::MidiNull() :
    live::Object("Null Midi Device",false,false,2) {

}

void MidiNull::mIn(const live::Event *, live::ObjectChain*) {
}

void MidiOut::mIn(const live::Event *ev, live::ObjectChain*p) {
    setTemporary(0);

    if(!ev) return;
    if(ev->flareId!=-1) {
        if(ev->flareId==0) {
            if(p->size()) SecretMidi::me->cancel(p->first());
        }
    } else {
        SecretMidi::me->queue(ev,device,*p);
    }
}

void live::midi::refresh() {
    if(!SecretMidi::me) new SecretMidi;
    SecretMidi::me->refresh();
}

live::ObjectPtr live::midi::null() {
    if(!SecretMidi::me) new SecretMidi;

    return new MidiNull;
}

int live::midi::getTime_msec() {
    timespec ts;
    clock_gettime(CLOCK_REALTIME,&ts);
    return (ts.tv_sec-1341653917)*1000+ts.tv_nsec/1000000;    // <- look at all those zeros we don't care about :)
}

live::Time live::midi::getTime() {
    return live::Time(getTime_msec());
}

