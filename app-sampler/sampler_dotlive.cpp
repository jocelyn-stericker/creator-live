/*****************************************************
sampler_dotlive.cpp

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

#include "samplerapp.h"
#include "samplerframe.h"
#include "live/midibinding.h"
#include <QByteArray>
#include <QDataStream>

#ifndef SAMPLER_DOTLIVE_CPP
#define SAMPLER_DOTLIVE_CPP
#define LOAD
#include "sampler_dotlive.cpp"
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
    if(b!=chk)
    {
        qCritical()<<"Found"<<b;
        qCritical()<<"Should be"<<chk;
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
    if(b!=chk)
    {
        qCritical()<<"Found"<<b;
        qCritical()<<"Should be"<<chk;
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
    } while(0)

#define P_BOOL(var) \
    do { \
    xbool=var; \
    ret IO xbool; \
    var=xbool; \
    } while(0)

#define P_QSTRING(var) \
    do { \
    xqstring=var; \
    ret IO xqstring; \
    var=xqstring; \
    } while(0)
#endif

using namespace live;
using namespace live_widgets;


#ifdef LOAD
live::ObjectPtr SamplerApp::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray SamplerApp::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN SamplerAppApp"));

    /*002*/
    (verify(ret,(qint32 )110731));


    //////////////////////////////////////////////////////////////////


    SamplerApp*x=IS_SAVE?_THIS:0;
    MidiTrack* midiTracks[16];      /*003*/
    AudioTrack* audioTracks[16];    /*004*/
    for(int i=0;i<16;i++)
    {
        if(IS_SAVE)
        {
            xba=x->s_midiTracks[i]->save();
        }
        ret IO xba;
        if(IS_LOAD)
        {
            midiTracks[i]=MidiTrack::load(xba);
        }


        if(IS_SAVE)
        {
            xba=x->s_audioTracks[i]->save();
        }
        ret IO xba;
        if(IS_LOAD)
        {
            audioTracks[i]=AudioTrack::load(xba);
        }
    }

    qint32 id=IS_SAVE?x->s_id:-1;
    ret IO id;  /*005*/
    if(IS_LOAD)
    {
        x=new SamplerApp(midiTracks,audioTracks,0);
        x->s_id=id;
        x->s_lastId=qMax(x->s_lastId,id);
    }

    bool record=IS_SAVE?x->s_record:0;
    ret IO record;  /*006*/
    if(record&&IS_LOAD)
    {
        x->setRecordMode();
    }

    bool multi=IS_SAVE?x->s_multi:0;
    ret IO multi;   /*007*/
    if(!multi&&IS_LOAD)
    {
        x->unsetMultiMode();
    }

    qint32 count=IS_SAVE?x->s_customBindings.size():0;
    ret IO count;   /*008*/
    for(int i=0;i<count;i++)
    {
        QString inname=IS_SAVE?x->s_customBindings.keys()[i]->name():0;
        ret IO inname;
        ObjectPtr loadptr(0);
        int lcount=200;
        ret IO lcount;
        if(IS_LOAD)
        {
            x->s_customBindings.insert(loadptr=object::request(inname,MidiOnly|InputOnly),new int[lcount]);
        }
        for(int j=0;j<lcount;j++)
        {
            ret IO x->s_customBindings.values()[i][j];
            if(x->s_customBindings.values()[i][j]!=-1&&IS_LOAD)
            {
                MidiBinding::customKey->value(loadptr)[j]=x;
            }
        }
    }

    /*009*/
    (verify(ret,(QString)"END SamplerApp"));

    RETURN;
}
