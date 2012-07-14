/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef KEYSIGNATURE_H
#define KEYSIGNATURE_H

#include "live/pitch.h"
#include "liblivecore_global.h"

namespace live {

class LIBLIVECORESHARED_EXPORT KeySignature : public Pitch
{
public:
    enum Tonality
    {
         Major,
        Minor
    };

    Tonality s_tonality;    /*006*/

    KeySignature(char croot,Accidental cacc,Tonality cton);
    KeySignature(char croot,char cacc,Tonality cton);
    KeySignature(QString cpitch,Tonality cton);

    QByteArray save();
    static KeySignature* load(const QByteArray&str);
protected:
    KeySignature() : Pitch() {}
};

}

#endif // KEYSIGNATURE_H
