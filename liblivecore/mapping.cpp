/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/mapping.h>
live::Mapping::Mapping(QString cname,live::ObjectPtr other) : live::Object(cname,false,false),s_other(other) { other->hybridConnect(this); }

void live::Mapping::aIn (const float*data,int chan,live::ObjectChain&p) { if (p.back()==s_other) aOut(data,chan,p); else s_other->aIn(data,chan,p); }
bool live::Mapping::isAudioObject() { return s_other->isAudioObject(); }

void live::Mapping::mIn(const live::Event*data,live::ObjectChain&p) { if (p.back()==s_other) mOut(data,p); else s_other->mIn(data,p); }
bool live::Mapping::isMidiObject() { return s_other->isMidiObject(); }

bool live::Mapping::isInput(){return s_other->isInput();}
bool live::Mapping::isOutput(){return s_other->isOutput();}

