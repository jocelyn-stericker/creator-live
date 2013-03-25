/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef MIDISYSTEM_P_H
#define MIDISYSTEM_P_H

#include "live/object"
#ifndef __QNX__
#include <portmidi.h>
#include <porttime.h>
#endif
#include <QMutex>
#include <QThread>

namespace live_private {

class LIBLIVECORESHARED_EXPORT MidiIn : public live::Object
{
public:
    LIVE_MIDI
    LIVE_INPUT
    bool valid;
    int deviceId;
    live::ObjectPtr m_null;
    live::Connection m_connection;
    MidiIn(QString ccname,int devId);
    virtual void mIn(const live::Event *ev, live::ObjectChain*p)
    {
        p->push_back(this);
        mOut(ev,p);
        p->pop_back();
    }
};

class LIBLIVECORESHARED_EXPORT MidiOut : public live::Object
{
public:
    LIVE_MIDI
    LIVE_OUTPUT
    bool valid;
    int device;

public:
    MidiOut(QString ccname,int cdevice) : live::Object(ccname,true,true,2), valid(1), device(cdevice) { setTemporary(0); }
    virtual void mIn(const live::Event *data, live::ObjectChain*p);   //Midi.cpp
};

class LIBLIVECORESHARED_EXPORT MidiNull : public live::Object
{
public:
    LIVE_MIDI
    LIVE_INPUT
    MidiNull();
    virtual void mIn(const live::Event *, live::ObjectChain*);
};

class LIBLIVECORESHARED_EXPORT SecretMidi : public QThread
{
    Q_OBJECT //Midi.cpp
public:
    static SecretMidi* me;
    QList< MidiIn* > inputs;
    QList< MidiOut*> outputs;
    MidiNull* null;

    QMutex x_midi; // Every function must use this.

#ifndef __QNX__
    QList< PmStream* > pmouts;
    QList< PmStream* > pmins;
    QList< PmEvent*  > pmqueue;
#endif
    QList< int       > idqueue;
    QList< live::ObjectChain > fromqueue;

    struct Withheld {
        live::Event* ev;
        live::ObjectChain p;
        live::ObjectPtr obj;
        bool reverse;
        Withheld(live::Event* cev, live::ObjectChain cp, live::ObjectPtr cobj, bool creverse)
          : ev(cev)
          , p(cp)
          , obj(cobj)
          , reverse(creverse)
          {
        }

        Withheld(const Withheld& w)
          : ev(w.ev)
          , p(w.p)
          , obj(w.obj)
          , reverse(w.reverse)
          { // Careful.
        }

        Withheld& operator=(const Withheld& w) {
            ev = w.ev;
            p = w.p;
            obj = w.obj;
            reverse = w.reverse;
            // Careful.
            return *this;
        }
    };

    QList<Withheld> withheld;

public:
    SecretMidi()
      : QThread()
      , inputs()
      , outputs()
      , null(new MidiNull)
      , x_midi(QMutex::Recursive)
  #ifndef __QNX__
      , pmouts()
      , pmins()
      , pmqueue()
      , idqueue()
  #endif
      , fromqueue()
      , withheld()
      { init();
    }

    void init();
#ifdef __linux__
    void run() __attribute__ ((noreturn));
#else
    void run();
#endif

    void refresh();
    void queue(const live::Event* ev, int device, live::ObjectChain from);
    void cancel(live::ObjectPtr from);
    void mWithhold(live::Event* x, live::ObjectChain p, live::ObjectPtr obj, bool reverse=0);
    void mRemoveWithheld(live::ObjectPtr obj);
    void mClearEventsTo(live::Object* obj);
private:
    Q_DISABLE_COPY(SecretMidi)
};

}

#endif // MIDISYSTEM_P_H
