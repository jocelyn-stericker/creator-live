/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef MIDISYSTEM_H
#define MIDISYSTEM_H

#include "live/object.h"
#include "live/time.h"

namespace live {

class LIBLIVECORESHARED_EXPORT midi
{
public:
    static void refresh();
    static ObjectPtr getNull();                // fast
    static int getTime_msec();             // fast, but CC
    static Time getTime();
};

}

#endif // MIDISYSTEM_H
