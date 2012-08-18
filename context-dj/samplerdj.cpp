/*****************************************************
Sampler.cpp                                rev. 110731

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield  (trintith@gmail.com)

                 All rights reserved.
*****************************************************/

#include "samplerdj.h"
#include <live/midibinding>

using namespace live;

int SamplerDJ::s_lastId=-1;

SamplerDJ::SamplerDJ(MidiTrack**cmidiTracks,AudioTrack**caudioTracks,bool newId) :
    live::Object("SAMPLERDJ",0,0), s_bindingMode(-1), s_record(0), s_play(1), s_multi(1), s_id(newId?++s_lastId:-1)
{
    live_async for (int i=0;i<8;i++)
    {
        s_midiTracks[i]=(cmidiTracks&&cmidiTracks[i])?cmidiTracks[i]:new MidiTrack;
        if (s_midiTracks[i]->isPlay())
        {
            s_midiTracks[i]->stopPlayback();
        }
        if (s_midiTracks[i]->isRecord())
        {
            s_midiTracks[i]->stopRecord();
        }
        if (s_midiTracks[i]->isOverdub())
        {
            s_midiTracks[i]->stopOverdub();
        }
        if (s_midiTracks[i]->isMute())
        {
            s_midiTracks[i]->stopMute();
        }
        s_midiTracks[i]->_setThru(0);
        s_midiTracks[i]->hybridConnect(this);

        s_audioTracks[i]=(caudioTracks&&caudioTracks[i])?caudioTracks[i]:new AudioTrack(2);
        if (s_audioTracks[i]->isPlay())
        {
            s_audioTracks[i]->stopPlayback();
        }
        if (s_audioTracks[i]->isRecord())
        {
            s_audioTracks[i]->stopRecord();
        }
        if (s_audioTracks[i]->isOverdub())
        {
            s_audioTracks[i]->stopOverdub();
        }
        if (s_audioTracks[i]->isMute())
        {
            s_audioTracks[i]->stopMute();
        }
    }
}

SamplerDJ::~SamplerDJ()
{
    for (int i=0;i<8;i++)
    {
        delete s_midiTracks[i];
        delete s_audioTracks[i];
    }
}

const bool& SamplerDJ::isRecordMode()
{
    return s_record;
}

const bool& SamplerDJ::isPlayMode()
{
    return s_play;
}

const bool& SamplerDJ::isMultiMode()
{
    return s_multi;
}

void SamplerDJ::setRecordMode(bool r) {
    if (!r) return setPlayMode();
    if (s_record)
    {
        return;
    }

    live_async {
        for (int i=0;i<8;i++)
        {
            s_midiTracks[i]->startRecord();
            s_audioTracks[i]->startRecord();
        }
        s_play=0;
        s_record=1;
        emit recordModeSet(1);
        emit playModeSet(0);
    }
}

void SamplerDJ::setPlayMode() {
    if (s_play)
    {
        return;
    }

    live_async {
        for (int i=0;i<8;i++)
        {
            s_midiTracks[i]->stopRecord();
            s_audioTracks[i]->stopRecord();
        }

        s_play=1;
        s_record=0;
        emit recordModeSet(0);
        emit playModeSet(1);
    }
}

void SamplerDJ::setMultiMode(bool r) {
    if (!r) return unsetMultiMode();
    if (s_multi)
    {
        return;
    }

    live_async {
        s_multi=1;
        emit multiModeSet(1);
    }
}

void SamplerDJ::unsetMultiMode() {
    if (!s_multi)
    {
        return;
    }

    live_async {
        s_multi=0;
        emit multiModeSet(0);
    }
}

void SamplerDJ::hit(int button) {
    Q_ASSERT(button>=0&&button<=15);

    live_async {
        if (s_midiTracks[button]->isPlay())
        {
            s_midiTracks[button]->stopPlayback();
            s_audioTracks[button]->stopPlayback();
        }
        s_midiTracks[button]->setPos(0);
        s_audioTracks[button]->setPos(0);

        if (s_midiTracks[button]->isRecord())
        {
            s_midiTracks[button]->clearData();
            s_audioTracks[button]->clearData();
        }

        s_midiTracks[button]->startPlayback();
        s_audioTracks[button]->startPlayback();
    }
}

void SamplerDJ::release(int button) {
    Q_ASSERT(button>=0&&button<=15);

    live_async {
        if (s_midiTracks[button]->isPlay()) {
            s_midiTracks[button]->stopPlayback();
            s_audioTracks[button]->stopPlayback();
        }
    }
}

void SamplerDJ::setBindingMode(int b)
{
    s_bindingMode=b;
    if (s_bindingMode!=-1) {
        live_async MidiBinding::customNow=this;
    }
}

void SamplerDJ::aIn(const float *in, int chan, ObjectChain*p)
{
    const unsigned long& nframes=audio::nFrames();

    bool cpy_record=s_record;

    float* proc=!cpy_record?new float[nframes]:0;
    if (proc)
    {
        memcpy(proc,in,sizeof(float)*nframes);
    }

    if (cpy_record)    //[1]
    {
        for (int i=0;i<8;i++)
        {
            p->push_back(this);
            s_audioTracks[i]->aIn(in,chan,p);
            p->pop_back();
        }
    }
    else
    {
        for (int i=0;i<8;i++)
        {
            s_audioTracks[i]->aThru(proc,chan);
        }
    }

    p->push_back(this);
    aOut(proc?proc:in,chan,p);
    p->pop_back();
    delete[]proc;
}

void SamplerDJ::mIn(const Event *data, ObjectChain*p)
{
    if (!s_customBindings.value(p->first()))
    {
        s_customBindings.insert(p->first(),new int[200]);    //mem
        for (int i=0;i<200;i++)
        {
            s_customBindings.value(p->first())[i]=-1;
        }
    }
    for (int i=0;i<8;i++)
    {
        if (p->size()&&p->back()==s_midiTracks[i])
        {
            mOut(data,p);
            return;
        }
    }

    if ((s_bindingMode!=-1)&&data->simpleStatus()==Event::NOTE_ON&&data->velocity())
    {
        Q_ASSERT(!MidiBinding::customKey->value(p->first())[data->note()]);
        s_customBindings.value(p->first())[data->note()]=s_bindingMode;
        MidiBinding::customKey->value(p->first())[data->note()]=this;
        s_bindingMode=-1;
    }
    else if (data->simpleStatus()==Event::NOTE_ON&&data->velocity()&&(s_customBindings.value(p->first())[data->note()]!=-1))
    {
        hit(s_customBindings.value(p->first())[data->note()]);
    }
    else if ((data->simpleStatus()==Event::NOTE_OFF||data->simpleStatus()==Event::NOTE_ON)&&!data->velocity()&&
              s_customBindings.value(p->first())[data->note()]!=-1)
    {
        release(s_customBindings.value(p->first())[data->note()]);
    }
    else
    {
        p->push_back(this);
        for (int i=0;i<8;i++)
        {
            s_midiTracks[i]->mIn(data,p);
        }
        mOut(data,p);
        p->pop_back();
    }
}

