/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifdef __QNX__
#include "midiqnx.cpp"
#else

#include <live/midi>
#include "midisystem_p.h"
#include <live/midifilter>
#include <live/midibinding>
#include <live/audio>
#include <live/midievent>
#include <QtConcurrentRun>

using namespace live_private;

int live::MidiFilter::ss_lastId=-1;
QList<live::MidiFilter*> live::MidiFilter::_u;
int live::MidiFilter::ss_loadState=-1;

/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
Interface with portmidi
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////

//from the test program in portmidi
//{
#define INPUT_BUFFER_SIZE 100
#define OUTPUT_BUFFER_SIZE 0
#define DRIVER_INFO NULL
#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL
//}

/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
SecretMidi
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////

SecretMidi* SecretMidi::me=0;

void SecretMidi::init() {

    live_mutex(x_midi) {
        if (SecretMidi::me)
            return;

        SecretMidi::me=this;
    }

    Pm_GetDefaultInputDeviceID();
    Pt_Start( 0, 0, 0 );

    int lastI=-1;
    int lastO=-1;
    for ( int i = 0; i < Pm_CountDevices(); i++ ) {
        const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
        if (info-> input) {
            pmins.push_back(NULL);

            Pm_OpenInput
                    (
                        &pmins.back(),
                        i,
                        DRIVER_INFO,
                        INPUT_BUFFER_SIZE,
                        TIME_PROC,
                        TIME_INFO
                        );

            inputs.push_back( new MidiIn(info->name, ++lastI) );
            // inputs correspond to pmins. They are equivalents in different systems
        } else if (info->output) {
            pmouts.push_back(NULL);

            Pm_OpenOutput
                    (
                        &pmouts.back(),
                        i,
                        DRIVER_INFO,
                        OUTPUT_BUFFER_SIZE,
                        TIME_PROC,
                        TIME_INFO,
                        0
                        );

            outputs.push_back( new MidiOut(info->name, ++lastO ) );
            // outputs correspond to pmouts. They are equivalents in different systems
        }
    }

    this->start();
}

void SecretMidi::refresh() {
    live_mutex(x_midi) {
        while (pmouts.size()) {
            Pm_Close(pmouts.takeFirst());
        }
        for (int i=0; i<outputs.size(); i++) {
            outputs[i]->device=-2;
            outputs[i]->valid=0;
        }
        while (pmins.size()) {
            Pm_Close(pmins.takeFirst());
        }
        for (int i=0; i<inputs.size(); i++) {
            inputs[i]->valid=0;
        }
        pmins.clear();
        pmouts.clear();
        Pm_Terminate();
        Pm_Initialize();
        int lastI=-1;
        for ( int i = 0; i < Pm_CountDevices(); i++ ) {
            const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
            bool ok=1;

            if (ok) {
                if (info-> input) {
                    pmins.push_back(NULL);

                    Pm_OpenInput
                            (
                                &pmins.back(),
                                i,
                                DRIVER_INFO,
                                INPUT_BUFFER_SIZE,
                                TIME_PROC,
                                TIME_INFO
                                );

                    int validate =-2;
                    for (int i1=0; i1<inputs.size(); i1++) {
                        if (inputs[i1]->name()==info->name&&!inputs[i1]->valid) {
                            inputs[i1]->valid=1;
                            validate=i1;
                            break;
                        }
                    }
                    if (validate==-2) {
                        inputs.push_back( new MidiIn(info->name,++lastI) );
                    } else {
                        inputs[validate]->deviceId=++lastI;
                    }
                    // inputs correspond to pmins. They are equivalents in different systems
                } else if (info->output) {
                    pmouts.push_back(NULL);

                    Pm_OpenOutput
                            (
                                &pmouts.back(),
                                i,
                                DRIVER_INFO,
                                OUTPUT_BUFFER_SIZE,
                                TIME_PROC,
                                TIME_INFO,
                                0
                                );

                    int validate=-2;
                    QString uniqueNameAppend;
                    for (int i1=0; i1<outputs.size(); i1++) {
                        if (outputs[i1]->name()==info->name+uniqueNameAppend&&!outputs[i1]->valid) {
                            outputs[i1]->valid=1;
                            validate=i1;
                            break;
                        } else if (outputs[i1]->name()==info->name+uniqueNameAppend&&outputs[i1]->valid) {
                            if (uniqueNameAppend.isEmpty()) {
                                uniqueNameAppend="(2)";
                            } else {
                                QString b=uniqueNameAppend;
                                b.chop(1);
                                b.remove(0,1);
                                uniqueNameAppend=("("+b+")");
                            }
                        }
                    }
                    if (validate==-2) {
                        outputs.push_back( new MidiOut(info->name+uniqueNameAppend, pmouts.size()-1 ) );
                    } else {
                        outputs[validate]->device=pmouts.size()-1;
                    }
                    // outputs correspond to pmouts. They are equivalents in different systems
                }
            }
        }

        //store.
        kill_kitten {
            live::object::clear(live::MidiOnly|live::InputOnly|live::NoRefresh);
            live::object::clear(live::MidiOnly|live::OutputOnly|live::NoRefresh);
            for (int i=0;i<inputs.size();i++) {
                live::object::set(inputs[i]);
            }
            for (int i=0;i<outputs.size();i++) {
                live::object::set(outputs[i]);
            }
        }
    }
}

