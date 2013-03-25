/*****************************************************
Pitch.cpp                                  rev. 110720

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield  (trintith@gmail.com)

                 All rights reserved.
*****************************************************/

#include "pitchapp_p.h"
#include <live/midievent>
#include <live/audio>

using namespace live;

int PitchApp:: m_lastId=-1;

PitchApp::PitchApp()
  : Object("PITCH", 0, 0, 2)
  ,  m_audioR(new PitchAppAudioR)
  ,  m_connections( m_audioR, this, live::HybridConnection)
  ,  m_stShift(0)
  ,  m_id(++ m_lastId)
{
}

PitchApp::~PitchApp()
{
    delete  m_audioR;
}

const int& PitchApp::shiftAmount()
{
    return  m_stShift;
}

void PitchApp::shiftUp()
{
     m_stShift++;
}
void PitchApp::shiftDown()
{
     m_stShift--;
}

void PitchApp::setShift(const int &s)
{
     m_stShift=s;
     m_audioR->setPitchShift(s);
}

void PitchApp::aIn(const float *data, int chan, Object*p)
{
    // if shift is 0, it's best to avoid running it through SoundTouch,
    // because that represents a degradation of sound quality.
    if (! m_stShift)
    {
        aOut(data, chan, p);
        return;
    }
    if (p== m_audioR)
    {
        aOut(data,chan,p);
    }
    else
    {
         m_audioR->aIn(data,chan,this);
    }
}

void PitchApp::mIn(const Event *data, ObjectChain*p)
{
    if (! m_mOn) {
        p->push_back(this);
        mOut(data,p);
        p->pop_back();
        return;
    }

    Event* nd = new Event;

    *nd = *data;
    nd->setNote((qint16)((data->simpleStatus()==Event::NOTE_ON)||(data->simpleStatus()==Event::NOTE_OFF)?data->note()+ m_stShift:data->note()));
    p->push_back(this);
    mOut(nd,p);   // DIRECT CONNECTION!?
    p->pop_back();
    delete nd;
}

PitchAppAudioR::PitchAppAudioR() : Object("SoundTouch PitchApp implementation", 0, 0, 2),  m_soundTouch(new soundtouch::SoundTouch),
     m_latency(0),  m_inCache(new float[audio::nFrames()*2]),  m_outCache(new float[audio::nFrames()*2])
{
    for (quint32 i=0;i<audio::nFrames()*2;i++) {
         m_inCache[i]=0.0f;
    }
     m_soundTouch->setSampleRate(audio::sampleRate());
     m_soundTouch->setChannels(2);
     m_soundTouch->setTempoChange(0);
     m_soundTouch->setPitchSemiTones(0);
     m_soundTouch->setRateChange(0);

     m_soundTouch->setSetting(SETTING_USE_QUICKSEEK, true);
     m_soundTouch->setSetting(SETTING_USE_AA_FILTER, false);
     m_soundTouch->setSetting(SETTING_SEQUENCE_MS, 10);
     m_shiftPitchAction=999;
}
PitchAppAudioR::~PitchAppAudioR()
{
    delete  m_soundTouch;
    delete[]  m_inCache;
    delete[]  m_outCache;
}

float PitchAppAudioR::latency_msec()
{
    return (float) m_latency/((float)audio::sampleRate())*1000.0f;
}

void PitchAppAudioR::aIn(const float *data, int chan, Object*p)
{
    Q_ASSERT(chan<2);

    if (! m_aOn) {
        aOut(data,chan,this);
        return;
    }

    if ( m_shiftPitchAction.fetchAndAddAcquire(0)!=999)
    {
        int x= m_shiftPitchAction.fetchAndStoreOrdered(999);
         m_soundTouch->setPitchSemiTones(x);
    }

    float*proc=new float[audio::nFrames()];
    memcpy(proc,data,sizeof(float)*audio::nFrames());

    for (quint32 i=0;i<audio::nFrames();i++)
    {
         m_inCache[2*i+chan]=data[i];
    }

    if (chan)
    {
         m_soundTouch->putSamples( m_inCache,audio::nFrames());
        if ((quint32) m_soundTouch->numSamples()>=audio::nFrames()
                ) {
             m_soundTouch->receiveSamples( m_outCache,audio::nFrames());
            for (quint32 i=0;i<audio::nFrames();i++)
            {
                proc[i]= m_outCache[2*i+1];
            }
        } else {
            for (quint32 i=0;i<audio::nFrames();i++)
            {
                proc[i]=0.0;
            }
        }
    }
    else
    {
        for (quint32 i=0;i<audio::nFrames();i++)
        {
            proc[i]= m_outCache[2*i];
        }
    }
    aOut(proc,chan,this);
    delete[]proc;
}
void PitchAppAudioR::setPitchShift(int x) {
     m_shiftPitchAction=x;
}

