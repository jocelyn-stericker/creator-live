/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef MIDISYSTEM_P_H
#define MIDISYSTEM_P_H

#include "live/object.h"
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
    live::ObjectPtr s_null;
    MidiIn(QString ccname,int devId);
    virtual void mIn(const live::Event *ev, live::ObjectChain&p)
    {
        p.push_back(this);
        mOut(ev,p);
        p.pop_back();
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
    MidiOut(QString ccname,int cdevice) : live::Object(ccname,true,true), valid(1), device(cdevice) { setTemporary(0); }
    virtual void mIn(const live::Event *data, live::ObjectChain&p);   //Midi.cpp
};

class LIBLIVECORESHARED_EXPORT MidiNull : public live::Object
{
public:
    LIVE_MIDI
    LIVE_INPUT
    MidiNull();
    virtual void mIn(const live::Event *, live::ObjectChain&);
};

#ifndef __QNX__
class LIBLIVECORESHARED_EXPORT SecretMidi : public QThread
{
    Q_OBJECT //Midi.cpp
public:
    static SecretMidi* me;
    QList< MidiIn* > inputs;
    QList< MidiOut*> outputs;
    MidiNull* null;
    QList< PmStream* > pmouts;
    QList< PmStream* > pmins;
    QList< PmEvent*  > pmqueue;
    QList< int       > idqueue;
    QList< live::ObjectChain > fromqueue;

    QList<live::Event*> withheld_ev;
    QList<live::ObjectChain> withheld_p;
    QList<live::ObjectPtr> withheld_obj;
    QList<bool> withheld_reverse;

public:
    SecretMidi() : QThread(), inputs(),outputs(),null(new MidiNull),pmouts(),pmins(),pmqueue(), idqueue(), csMutex(QMutex::Recursive)
    {
        init();
    }

    void init();
    void run();
    void refresh();
    void queue(const live::Event* ev, int device, live::ObjectChain from);
    void cancel(live::ObjectPtr from);
    void mWithhold(live::Event* x, live::ObjectChain p, live::ObjectPtr obj, bool reverse=0);
    void mRemoveWithheld(live::ObjectPtr obj);
    void mRemoveWithheld_object_dest(live::Object* obj);

private:
    QMutex csMutex;
};
#else
class LIBLIVECORESHARED_EXPORT SecretMidi : public QThread
{
    Q_OBJECT //Midi.cpp
public:
    static SecretMidi* me;
    QList< MidiIn* > inputs;
    QList< MidiOut*> outputs;
    MidiNull* null;

    QList< int       > idqueue;
    QList< live::ObjectChain > fromqueue;
    QList<live::Event*> withheld_ev;
    QList<live::ObjectChain> withheld_p;
    QList<live::ObjectPtr> withheld_obj;
    QList<bool> withheld_reverse;

public:
    SecretMidi() : QThread(), inputs(),outputs(),null(new MidiNull),/*pmouts(),pmins(),pmqueue(), idqueue(), */csMutex(QMutex::Recursive)
    {
        init();
    }

    void init();
    void run();
    void refresh();
    void queue(const live::Event* ev, int device, live::ObjectChain from);
    void cancel(live::ObjectPtr from);
    void mWithhold(live::Event* x, live::ObjectChain p, live::ObjectPtr obj, bool reverse=0);
    void mRemoveWithheld(live::ObjectPtr obj);
    void mRemoveWithheld_object_dest(live::Object* obj);

private:
    QMutex csMutex;
};
#endif

}

#endif // MIDISYSTEM_P_H
