/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef CHORD_H
#define CHORD_H

#include "live/keysignature.h"
#include "live/pitch.h"
#include <QList>

namespace live {

class LIBLIVECORESHARED_EXPORT Chord
{
    virtual QList<Pitch> notes(KeySignature sig)=0;
};

class LIBLIVECORESHARED_EXPORT SpecificChord : public Chord, public QList<Pitch>
{
    QList<Pitch> notes(KeySignature);
    QByteArray save();
    static SpecificChord* load(const QByteArray&str);
};

}

#endif // CHORD_H
