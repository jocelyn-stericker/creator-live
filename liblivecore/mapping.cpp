/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/mapping>
live::Mapping::Mapping(QString cname,live::ObjectPtr other)
  : live::Object(cname,false,false, 2)
  , m_other(other)
  , m_connection(other, this, live::HybridConnection)
  {
}

void live::Mapping::aIn (const float*data,int chan,live::Object*p) { if (p==m_other.data()) aOut(data,chan,p); else m_other->aIn(data,chan, p); }
bool live::Mapping::isAudioObject() const { return m_other->isAudioObject(); }

void live::Mapping::mIn(const live::Event*data,live::ObjectChain*p) { if (p->back()==m_other) mOut(data,p); else m_other->mIn(data,p); }
bool live::Mapping::isMidiObject() const { return m_other->isMidiObject(); }

bool live::Mapping::isInput() const {return m_other->isInput();}
bool live::Mapping::isOutput() const {return m_other->isOutput();}

