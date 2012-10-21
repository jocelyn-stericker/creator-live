/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/songsystem>

#if !defined(LIBLIVECORE_LIBRARY)
#error LIBLIVCORE_LIBRARY NOT DEFINED
#endif

QList<live::song*> live::song::universe;
live::song* live::song::s_current=NULL;

live::song::song(QString cname)
  : metronome(new Metronome(TimeSignature(4,4),120))
  , keySignature(new KeySignature('A',' ',KeySignature::Minor))
  , midiMaps()
  , name(cname)
  , songName("Unnamed")
  , colours() // see below
  { //don't count on "current" in this constructor
    universe.push_back(this);
    colours<<"white"<<"red"<<"green"<<"blue"<<"yellow"<<"orange"<<"purple"<<"indigo"<<"grey";
}

live::song::~song() {
    qDebug() << "DELETED SONGSYSTEM";
    delete metronome;
    delete keySignature;
}

live::song::song()
  : metronome(0)
  , keySignature(0)
  , midiMaps()
  , name("INVALID")
  , songName("INVALID")
  , colours() // see below
  {
    //don't count on "current"
    universe.push_back(this);
    colours<<"white"<<"red"<<"green"<<"blue"<<"yellow"<<"orange"<<"purple"<<"indigo"<<"grey";
}
