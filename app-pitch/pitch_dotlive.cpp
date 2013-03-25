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

#include "pitchapp.h" // this will include everything else.
#include <live/midibinding>
#include <QByteArray>
#include <QDataStream>

#ifndef PITCH_DOTLIVE_CPP
#define PITCH_DOTLIVE_CPP
#define LOAD
#include "pitch_dotlive.cpp"
#undef LOAD
#undef BEGIN
#undef IS_LOAD
#undef IS_SAVE
#undef IO
#undef _THIS
#undef RETURN
#undef verify
#define SAVE
#endif // PITCH_DOTLIVE_CPP

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
//using namespace live_widgets;

#ifdef LOAD
PitchApp* PitchApp::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray PitchApp::save()
#endif
{
    BEGIN;
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN PitchApp"));

    /*002*/
    (verify(ret,(qint32 )120729));


    //////////////////////////////////////////////////////////////////


    PitchApp*x=IS_SAVE?_THIS:new PitchApp;

    /*003*/ P_INT32(x->m_stShift);

    /*004*/
    P_INT32(x->m_id);
    if (x->m_id>x->m_lastId)
    {
        x->m_lastId=x->m_id;
    }

    /*005*/
    (verify(ret,(QString)"END PitchApp"));

    RETURN;
}
