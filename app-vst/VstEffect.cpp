/*****************************************************
VSTEffect.cpp                              rev. 110720

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield  (trintith@gmail.com)

                 All rights reserved.
*****************************************************/

#include "src/Apps/Vst/VstEffect.h"

int VstEffectApp::s_lastId=-1;

VstEffectApp::VstEffectApp() :
    LObject("VST Effect App"),
    s_internal(0), s_id(++s_lastId), s_isInit(0), s_filename("")
{
}

VstEffectApp::~VstEffectApp()
{
    delete s_internal;
}

void VstEffectApp::aIn(const float *data, int chan, Object*p)
{
    if(p->back()==s_internal)
    {
        aOut(data,chan,p);
        return;
    }
    if(!s_internal)
    {
        p->push_back(this);
        aOut(data,chan,p);
        p->pop_back();
    }
    else
    {
        p->push_back(this);
        s_internal->aIn(data,chan,p);
        p->pop_back();
    }
}

void VstEffectApp::mIn(const Event *data, ObjectChain*p)
{
    p->push_back(this);
    if(s_internal)
    {
        s_internal->mIn(data,p);
    }
    mOut(data,p);
    p->pop_back();
}

void VstEffectApp::init(QString path)
{
    Q_ASSERT(!s_internal);
    s_isInit=1;
    s_filename=path;
    s_internal=new Vst(path);
    //ASSERT THAT IT IS VALID!!!
    s_internal->hybridConnect(this);
    s_internal->show();
}

void VstEffectApp::show()
{
    Q_ASSERT(s_internal);
    s_internal->show();
}
