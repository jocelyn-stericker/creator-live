/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "sequencerapp.h"
#include "sequencersys.h"

using namespace live;
//using namespace live_widgets;

SequencerApp::SequencerApp(QString name,MidiTrack*cmidiTrack,AudioTrack*caudioTrack,MidiEventCounter*cmidicounter,bool newId) :
    Object(name,false,false),
    s_midiTrack(cmidiTrack),
    s_audioTrack(caudioTrack),
    s_cheat(0),
    s_counter(cmidicounter),
    b_clipped(0),
    s_id(newId?SequencerSys::newIdForTrack():-1),
    s_audioOverdubForced(0),
    s_scale(2646000)
{
    Q_ASSERT(name=="SEQUENCER"||name=="LOOPER");    //awkward inheritance
    s_midiTrack->hybridConnect(this);
    s_counter->hybridConnect(s_midiTrack);

    s_audioTrack->hybridConnect(this);
}


SequencerApp::~SequencerApp()
{
    qDebug() << "!!DEL";
    delete s_midiTrack;
    delete s_audioTrack;
    delete s_counter;
}

const bool& SequencerApp::isRecord() const
{
    Q_ASSERT(s_midiTrack->isRecord()==s_audioTrack->isRecord());
    return s_midiTrack->isRecord();
}

const bool& SequencerApp::isOverdub() const
{
    /*if (!s_audioOverdubForced)*/ Q_ASSERT(s_midiTrack->isOverdub()==s_audioTrack->isOverdub());
    return s_midiTrack->isOverdub();
}

const bool& SequencerApp::isPlaying() const
{
    if (s_midiTrack->isPlay()!=s_audioTrack->isPlay())
    {
        qDebug() << "UH OH!!!";
        qDebug()<<s_midiTrack->isPlay()<<s_audioTrack->isPlay();
    }
    Q_ASSERT(s_midiTrack->isPlay()==s_audioTrack->isPlay());
    return s_midiTrack->isPlay();
}

const bool& SequencerApp::isMute() const
{
    Q_ASSERT(s_midiTrack->isMute()==s_audioTrack->isMute());
    return s_midiTrack->isMute();
}

int SequencerApp::pos() const
{
//        Q_ASSERT(s_midiTrack->pos()==s_audioTrack->pos());
    return s_audioTrack->pos();
}

bool SequencerApp::clipped() const
{
    return b_clipped;
}

void SequencerApp::startRecord()
{
    live_mutex(x_sequencer) {
        s_midiTrack->startRecord();
        s_audioTrack->startRecord();
    }
}

void SequencerApp::stopRecord()
{
    live_mutex(x_sequencer) {
        s_midiTrack->stopRecord();
        s_audioTrack->stopRecord();
    }
}

void SequencerApp::startOverdub()
{
    live_mutex(x_sequencer) {
        s_midiTrack->startOverdub();
        s_audioTrack->startOverdub();
    }
}

void SequencerApp::stopOverdub()
{
    live_mutex(x_sequencer) {
        s_midiTrack->stopOverdub();
        s_audioTrack->stopOverdub();
    }
}

void SequencerApp::startPlayback()
{
    live_mutex(x_sequencer) {
        s_midiTrack->startPlayback();
        s_audioTrack->startPlayback();
    }
    emit playbackStarted();
}

void SequencerApp::stopPlayback()
{
    live_mutex(x_sequencer) {
        s_midiTrack->stopPlayback();
        s_audioTrack->stopPlayback();
    }
    emit playbackStopped();
}

void SequencerApp::startMute()
{
    live_mutex(x_sequencer) {
        s_midiTrack->startMute();
        s_audioTrack->startMute();
    }
}

void SequencerApp::stopMute()
{
    live_mutex(x_sequencer) {
        s_midiTrack->stopMute();
        s_audioTrack->stopMute();
    }
}

void SequencerApp::setPos(int pos)
{
    live_mutex(x_sequencer) {
        s_midiTrack->setPos(pos);
        s_audioTrack->setPos(pos);
    }
    emit posSet(pos);
}

void SequencerApp::setClipped(bool clipped)
{
    live_mutex(x_sequencer) {
        if (clipped&&!b_clipped)
            SequencerSys::registerClippedSeq(this);
        else if (b_clipped&&!clipped)
            SequencerSys::deregisterClippedSeq(this);

        b_clipped=clipped;
    }
}


void SequencerApp::aIn(const float *data, int chan, ObjectChain*p) {
    if (p->back()==s_audioTrack) {  // FIXME: eliminate ObjectChain
        aOut(data,chan,p);
    } else {
        p->push_back(this);
        s_audioTrack->aIn(data,chan,p);
        p->pop_back();
    }
}

void SequencerApp::setScale(int z)
{
    live_mutex(x_sequencer) {
        s_scale=z;
        emit scaleChanged(z);
    }
}

void SequencerApp::mIn(const Event *data, ObjectChain*p)
{
    if (p->back()==s_midiTrack)
    {
        mOut(data,p);
        return;
    }
    p->push_back(this);
    s_counter->mIn(data,p);
    s_midiTrack->mIn(data,p);
    p->pop_back();
}

SequencerSys* SequencerSys::self=0;
