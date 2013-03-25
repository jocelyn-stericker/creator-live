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
   m_internal(0),m_id(++s_lastId),m_isInit(0),m_filename("")
{
}

VstEffectApp::~VstEffectApp()
{
    deletem_internal;
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
       m_internal->aIn(data,chan,p);
        p->pop_back();
    }
}

void VstEffectApp::mIn(const Event *data, ObjectChain*p)
{
    p->push_back(this);
    if(s_internal)
    {
       m_internal->mIn(data,p);
    }
    mOut(data,p);
    p->pop_back();
}

void VstEffectApp::init(QString path)
{
    Q_ASSERT(!s_internal);
   m_isInit=1;
   m_filename=path;
   m_internal=new Vst(path);
    //ASSERT THAT IT IS VALID!!!
   m_internal->hybridConnect(this);
   m_internal->show();
}

void VstEffectApp::show()
{
    Q_ASSERT(s_internal);
   m_internal->show();
}
