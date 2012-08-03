/*****************************************************
vst_dotlive.cpp

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

#include "vst.h"
#include <QByteArray>

#ifndef VST_DOTLIVE_CPP
#define VST_DOTLIVE_CPP
#define LOAD
#include "vst_dotlive.cpp"
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
//using namespace live_widgets;

#ifdef LOAD
Vst* Vst::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray Vst::save()
#endif
{
    BEGIN;
    Q_UNUSED(xbool);
    Q_UNUSED(xint32);
    Q_UNUSED(xba);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN Vst"));

    /*002*/
    (verify(ret,(qint32 )110708));


    //////////////////////////////////////////////////////////////////

    Vst*x=IS_SAVE?_THIS:0;

    QString xstr=IS_SAVE?x->filename:"";
    ret IO xstr; /*C01*/

    QString xstr2=IS_SAVE?x->instname:"";
    ret IO xstr2; /*C02*/

    x=IS_SAVE?x:new Vst(xstr,xstr2);

    /*003*/
    ret IO x->vstId;

    /*004*/
    (verify(ret,(QString)"END Vst"));

    RETURN;
}

