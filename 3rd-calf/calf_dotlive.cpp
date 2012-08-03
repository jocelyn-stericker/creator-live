/* This file is part of the Calf Creator Live plugin.
 * The Calf Creator Live plugin is a fork of the Calf DSP library.
 *
 * Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "fjreverbframe.h" // this will include everything else.
#include <live/midibinding.h>

#include <QByteArray>
#include <QDataStream>

#ifndef SAMPLER_DOTLIVE_CPP
#define SAMPLER_DOTLIVE_CPP
#define LOAD
#include "calf_dotlive.cpp"
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
    if (b!=chk)
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
live::ObjectPtr FJReverbApp::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray FJReverbApp::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN FJReverbApp"));

    /*002*/
    (verify(ret,(qint32 )120828));


    //////////////////////////////////////////////////////////////////


    FJReverbApp*x=IS_SAVE?_THIS:0;

    float xf=IS_SAVE?x->getRoomSize():0;
    ret IO xf;  /*003*/
    if (IS_LOAD) x->setRoomSize((RoomSize)xf);

    xf=IS_SAVE?x->getDiffusion():0;
    ret IO xf;  /*004*/
    if (IS_LOAD) x->setDiffusion(xf);

    xf=IS_SAVE?x->getDecayTime():0;
    ret IO xf;  /*005*/
    if (IS_LOAD) x->setDecayTime(xf);

    xf=IS_SAVE?x->getHFDamp():0;
    ret IO xf;  /*006*/
    if (IS_LOAD) x->setHFDamp(xf);

    xf=IS_SAVE?x->getWet():0;
    ret IO xf;  /*007*/
    if (IS_LOAD) x->setWet(xf);

    xf=IS_SAVE?x->getDry():0;
    ret IO xf;  /*008*/
    if (IS_LOAD) x->setDry(xf);

    xf=IS_SAVE?x->getTrebleCut():0;
    ret IO xf;  /*009*/
    if (IS_LOAD) x->setTrebleCut(xf);

    xf=IS_SAVE?x->getBassCut():0;
    ret IO xf;  /*010*/
    if (IS_LOAD) x->setBassCut(xf);

    xf=IS_SAVE?x->getPredelay():0;
    ret IO xf;  /*011*/
    if (IS_LOAD) x->setPredelay(xf);

    /*012*/
    (verify(ret,(QString)"END FJReverbApp"));

    RETURN;
}

#ifdef LOAD
live::ObjectPtr FJFlangerApp::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray FJFlangerApp::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN FJFlangerApp"));

    /*002*/
    (verify(ret,(qint32 )120829));


    //////////////////////////////////////////////////////////////////


    FJFlangerApp*x=IS_SAVE?_THIS:0;

    float xf=IS_SAVE?x->getDelay():0;
    ret IO xf;  /*003*/
    if (IS_LOAD) x->setDelay(xf);

    xf=IS_SAVE?x->getDepth():0;
    ret IO xf;  /*004*/
    if (IS_LOAD) x->setDepth(xf);

    xf=IS_SAVE?x->getRate():0;
    ret IO xf;  /*005*/
    if (IS_LOAD) x->setRate(xf);

    xf=IS_SAVE?x->getFB():0;
    ret IO xf;  /*006*/
    if (IS_LOAD) x->setFB(xf);

    xf=IS_SAVE?x->getStereo():0;
    ret IO xf;  /*007*/
    if (IS_LOAD) x->setStereo(xf);

    xf=IS_SAVE?x->getReset():0;
    ret IO xf;  /*008*/
    if (IS_LOAD) x->setReset(xf);

    xf=IS_SAVE?x->getAmount():0;
    ret IO xf;  /*009*/
    if (IS_LOAD) x->setAmount(xf);

    xf=IS_SAVE?x->getDryAmount():0;
    ret IO xf;  /*010*/
    if (IS_LOAD) x->setDryAmount(xf);

    /*011*/
    (verify(ret,(QString)"END FJFlangerApp"));

    RETURN;
}


#ifdef LOAD
live::ObjectPtr FJFilterApp::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray FJFilterApp::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN FJFilterApp"));

    /*002*/
    (verify(ret,(qint32 )120829));


    //////////////////////////////////////////////////////////////////


    FJFilterApp*x=IS_SAVE?_THIS:0;

    float xf=IS_SAVE?x->getCutoff():0;
    ret IO xf;  /*003*/
    if (IS_LOAD) x->setCutoff(xf);

    xf=IS_SAVE?x->getResonance():0;
    ret IO xf;  /*004*/
    if (IS_LOAD) x->setResonance(xf);

    xf=IS_SAVE?x->getMode():0;
    ret IO xf;  /*005*/
    if (IS_LOAD) x->setMode((Filter)(int)xf);

    xf=IS_SAVE?x->getInertia():0;
    ret IO xf;  /*006*/
    if (IS_LOAD) x->setIntertia(xf);

    /*017*/
    (verify(ret,(QString)"END FJFilterApp"));

    RETURN;
}


#ifdef LOAD
live::ObjectPtr FJPhaserApp::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray FJPhaserApp::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN FJPhaserApp"));

    /*002*/
    (verify(ret,(qint32 )120829));


    //////////////////////////////////////////////////////////////////


    FJPhaserApp*x=IS_SAVE?_THIS:0;

    float xf=IS_SAVE?x->getFreq():0;
    ret IO xf;  /*003*/
    if (IS_LOAD) x->setFreq(xf);

    xf=IS_SAVE?x->getDepth():0;
    ret IO xf;  /*004*/
    if (IS_LOAD) x->setDepth(xf);

    xf=IS_SAVE?x->getRate():0;
    ret IO xf;  /*005*/
    if (IS_LOAD) x->setRate(xf);

    xf=IS_SAVE?x->getFB():0;
    ret IO xf;  /*006*/
    if (IS_LOAD) x->setFB(xf);

    xf=IS_SAVE?x->getStages():0;
    ret IO xf;  /*007*/
    if (IS_LOAD) x->setStages(xf);

    xf=IS_SAVE?x->getAmount():0;
    ret IO xf;  /*008*/
    if (IS_LOAD) x->setAmount(xf);

    xf=IS_SAVE?x->getDryAmount():0;
    ret IO xf;  /*009*/
    if (IS_LOAD) x->setDryAmount(xf);

    /*010*/
    (verify(ret,(QString)"END FJPhaserApp"));

    RETURN;
}

