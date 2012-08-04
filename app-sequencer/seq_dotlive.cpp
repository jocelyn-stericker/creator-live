/*****************************************************
seq_dotlive.cpp

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield    (joshua@nettek.ca)

                 All rights reserved.

 To ensure save-load compatability, whenever possible
 saving and loading code is kept the same. It's a bit
 ugly, but _probably_ safer than writing two distinct
 functions in each class. In each class' header file,
 comments in the form XXX indicate the order in which
 properties are written here. You can thank my OCD...
*****************************************************/

#include "looperapp.h"
#include "sequencerapp.h"
#include "looperframe.h"
#include "sequencerframe.h"
#include "sequencersys.h"
#include <QByteArray>

#ifndef SEQ_DOTLIVE_CPP
#define SEQ_DOTLIVE_CPP
#define LOAD
#include "seq_dotlive.cpp"
#undef LOAD
#undef BEGIN
#undef IS_LOAD
#undef IS_SAVE
#undef IO
#undef _THIS
#undef RETURN
#undef verify
#define SAVE
#endif

#ifdef LOAD
#define BEGIN QDataStream ret(str); QString xqstring; qint32 xint32; bool xbool; QByteArray xba
#define IS_LOAD 1
#define IS_SAVE 0
#define IO >>
#define _THIS 0
#define RETURN return x;
template<typename T> bool verify1(QDataStream&ret,T chk)
{
    T b=chk;
    ret IO b;
    if (b!=chk)
    {
        qCritical() << "Found"<<b;
        qCritical() << "Should be"<<chk;
        Q_ASSERT(0);
    }
    return b==chk;
}
#define verify(x,y) verify1(x,y)
#endif
#ifdef SAVE
#define BEGIN QByteArray str; QDataStream ret(&str,QIODevice::WriteOnly); QString xqstring; qint32 xint32; bool xbool; QByteArray xba
#define IS_LOAD 0
#define IS_SAVE 1
#define IO <<
#define _THIS this
#define RETURN return str

template<typename T> bool verify2(QDataStream&ret,T chk)
{
    T b=chk;
    ret IO b;
    if (b!=chk)
    {
        qCritical() << "Found"<<b;
        qCritical() << "Should be"<<chk;
        Q_ASSERT(0);
    }
    return b==chk;
}
#define verify(x,y) verify2(x,y)
#endif


#ifndef TYPES_
#define TYPES_
#define P_INT32(var) \
    do { \
    xint32=var; \
    ret IO xint32; \
    var=xint32; \
    } while (0)

#define P_BOOL(var) \
    do { \
    xbool=var; \
    ret IO xbool; \
    var=xbool; \
    } while (0)

#define P_QSTRING(var) \
    do { \
    xqstring=var; \
    ret IO xqstring; \
    var=xqstring; \
    } while (0)
#endif

using namespace live;
using namespace live_widgets;

#ifdef LOAD
live::ObjectPtr LooperApp::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray LooperApp::save()
#endif
{
    BEGIN;
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN LooperApp"));

    /*002*/
    (verify(ret,(qint32 )110708));


    //////////////////////////////////////////////////////////////////


    LooperApp*x=IS_SAVE?_THIS:0;


    //////////////////////////////////////////////////////////////////
    // FROM Sequencer::
    /////////

    /*003*/
    MidiTrack*mt=0;

    xba=IS_SAVE?x->s_midiTrack->save():QByteArray();
    ret IO xba;
    if (IS_LOAD)
    {
        mt = MidiTrack::load(xba);
    }

    /*004*/
    AudioTrack*at=0;

    xba=IS_SAVE?x->s_audioTrack->save():QByteArray();
    ret IO xba;
    if (IS_LOAD)
    {
        at = AudioTrack::load(xba);
    }

    /*005*/
    MidiEventCounter*mc=0;

    xba=IS_SAVE?x->s_counter->save():QByteArray();
    ret IO xba;
    if (IS_LOAD)
    {
        mc = MidiEventCounter::load(xba);
    }

    if (IS_LOAD)
    {
        x=new LooperApp(mt,at,mc);
    }

    /*006/7*/
    P_INT32(x->s_id);
    if (x->s_id>(SequencerSys::self?SequencerSys::self:new SequencerSys)->_lastId)
    {
        (SequencerSys::self?SequencerSys::self:new SequencerSys)->_lastId=x->s_id;
    }

    /*007*/
    P_INT32(x->s_scale);

    //////////////////////////////////////////////////////////////////
    // FROM LooperApp::
    /////////

    /*008*/
    int loopMode=x->b_loopMode;
    P_INT32(loopMode);
    x->b_loopMode=static_cast<LoopMode>(loopMode); //!?!*!@#@?

    /*009*/
    P_INT32(x->b_loopLength);

    /*010*/
    P_INT32(x->s_id_looper);
    if (x->s_id_looper>x->s_lastId)
    {
        x->s_lastId=x->s_id_looper;
    }

    /*011*/
    (verify(ret,(QString)"END LooperApp"));

    RETURN;
}

#ifdef LOAD
live::ObjectPtr SequencerApp::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray SequencerApp::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN SequencerApp"));

    /*002*/
    (verify(ret,(qint32 )110714));


    //////////////////////////////////////////////////////////////////


    SequencerApp*x=IS_SAVE?_THIS:0;

    Q_UNUSED(x);

    //////////////////////////////////////////////////////////////////

    /*003*/
    MidiTrack* cmidiTrack=0;
    Q_UNUSED(cmidiTrack);

    xba=IS_SAVE?x->s_midiTrack->save():QByteArray();
    ret IO xba;
    if (IS_LOAD)
    {
        cmidiTrack = MidiTrack::load(xba);
    }

    /*004*/
    AudioTrack* caudioTrack=0;
    Q_UNUSED(caudioTrack);

    xba=IS_SAVE?x->s_audioTrack->save():QByteArray();
    ret IO xba;
    if (IS_LOAD)
    {
        caudioTrack = AudioTrack::load(xba);
    }

    /*005*/
    MidiEventCounter* cmidicounter=0;
    Q_UNUSED(cmidicounter);

    xba=IS_SAVE?x->s_counter->save():QByteArray();
    ret IO xba;
    if (IS_LOAD)
    {
        cmidicounter = MidiEventCounter::load(xba);
    }

    //
    if (IS_LOAD)
    {
        x=new SequencerApp("SEQUENCER",cmidiTrack,caudioTrack,cmidicounter,0);
    }

    //because audio/midi tracks take time to load, we might be a bit off now...
    if (IS_LOAD)
    {
        x->setPos(cmidiTrack->pos());
    }

    //////////////////////////////////////////////////////////////////

    /*006*/
    P_BOOL(x->b_clipped);

    /*007*/
    P_INT32(x->s_id);
    if (x->s_id>(SequencerSys::self?SequencerSys::self:new SequencerSys)->_lastId)
    {
        (SequencerSys::self?SequencerSys::self:new SequencerSys)->_lastId=x->s_id;
    }

    /*008*/
    P_INT32(x->s_scale);

    /*009*/
    (verify(ret,(QString)"END SequencerApp"));

    RETURN;
}

