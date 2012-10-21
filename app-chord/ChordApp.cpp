/*****************************************************
Chord.cpp                                  rev. 110716

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield  (trintith@gmail.com)

                 All rights reserved.
*****************************************************/

#include "src/Apps/Chord/ChordApp.h"

ChordApp::ChordApp():
        LObject("Chord"),
        QHash<Event, QList<Event> >()
{
}

void ChordApp::aIn(const float *data, int chan, Object*p)
{
    //no implementation :(
    p->push_back(this);
    aOut(data,chan,p);
    p->pop_back();
}

void ChordApp::mIn(const Event *data, ObjectChain*p)
{
    QList<Event> _x = value(*data);
    if(!_x.size())
    {
        p->push_back(this);
        mOut(data,p);
        p->pop_back();
    }
    else
    {
        for(int i=0; i<_x.size(); i++)
        {
            p->push_back(this);
            mOut(&_x[i],p);
            p->pop_back();
        }
    }
}