void SecretMidi::run() { // [MIDI THREAD]

    live::lthread::midiInit();

    forever {
        msleep(20);
        live_mutex(x_midi) {
            for ( int i = 0; i < pmins.size(); i++ ) {
                while ( pmins[i] && Pm_Poll( pmins[i] ) ) {
                    PmEvent evnp;
                    Pm_Read( pmins[i], &evnp, 1 );

                    live::Event* e = new live::Event;

                    e->message=Pm_MessageStatus(evnp.message);
                    e->data1=Pm_MessageData1(evnp.message);
                    e->data2=Pm_MessageData2(evnp.message);
                    e->time=live::Time(evnp.timestamp);

                    //make less inefficient
                    int inverse=-1;
                    for (int j=0; j<inputs.size(); j++) {
                        if (inputs[j]->deviceId==i&&inputs[j]->valid) {
                            inverse=j;
                            break;
                        }
                    }

                    if (inverse!=-1) {
                        Q_ASSERT(live::MidiBinding::customKey->value(inputs[inverse],0));
                        if ((live::MidiBinding::customKey->value(inputs[inverse],0)&&live::MidiBinding::customKey->value(inputs[inverse])[e->note()].valid())||(*live::MidiBinding::customNow).valid()) {
                            live::ObjectChain p;
                            p.push_back(inputs[inverse]);
                            ((*live::MidiBinding::customNow).valid()?*live::MidiBinding::customNow:live::MidiBinding::customKey->value(inputs[inverse])[e->note()])->mIn(e, &p);
                        }

                        live::ObjectChain p;  // FIXME?
                        inputs[inverse]->mIn(e, &p);
                    }

                    delete e;
                    // pmins and inputs are equiv. in different systems
                }
            }

            while ( !pmqueue.isEmpty() && (!pmqueue.first()->timestamp || (pmqueue.first()->timestamp - TIME_PROC(TIME_INFO) < 10)) ) {
                Pm_Write( pmouts[ idqueue.takeFirst() ], pmqueue.first(), 1 );

                fromqueue.removeFirst();
                delete pmqueue.takeFirst();
            }


            for (int i=0;i<withheld_ev.size();i++) {
                if (withheld_ev[i]->time.toTime_ms()-TIME_PROC(TIME_INFO)<5) {

                    live::ObjectChain p=withheld_p.takeAt(i);
                    if (!withheld_reverse.takeAt(i)) {
                        withheld_obj.takeAt(i)->mOut(withheld_ev.at(i), &p);
                    } else {
                        withheld_obj.takeAt(i)->mOutverse(withheld_ev.at(i), &p);
                    }
                    delete withheld_ev.takeAt(i);
                    --i;

                }
            }
        }
    }
}

void SecretMidi::queue(const live::Event* ev, int device, live::ObjectChain from) { // NOT MIDI THREAD
    live_mutex(x_midi) {
        PmEvent* pmev = new PmEvent;
        pmev->message = Pm_Message(ev->message,ev->data1,ev->data2);

        if (ev->time.sec==-1) {
            Q_ASSERT(ev->time.nsec==-1);
            pmev->timestamp=0;
        } else {
            pmev->timestamp = (ev->time.toTime_ms());
        }

        // order matters.
        if ( pmqueue.isEmpty()||!pmev->timestamp ) {
            pmqueue.prepend( pmev );
            idqueue.prepend( device );
            fromqueue.prepend( from );
        } else for ( int i = pmqueue.size()-1; i > -1; i-- ) {
            if ( pmqueue[i]->timestamp < pmev->timestamp ) {
                pmqueue.insert( i+1, pmev );
                idqueue.insert( i+1, device );
                fromqueue.insert( i+1, from );
                break;
            } else if ( i == 0 ) {
                pmqueue.prepend( pmev );
                idqueue.prepend( device );
                fromqueue.prepend( from );
            }
        }
    }
}

