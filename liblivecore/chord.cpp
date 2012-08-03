/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/chord.h>

QList<live::Pitch> live::SpecificChord::notes(live::KeySignature)
{
    return *this;
}
