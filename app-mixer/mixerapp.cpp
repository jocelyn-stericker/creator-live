/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "mixerapp.h"

#include <live/audio>
#include <live/midievent>

using namespace live;

QList<MixerApp*> MixerApp::_u;

MixerApp::MixerApp() : Object("Mixer", 0, 0), b_vol(100), b_pan(50), b_mute(0), b_solo(0)
{
    for (int i = 0; i < 2; ++i)
        s_period[i] = new float[audio::nFrames()];
    _u.push_back(this);
}

MixerApp::~MixerApp()
{
    for (int i = 0; i < 2; ++i)
        delete[] s_period[i];
    _u.removeOne(this);
}

void MixerApp::aIn(const float *data, int chan, ObjectChain* p)
{
    bool zero=0;
    if (!b_solo)
    {
        for (int i=0;i<_u.size();i++)
        {
            if (_u[i]->b_solo.ref())
            {
                zero = 1;
            }
        }
    }

    if (b_mute)
    {
        zero = 1;
    }

    if (b_pan.ref()==50&&b_vol.ref()==100)
    {
        aOut(data,chan,this);
        return;
    }
    const quint32& nframes=audio::nFrames();
    float* dx = s_period[chan];
    for (unsigned i=0;i<nframes;i++)
    {
        if (zero)
        {
            dx[i] = 0.0f;
        }
        else
        {
            float panamount;
            if (chan)
            {
                panamount = b_pan;
            }
            else
            {
                panamount = 100.0f - (float)b_pan;
            }
            dx[i] = data[i] * ((float)b_vol) / 100.0f * (panamount / 100.0f);  // FIXME: eventually consider proper pan.
            if (dx[i]>1.0f)
            {
                b_vol=(float)b_vol*1.0f/dx[i];
            }
        }
    }

    aOut(dx,chan,this);
}

void MixerApp::mIn(const Event *data, ObjectChain* p)
{
    if (!b_solo)
    {
        for (int i=0;i<_u.size();i++)
        {
            if (_u[i]->b_solo.ref())
            {
                return;
            }
        }
    }
    else if (b_mute)
    {
        return;
    }

    Event x=*data;
    if (x.simpleStatus()==0x90)
    {
        x.setVelocity((float)x.velocity()*(float)b_vol/100.0f);
    }
    p->push_back(this);
    mOut(&x,p);

    if (b_pan!=50)
    {
        x.setSimpleStatus(0xB0);
        x.data1=10;
        x.data2=(float)b_pan/100.0f*127.0f;
        mOut(&x,p);
    }
    p->pop_back();
}
