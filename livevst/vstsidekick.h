/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#include <live/object>
#include <live/midievent>
#include "vst.h"
#ifdef __linux
#include "vstlinux_p.h"
#endif
#ifdef _WIN32
#include <live/audio>
#endif
#include "vst_p.h"

#ifndef VSTSIDEKICK_H
#define VSTSIDEKICK_H

class VstSidekick : public live::Object
{
    friend class VstR;
    live::ObjectPtr me;
    int midian;
    int audioanOffset;
    live::ObjectPtr  audioToVst;
    live::ObjectPtr  audioFromVst;
    live::ObjectPtr lastP;

    QList<live::Connection> connections;
    bool aOn() const { return 1; } bool mOn() const{ return 1; }

public:
    LIVE_HYBRID
    LIVE_EFFECT
    VstSidekick(VstR*rep,int cchan,live::ObjectPtr to,live::ObjectPtr from,int caudioanOffset=0)
      : live::Object(rep->me.s_name+"_"+QString::number(rep->me.vstId) +" Chan "+QString::number(cchan),false,false,2)
      , me(rep)
      , midian(cchan)
      , audioanOffset(caudioanOffset)
      , audioToVst(to)
      , audioFromVst(from)
    {
        // FIXME: needs mixer ability
        setTemporary(0);
        connections.push_back(live::Connection(audioFromVst, this, live::AudioConnection));
        if(audioToVst.valid())
        {
            audioToVst->newConnection();
//            cast_to_cptr<AudioOut*>(audioToVst)->newConnection();
        }
        live::object::singleton()->notify();
    }
    ~VstSidekick()
    {
        if(audioToVst.valid())
        {
            audioToVst->deleteConnection();
//            cast_to_cptr<AudioOut*>(audioToVst)->deleteConnection();
        }
        live::object::singleton()->notify();
    }

    void aIn(const float *data, int chan, live::Object* p)
    {
        if(p==audioFromVst.data())
        {
            if(chan==audioanOffset||chan==audioanOffset+1)
            {
                aOut(data,chan%2,audioToVst.valid()?lastP.data():p);
            }
            return;
        }
        lastP=this;
        if(audioToVst.valid())
        {
            audioToVst->aIn(data,chan+audioanOffset,this);
        }
        else
        {
            aOut(data,chan+audioanOffset, lastP.data());
        }
    }
    void mIn(const live::Event *data, live::ObjectChain* p)
    {
        live::Event b=*data;
        b.setChan(midian);
        p->push_back(this);
        if(live::cast<VstR*>(me)) live::cast<VstR*>(me)->me.mIn(&b,p);
        p->pop_back();
    }
};

#endif // VSTSIDEKICK_H