void SecretMidi::cancel(live::ObjectPtr from) {    
    live_mutex(x_midi) {
        for (int i=0; i<fromqueue.size(); i++) {
            if (!pmqueue[i]->timestamp) continue;
            bool ok=0;
            for (int j=0;j<fromqueue[i].size();j++) {
                if (fromqueue[i][j].data()==from.data()) {ok=1;break;}
            }
            if (ok) {
                pmqueue.removeAt(i);
                idqueue.removeAt(i);
                fromqueue.removeAt(i);
                i--;
            }
        }

        for (int i=0; i<withheld_ev.size(); i++) {
            bool ok=0;
            for (int j=0;j<withheld_p[i].size();j++) {
                if (withheld_p[i][j].data()==from.data()) {ok=1;break;}
            }
            if (ok) {
                withheld_ev.removeAt(i);
                withheld_p.removeAt(i);
                withheld_obj.removeAt(i);
                withheld_reverse.removeAt(i);
                i--;
            }
        }
    }
}

void SecretMidi::mWithhold(live::Event* x,live::ObjectChain p,live::ObjectPtr obj, bool reverse) {
    live_mutex(x_midi) {
        x->buddy=0;
        withheld_ev.push_back(x);
        withheld_p.push_back(p);
        withheld_obj.push_back(obj);
        withheld_reverse.push_back(reverse);
    }
}

void SecretMidi::mRemoveWithheld(live::ObjectPtr obj) {
    live_mutex(x_midi) {
        for (int i=0;i<withheld_ev.size();i++) {
            if (withheld_obj[i]==obj) {
                delete withheld_ev.takeAt(i);
                withheld_p.removeAt(i);
                withheld_obj.removeAt(i);
                withheld_reverse.removeAt(i);
                --i;
            }
        }
    }
}

void SecretMidi::mRemoveWithheld_object_dest(live::Object* obj) {
    live_mutex(x_midi) {
        for (int i=0;i<withheld_ev.size();i++) {
            if (withheld_obj[i].data()==obj) {
                delete withheld_ev.takeAt(i);
                withheld_p.removeAt(i);
                withheld_obj.removeAt(i);
                withheld_reverse.removeAt(i);
                --i;
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
  : live::Object(ccname,true,false)
  , valid(1)
  , deviceId(devId)
  , s_null(live::audio::null(2))
  { setTemporary(0);

    live::ObjectPtr*x=new live::ObjectPtr[200];
    for (int i=0;i<200;i++) {
        x[i]=0;
    }
    live::MidiBinding::customKey->insert(this,x);

    s_null->hybridConnect(this);
}

MidiNull::MidiNull() :
    live::Object("Null Midi Device",false,false) {

}

void MidiNull::mIn(const live::Event *, live::ObjectChain*) {
}

void MidiOut::mIn(const live::Event *ev, live::ObjectChain*p) {
    qDebug() << "OUT _ IN"<<ev->message<<" "<<ev->data1<<" "<<ev->data2;
    setTemporary(0);

    if (!ev) return;
    if (ev->flareId!=-1) {
        if (ev->flareId==0) {
            if (p->size()) SecretMidi::me->cancel(p->first());
        }
    } else {
        SecretMidi::me->queue(ev,device, *p);
        // pmouts and outputs are equiv. in different systems
    }
}

//from the test program in portmidi
//{
#define INPUT_BUFFER_SIZE 100
#define OUTPUT_BUFFER_SIZE 0
#define DRIVER_INFO NULL
#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL
//}

void live::midi::refresh() {
    if (!SecretMidi::me) new SecretMidi;
    SecretMidi::me->refresh();
}

live::ObjectPtr live::midi::null() {
    if (!SecretMidi::me) new SecretMidi;

    return new MidiNull;
}

int live::midi::getTime_msec() {
    return TIME_PROC(TIME_INFO);
}

live::Time live::midi::getTime() {
    return live::Time(getTime_msec());
}

#endif // !defined(__QNX__)
