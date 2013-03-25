/*****************************************************
Sampler.cpp                                rev. 110731

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield  (trintith@gmail.com)

                 All rights reserved.
*****************************************************/

#include "samplerdj.h"
#include <live/midibinding>

using namespace live;

int SamplerDJ::m_lastId=-1;

SamplerDJ::SamplerDJ(MidiTrack**cmidiTracks,AudioTrack**caudioTracks,bool newId)
  : live::Object("SAMPLERDJ",0,0,2)
  , live_widgets::BindableParent(this)
  , m_bindingMode(-1)
  , m_record(0)
  , m_play(1)
  , m_multi(1)
  , m_id(newId?++m_lastId:-1)
{
    kill_kitten for (int i=0;i<8;i++)
    {
        m_midiTracks[i]=(cmidiTracks&&cmidiTracks[i])?cmidiTracks[i]:new MidiTrack;
        if (m_midiTracks[i]->isPlay())
        {
            m_midiTracks[i]->stopPlayback();
        }
        if (m_midiTracks[i]->isRecord())
        {
            m_midiTracks[i]->stopRecord();
        }
        if (m_midiTracks[i]->isOverdub())
        {
            m_midiTracks[i]->stopOverdub();
        }
        if (m_midiTracks[i]->isMute())
        {
            m_midiTracks[i]->stopMute();
        }
        m_midiTracks[i]->_setThru(0);
        m_connections.push_back(live::Connection(m_midiTracks[i],this,live::HybridConnection));

        m_audioTracks[i]=(caudioTracks&&caudioTracks[i])?caudioTracks[i]:new AudioTrack(2);
        if (m_audioTracks[i]->isPlay())
        {
            m_audioTracks[i]->stopPlayback();
        }
        if (m_audioTracks[i]->isRecord())
        {
            m_audioTracks[i]->stopRecord();
        }
        if (m_audioTracks[i]->isOverdub())
        {
            m_audioTracks[i]->stopOverdub();
        }
        if (m_audioTracks[i]->isMute())
        {
            m_audioTracks[i]->stopMute();
        }
    }
}

SamplerDJ::~SamplerDJ()
{
    for (int i=0;i<8;i++)
    {
        delete m_midiTracks[i];
        delete m_audioTracks[i];
    }
}

const bool& SamplerDJ::isRecordMode()
{
    return m_record;
}

const bool& SamplerDJ::isPlayMode()
{
    return m_play;
}

const bool& SamplerDJ::isMultiMode()
{
    return m_multi;
}

void SamplerDJ::setRecordMode(bool r) {
    if (!r) return setPlayMode();
    if (m_record)
    {
        return;
    }

    kill_kitten {
        for (int i=0;i<8;i++)
        {
            m_midiTracks[i]->startRecord();
            m_audioTracks[i]->startRecord();
        }
        m_play=0;
        m_record=1;
        emit recordModeSet(1);
        emit playModeSet(0);
    }
}

void SamplerDJ::setPlayMode() {
    if (m_play)
    {
        return;
    }

    kill_kitten {
        for (int i=0;i<8;i++)
        {
            m_midiTracks[i]->stopRecord();
            m_audioTracks[i]->stopRecord();
        }

        m_play=1;
        m_record=0;
        emit recordModeSet(0);
        emit playModeSet(1);
    }
}

void SamplerDJ::setMultiMode(bool r) {
    if (!r) return unsetMultiMode();
    if (m_multi)
    {
        return;
    }

    kill_kitten {
        m_multi=1;
        emit multiModeSet(1);
    }
}

void SamplerDJ::unsetMultiMode() {
    if (!m_multi)
    {
        return;
    }

    kill_kitten {
        m_multi=0;
        emit multiModeSet(0);
    }
}

void SamplerDJ::hit(int button) {
    Q_ASSERT(button>=0&&button<=15);

    kill_kitten {
        if (m_midiTracks[button]->isPlay())
        {
            m_midiTracks[button]->stopPlayback();
            m_audioTracks[button]->stopPlayback();
        }
        m_midiTracks[button]->setPos(0);
        m_audioTracks[button]->setPos(0);

        if (m_midiTracks[button]->isRecord())
        {
            m_midiTracks[button]->clearData();
            m_audioTracks[button]->clearData();
        }

        m_midiTracks[button]->startPlayback();
        m_audioTracks[button]->startPlayback();
    }
}

void SamplerDJ::release(int button) {
    Q_ASSERT(button>=0&&button<=15);

    kill_kitten {
        if (m_midiTracks[button]->isPlay()) {
            m_midiTracks[button]->stopPlayback();
            m_audioTracks[button]->stopPlayback();
        }
    }
}

void SamplerDJ::setBindingMode(int b)
{
    m_bindingMode=b;
    if (m_bindingMode!=-1) kill_kitten {
        if (!MidiBinding::customNow) MidiBinding::customNow = new ObjectPtr;
        *MidiBinding::customNow=this;
    }
}

void SamplerDJ::aIn(const float *in, int chan, Object*p)
{
    const quint32& nframes=audio::nFrames();

    bool cpy_record=m_record;

    float* proc=!cpy_record?new float[nframes]:0;
    if (proc)
    {
        memcpy(proc,in,sizeof(float)*nframes);
    }

    if (cpy_record)    //[1]
    {
        for (int i=0;i<8;i++)
        {
            m_audioTracks[i]->aIn(in,chan,this);
        }
    }
    else
    {
        for (int i=0;i<8;i++)
        {
            m_audioTracks[i]->aThru(proc,chan);
        }
    }

    aOut(proc?proc:in,chan,this);
    delete[]proc;
}

void SamplerDJ::mIn(const Event *data, ObjectChain*p)
{
    if (!m_customBindings.value(p->first()))
    {
        m_customBindings.insert(p->first(),new int[200]);    //mem
        for (int i=0;i<200;i++)
        {
            m_customBindings.value(p->first())[i]=-1;
        }
    }
    for (int i=0;i<8;i++)
    {
        if (p->size()&&p->back()==m_midiTracks[i])
        {
            mOut(data,p);
            return;
        }
    }

    if ((m_bindingMode!=-1)&&data->simpleStatus()==Event::NOTE_ON&&data->velocity())
    {
        Q_ASSERT(!MidiBinding::customKey->value(p->first())[data->note()]);
        m_customBindings.value(p->first())[data->note()]=m_bindingMode;
        MidiBinding::customKey->value(p->first())[data->note()]=this;
        m_bindingMode=-1;
    }
    else if (data->simpleStatus()==Event::NOTE_ON&&data->velocity()&&(m_customBindings.value(p->first())[data->note()]!=-1))
    {
        hit(m_customBindings.value(p->first())[data->note()]);
    }
    else if ((data->simpleStatus()==Event::NOTE_OFF||data->simpleStatus()==Event::NOTE_ON)&&!data->velocity()&&
              m_customBindings.value(p->first())[data->note()]!=-1)
    {
        release(m_customBindings.value(p->first())[data->note()]);
    }
    else
    {
        p->push_back(this);
        for (int i=0;i<8;i++)
        {
            m_midiTracks[i]->mIn(data,p);
        }
        mOut(data,p);
        p->pop_back();
    }
}

