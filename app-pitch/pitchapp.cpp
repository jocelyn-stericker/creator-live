/*****************************************************
Pitch.cpp                                  rev. 110720

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield  (trintith@gmail.com)

                 All rights reserved.
*****************************************************/

#include "pitchapp_p.h"
#include "live/midievent.h"
#include "live/audio.h"

using namespace live;

int PitchApp::s_lastId=-1;

PitchApp::PitchApp() :
    Object("PITCH", 0, 0),
    s_audioR(new PitchAppAudioR),
    s_stShift(0),
    s_id(++s_lastId)
{
    s_audioR->hybridConnect(this);
}

PitchApp::~PitchApp()
{
    s_audioR->hybridConnect(this);
    delete s_audioR;
}

const int& PitchApp::shiftAmount()
{
    return s_stShift;
}

void PitchApp::shiftUp()
{
    s_stShift++;
}
void PitchApp::shiftDown()
{
    s_stShift--;
}

void PitchApp::setShift(const int &s)
{
    s_stShift=s;
    s_audioR->setPitchShift(s);
}

void PitchApp::aIn(const float *data, int chan, ObjectChain&p)
{
    if(p.back()==s_audioR)
    {
        aOut(data,chan,p);
    }
    else
    {
        p.push_back(this);
        s_audioR->aIn(data,chan,p);
        p.pop_back();
    }
}

void PitchApp::mIn(const Event *data, ObjectChain&p)
{
    Event* nd = new Event;

    *nd = *data;
    nd->setNote((qint16)((data->simpleStatus()==Event::NOTE_ON)||(data->simpleStatus()==Event::NOTE_OFF)?data->note()+s_stShift:data->note()));
    p.push_back(this);
    mOut(nd,p);   // DIRECT CONNECTION!?
    p.pop_back();
    delete nd;
}

PitchAppAudioR::PitchAppAudioR() : Object("Soundtouch PitchApp implementation", 0, 0), s_soundTouch(new soundtouch::SoundTouch),
    s_latency(0), s_inCache(new float[audio::nFrames()*2]), s_outCache(new float[audio::nFrames()*2])
{
    for(int i=0;i<audio::nFrames()*2;i++) {
        s_inCache[i]=0.0f;
    }
    s_soundTouch->setSampleRate(audio::sampleRate());
    s_soundTouch->setChannels(2);
    s_soundTouch->setTempoChange(0);
    s_soundTouch->setPitchSemiTones(0);
    s_soundTouch->setRateChange(0);

    s_soundTouch->setSetting(SETTING_USE_QUICKSEEK,1);
    s_soundTouch->setSetting(SETTING_USE_AA_FILTER,0);
    s_shiftPitchAction=999;
}
PitchAppAudioR::~PitchAppAudioR()
{
    delete s_soundTouch;
    delete[] s_inCache;
    delete[] s_outCache;
}

float PitchAppAudioR::latency_msec()
{
    return (float)s_latency/((float)audio::sampleRate())*1000.0f;
}

void PitchAppAudioR::aIn(const float *data, int chan, ObjectChain&p)
{
    Q_ASSERT(chan<2);
    if(s_shiftPitchAction!=999)
    {
        int x=s_shiftPitchAction.fetchAndStoreOrdered(999);
        s_soundTouch->setPitchSemiTones(x);
    }

    float*proc=new float[audio::nFrames()];
    memcpy(proc,data,sizeof(float)*audio::nFrames());

    for(int i=0;i<audio::nFrames();i++)
    {
        s_inCache[2*i+chan]=data[i];
    }

    if(chan)
    {
        s_soundTouch->putSamples(s_inCache,audio::nFrames());
        if((int)s_soundTouch->numSamples()>=audio::nFrames()
                ) {
            s_soundTouch->receiveSamples(s_outCache,audio::nFrames());
            for(int i=0;i<audio::nFrames();i++)
            {
                proc[i]=s_outCache[2*i+1];
            }
        } else {
            for(int i=0;i<audio::nFrames();i++)
            {
                proc[i]=0.0;
            }
        }
    }
    else
    {
        for(int i=0;i<audio::nFrames();i++)
        {
            proc[i]=s_outCache[2*i];
        }
    }
    p.push_back(this);
    aOut(proc,chan,p);
    p.pop_back();
    delete[]proc;
}
void PitchAppAudioR::setPitchShift(int x) {
    s_shiftPitchAction=x;
}

