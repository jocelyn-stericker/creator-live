/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef TIMESIGNATURE_H
#define TIMESIGNATURE_H

#include "liblivecore_global.h"

class QByteArray;

namespace live {

class LIBLIVECORESHARED_EXPORT TimeSignature
{
public:
    int numerator;      /*003*/
    int denomenator;    /*004*/

    TimeSignature(int cnum,int cden);

    QByteArray save();
    static TimeSignature* load(const QByteArray&str);
protected:
    TimeSignature() {}
};

}

#endif // TIMESIGNATURE_H
