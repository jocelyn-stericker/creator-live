/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef TIME_H
#define TIME_H

#include <qglobal.h>
#include <QByteArray>

#include "liblivecore_global.h"

namespace live {

class LIBLIVECORESHARED_EXPORT Time
{
public:
    qint32 sec;     /*003*/
    qint32 nsec;    /*004*/

    Time() : sec(-1), nsec(-1) {}

    Time(qint32 time_ms) : sec(time_ms/1000), nsec(time_ms%1000*1000) { }

    qint32 toTime_ms() const;
    QByteArray save();
    static Time* load(const QByteArray&str);
};

}

LIBLIVECORESHARED_EXPORT bool operator <(live::Time a,live::Time b);
LIBLIVECORESHARED_EXPORT bool operator <(live::Time a,qint32 b);
LIBLIVECORESHARED_EXPORT bool operator <(qint32 a,live::Time b);
LIBLIVECORESHARED_EXPORT bool operator <=(live::Time a, live::Time b);
LIBLIVECORESHARED_EXPORT bool operator <=(live::Time a,qint32 b);
LIBLIVECORESHARED_EXPORT bool operator <=(qint32 a,live::Time b);
LIBLIVECORESHARED_EXPORT bool operator >(live::Time a,live::Time b);
LIBLIVECORESHARED_EXPORT bool operator >(live::Time a,qint32 b);
LIBLIVECORESHARED_EXPORT bool operator >(qint32 a,live::Time b);
LIBLIVECORESHARED_EXPORT bool operator >=(live::Time a,live::Time b);
LIBLIVECORESHARED_EXPORT bool operator >=(live::Time a,qint32 b);
LIBLIVECORESHARED_EXPORT bool operator >=(qint32 a,live::Time b);

LIBLIVECORESHARED_EXPORT live::Time operator +(live::Time a,live::Time b);
LIBLIVECORESHARED_EXPORT live::Time operator +(live::Time a,qint32 b);
LIBLIVECORESHARED_EXPORT live::Time operator +(qint32 a,live::Time b);
LIBLIVECORESHARED_EXPORT live::Time operator +=(live::Time a,live::Time b);
LIBLIVECORESHARED_EXPORT live::Time operator +=(live::Time a,qint32 b);
LIBLIVECORESHARED_EXPORT live::Time operator +=(qint32 a,live::Time b);
LIBLIVECORESHARED_EXPORT live::Time operator -(live::Time a,live::Time b);
LIBLIVECORESHARED_EXPORT live::Time operator -(live::Time a,qint32 b);
LIBLIVECORESHARED_EXPORT live::Time operator -(qint32 a,live::Time b);
LIBLIVECORESHARED_EXPORT live::Time operator -=(live::Time a,live::Time b);
LIBLIVECORESHARED_EXPORT live::Time operator -=(live::Time a,qint32 b);
LIBLIVECORESHARED_EXPORT live::Time operator -=(qint32 a,live::Time b);

#endif // TIME_H
