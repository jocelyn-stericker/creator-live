/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "looperapp.h"

using namespace live;
//using namespace live_widgets;

int LooperApp::m_lastId=-1;

LooperApp::LooperApp(MidiTrack*mt,AudioTrack*at) :
    SequencerApp("LOOPER",mt=(mt?mt:new MidiTrack),at=(at?at:new AudioTrack(2)),0),
    b_loopMode(Off),
    b_loopLength(0),
    m_id_looper(++m_lastId)
{
    m_cheat=this;

    connect(&b_loopMode,SIGNAL(changeObserved(qint64,qint64)),this,SLOT(modeChanged(qint64)));
}

const bool& LooperApp::isMute() const
{
    return SequencerApp::isMute();
}

qint64 LooperApp::pos() const
{
    return SequencerApp::pos();
}

void LooperApp::modeChanged(int now)
{
    live_mutex(x_sequencer) switch (now) {
    case Off:
        if (isPlaying())
            stopPlayback();

        break;
    case Playing:
        if (isPlaying())
            stopPlayback();

        setPos(0);

        if (isRecord())
            stopRecord();

        if (isOverdub())
            stopOverdub();

        if (!isPlaying())
            startPlayback();

        break;
    case Recording:
        b_loopLength=0;
    case Replacing:
        m_audioTrack->clearData();
        m_midiTrack->clearData();
        if (now==Replacing) {
            b_loopMode=Overdubbing;
            return;
        }

        if (now==Recording)
        {
            if (isPlaying())
                stopPlayback();

            setPos(0);
        }
    record:
        if (isOverdub())
            stopOverdub();

        if (!isRecord())
            startRecord();

        if (!isPlaying())
            startPlayback();

        break;
    case Multiplying:
        Q_ASSERT(0);    //don't feel like it right now.
    case Overdubbing:
        if (isRecord())
            stopRecord();

        if (!isOverdub())
            startOverdub();

        if (!isPlaying())
            startPlayback();

        break;
    case Substituting:
        goto record;
        break;
    }
}

void LooperApp::looperLogic()
{
    live_mutex(x_sequencer) if (isPlaying())
    {
        qint64 curpos=pos();
        if (curpos>b_loopLength)
        {
            switch (b_loopMode.ref())
            {
            case Off:
                Q_ASSERT(0);
                break;
            case Replacing:
            case Substituting:
                b_loopMode=Playing;
            case Playing:
            case Overdubbing:
                    stopPlayback();
                    setPos(0);
                    startPlayback();
                break;
            case Multiplying:
                Q_ASSERT(0);    //don't feel like it right now.
            case Recording:
                b_loopLength=curpos;
                break;
            }
        }
    }
}

void LooperApp::aIn(const float *data, int chan, Object*p)
{
    qint64 curpos=pos();
    if (curpos>b_loopLength)
        QMetaObject::invokeMethod(this, "looperLogic", Qt::QueuedConnection);
//    looperLogic();
    if (p==m_audioTrack)
    {
        SequencerApp::aIn(data,chan,p);
    }
    else
    {
        SequencerApp::aIn(data,chan,this);
    }
}

void LooperApp::mIn(const Event *data, ObjectChain*p)
{
    if (p->back()==m_midiTrack)
    {
        SequencerApp::mIn(data,p);
        return;
    }
    else
    {
        p->push_back(this);
        SequencerApp::mIn(data,p);
        p->pop_back();
    }
}
