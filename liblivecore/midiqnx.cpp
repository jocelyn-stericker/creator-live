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
#include <time>

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
        live::Object::beginProc();

        for(int i=0;i<withheld_ev.size();i++) {
            if(withheld_ev[i]->time.toTime_ms()-live::midi::getTime_msec()<5) {
                ///////////////////////////
                live::Object::endProc(); ////////
                lock.unlock(); ////////////
                ///////////////////////////

                live::ObjectChain p=withheld_p.takeAt(i);
                if(!withheld_reverse.takeAt(i)) {
                    withheld_obj.takeAt(i)->mOutverse(withheld_ev.at(i),p);
                } else {
                    withheld_obj.takeAt(i)->mOut(withheld_ev.at(i),p);
                }
                delete withheld_ev.takeAt(i);
                --i;

                ///////////////////////////
                live::Object::beginProc(); //////
                lock.relock(); ////////////
                ///////////////////////////

            }
        }

        live::Object::endProc();
    }
}

void SecretMidi::queue(const live::Event* ev, int device, live::ObjectChain from) { // NOT MIDI THREAD

}

void SecretMidi::cancel(live::ObjectPtr from) {
    live::lthread::midi();
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

    for(int i=0; i<withheld_ev.size(); i++) {
        bool ok=0;
        for(int j=0;j<withheld_p[i].size();j++) {
            if(withheld_p[i][j].data()==from.data()) {ok=1;break;}
        }
        if(ok) {
            withheld_ev.removeAt(i);
            withheld_p.removeAt(i);
            withheld_obj.removeAt(i);
            withheld_reverse.removeAt(i);
            i--;
        }
    }
}

void SecretMidi::mWithhold(live::Event* x,live::ObjectChain p,live::ObjectPtr obj, bool reverse) {
    live::lthread::midi();
    x->buddy=0;
    withheld_ev.push_back(x);
    withheld_p.push_back(p);
    withheld_obj.push_back(obj);
    withheld_reverse.push_back(reverse);
}

void SecretMidi::mRemoveWithheld(live::ObjectPtr obj) {
    live::lthread::midi();
    for(int i=0;i<withheld_ev.size();i++) {
        if(withheld_obj[i]==obj) {
            delete withheld_ev.takeAt(i);
            withheld_p.removeAt(i);
            withheld_obj.removeAt(i);
            withheld_reverse.removeAt(i);
            --i;
        }
    }
}

void SecretMidi::mRemoveWithheld_object_dest(live::Object* obj) {
    live::lthread::midi();
    for(int i=0;i<withheld_ev.size();i++) {
        if(withheld_obj[i].data()==obj) {
            delete withheld_ev.takeAt(i);
            withheld_p.removeAt(i);
            withheld_obj.removeAt(i);
            withheld_reverse.removeAt(i);
            --i;
        }
    }
}

/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
Physical Ports
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////

MidiIn::MidiIn(QString ccname,int devId) :
    live::Object(ccname,true,false),
    valid(1),
    deviceId(devId) {
    setTemporary(0);

    live::ObjectPtr*x=new live::ObjectPtr[200];
    for(int i=0;i<200;i++) {
        x[i]=0;
    }
    live::MidiBinding::customKey->insert(this,x);

    s_null=live::audio::null(2);
    live::audio::null(2)->hybridConnect(this);
}

MidiNull::MidiNull() :
    live::Object("Null Midi Device",false,false) {

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
        SecretMidi::me->queue(ev,device,p);
    }
}

void live::midi::refresh() {
    if(!SecretMidi::me) new SecretMidi;
    SecretMidi::me->refresh();
}

live::ObjectPtr live::midi::getNull() {
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

