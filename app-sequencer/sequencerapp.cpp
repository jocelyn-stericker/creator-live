/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "sequencerapp.h"
#include "sequencersys.h"

using namespace live;
//using namespace live_widgets;

SequencerApp::SequencerApp(QString name,MidiTrack*cmidiTrack,AudioTrack*caudioTrack,bool newId)
  : Object(name,false,false,2)
  , m_midiTrack(cmidiTrack)
  , m_midiTrackConnection(cmidiTrack,this, live::MidiConnection)
  , m_audioTrack(caudioTrack)
  , m_audioTrackConnection(caudioTrack,this, live::HybridConnection)
  , m_cheat(0)
  , b_clipped(0)
  , m_id(newId?SequencerSys::newIdForTrack():-1)
  , m_audioOverdubForced(0)
  , m_scale(2646000)
  , x_sequencer(QMutex::Recursive)
{
    Q_ASSERT(name=="SEQUENCER"||name=="LOOPER");    //awkward inheritance
}


SequencerApp::~SequencerApp()
{
    delete m_midiTrack;
    delete m_audioTrack;
}

const bool& SequencerApp::isRecord() const
{
    Q_ASSERT(m_midiTrack->isRecord()==m_audioTrack->isRecord());
    return m_midiTrack->isRecord();
}

const bool& SequencerApp::isOverdub() const
{
    /*if (!m_audioOverdubForced)*/ Q_ASSERT(m_midiTrack->isOverdub()==m_audioTrack->isOverdub());
    return m_midiTrack->isOverdub();
}

const bool& SequencerApp::isPlaying() const
{
    if (m_midiTrack->isPlay()!=m_audioTrack->isPlay())
    {
        qDebug() << "UH OH!!!";
        qDebug()<<m_midiTrack->isPlay()<<m_audioTrack->isPlay();
    }
    Q_ASSERT(m_midiTrack->isPlay()==m_audioTrack->isPlay());
    return m_midiTrack->isPlay();
}

const bool& SequencerApp::isMute() const
{
    Q_ASSERT(m_midiTrack->isMute()==m_audioTrack->isMute());
    return m_midiTrack->isMute();
}

qint64 SequencerApp::pos() const
{
//        Q_ASSERT(m_midiTrack->pos()==m_audioTrack->pos());
    return m_audioTrack->pos();
}

bool SequencerApp::clipped() const
{
    return b_clipped;
}

void SequencerApp::startRecord()
{
    live_mutex(x_sequencer) {
        m_midiTrack->startRecord();
        m_audioTrack->startRecord();
    }
}

void SequencerApp::stopRecord()
{
    live_mutex(x_sequencer) {
        m_midiTrack->stopRecord();
        m_audioTrack->stopRecord();
    }
}

void SequencerApp::startOverdub()
{
    live_mutex(x_sequencer) {
        m_midiTrack->startOverdub();
        m_audioTrack->startOverdub();
    }
}

void SequencerApp::stopOverdub()
{
    live_mutex(x_sequencer) {
        m_midiTrack->stopOverdub();
        m_audioTrack->stopOverdub();
    }
}

void SequencerApp::startPlayback()
{
    live_mutex(x_sequencer) {
        m_midiTrack->startPlayback();
        m_audioTrack->startPlayback();
    }
    emit playbackStarted();
}

void SequencerApp::stopPlayback()
{
    live_mutex(x_sequencer) {
        m_midiTrack->stopPlayback();
        m_audioTrack->stopPlayback();
    }
    emit playbackStopped();
}

void SequencerApp::startMute()
{
    live_mutex(x_sequencer) {
        m_midiTrack->startMute();
        m_audioTrack->startMute();
    }
}

void SequencerApp::stopMute()
{
    live_mutex(x_sequencer) {
        m_midiTrack->stopMute();
        m_audioTrack->stopMute();
    }
}

void SequencerApp::setPos(qint64 pos)
{
    Q_ASSERT(pos>=0);
    live_mutex(x_sequencer) {
        m_midiTrack->setPos(pos);
        m_audioTrack->setPos(pos);
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


void SequencerApp::aIn(const float *data, int chan, Object*p) {
    if (p==m_audioTrack) {
        aOut(data,chan,p);
    } else {
        m_audioTrack->aIn(data,chan,this);
    }
}

void SequencerApp::setScale(qint64 z)
{
    live_mutex(x_sequencer) {
        m_scale=z;
        emit scaleChanged(z);
    }
}

void SequencerApp::mIn(const Event *data, ObjectChain*p)
{
    if (p->back()==m_midiTrack)
    {
        mOut(data,p);
        return;
    }
    p->push_back(this);
    m_midiTrack->mIn(data,p);
    p->pop_back();
}

SequencerSys* SequencerSys::self=0;
