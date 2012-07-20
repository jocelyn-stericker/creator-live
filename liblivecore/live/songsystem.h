/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef SONGSYS_H
#define SONGSYS_H

#include "live/object.h"
#include "live/metronome.h"
#include "live/keysignature.h"
#include "live/midifilter.h"

namespace live {

class LIBLIVECORESHARED_EXPORT song
{
protected:
    static song* s_current;
public:
    static song* current() {
        if(!s_current) s_current=new song("Song");
        return s_current;
    }
    static void setCurrent(song* s) {
        s_current = s;
    }

    static QList<song*> universe;

    Metronome* metronome;               /*003*/
    KeySignature* keySignature;         /*004*/
    QMap<ObjectPtr,MidiFilter*> midiMaps;   /*005*/
    QString name;                       /*006*/
    QString songName;                   /*007*/
    QStringList colours;

    song(QString cname);
    ~song();

protected:
    song();
public:
    static song* load(const QByteArray&str);
    QByteArray save();
};

}

#endif // SONGSYS_H
