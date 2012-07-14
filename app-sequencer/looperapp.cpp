/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "looperapp.h"

using namespace live;
//using namespace live_widgets;

int LooperApp::s_lastId=-1;

LooperApp::LooperApp(MidiTrack*mt,AudioTrack*at,MidiEventCounter*mc) :
    SequencerApp("LOOPER",mt=(mt?mt:new MidiTrack),at=(at?at:new AudioTrack(2)),mc=(mc?mc:new MidiEventCounter),0),
    b_loopMode(Off),
    b_loopLength(0),
    s_id_looper(++s_lastId)
{
    s_cheat=this;

    connect(&b_loopMode,SIGNAL(changeObserved(int,int)),this,SLOT(modeChanged(int)));
}

const bool& LooperApp::isMute() const
{
    return SequencerApp::isMute();
}

int LooperApp::pos() const
{
    return SequencerApp::pos();
}

void LooperApp::modeChanged(int now)
{
    NOSYNC;
    switch(now)
    {
    case Off:
        if(isPlaying())
            stopPlayback();

        break;
    case Playing:
        if(isPlaying())
            stopPlayback();

        setPos(0);

        if(isRecord())
            stopRecord();

        if(isOverdub())
            stopOverdub();

        if(!isPlaying())
            startPlayback();

        break;
    case Recording:
        b_loopLength=0;
    case Replacing:
//        s_audioTrack->clearData();
//        s_midiTrack->clearData();
        if(now==Replacing) {
            b_loopMode=Overdubbing;
            return;
        }

        if(now==Recording)
        {
            if(isPlaying())
                stopPlayback();

            setPos(0);
        }
    record:
        if(isOverdub())
            stopOverdub();

        if(!isRecord())
            startRecord();

        if(!isPlaying())
            startPlayback();

        break;
    case Multiplying:
        Q_ASSERT(0);    //don't feel like it right now.
    case Overdubbing:
        if(isRecord())
            stopRecord();

        if(!isOverdub())
            startOverdub();

        if(!isPlaying())
            startPlayback();

        break;
    case Substituting:
        goto record;
        break;
    }
}

void LooperApp::looperLogic()
{
    NOSYNC;
    if(isPlaying())
    {
        int curpos=pos();
        if(curpos>b_loopLength)
        {
            switch(b_loopMode.ref())
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

void LooperApp::aIn(const float *data, int chan, ObjectChain&p)
{
    looperLogic();
    if(p.back()==s_audioTrack)
    {
        SequencerApp::aIn(data,chan,p);
    }
    else
    {
        p.push_back(this);
        SequencerApp::aIn(data,chan,p);
        p.pop_back();
    }
}

void LooperApp::mIn(const Event *data, ObjectChain&p)
{
    if(p.back()==s_midiTrack)
    {
        SequencerApp::mIn(data,p);
        return;
    }
    else
    {
        p.push_back(this);
        SequencerApp::mIn(data,p);
        p.pop_back();
    }
}
