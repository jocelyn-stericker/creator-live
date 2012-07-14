/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <QStringList>
#include "live/object.h"

namespace live {

class InstrumentInterface;

class LIBLIVECORESHARED_EXPORT instrument
{
protected:
    static instrument* s_singleton;
    QStringList s_instrumentNames;
    QList<InstrumentInterface*> s_instruments;
    instrument() {}
    ~instrument();

public:
    static void registerInterface(InstrumentInterface* c);
    static QStringList names();
    static QList<InstrumentInterface*> interfaces();
};

}

#endif // INSTRUMENT_H
