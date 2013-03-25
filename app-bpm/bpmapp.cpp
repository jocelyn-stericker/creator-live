/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "bpmapp.h"
#include <live/audio>
#include <live/object>
#include <live/songsystem>

#include <math.h>
#include <QTimer>
#include <cmath>
#include <math.h>

using namespace live;

int BPMApp::m_lastId=-1;

BPMApp::BPMApp() :
    Object("BPMDetect", 0, 0,2),
    _bpmAudio(),
    _bpmMidi(new AnaBeat()),
    m_id(++m_lastId),
    m_histogram(0),
    m_histoBoxes(-1),
    m_lastBPM(0.0f),
    b_sync(0)
{
    _bpmAudio.push_back(new soundtouch::BPMDetect(1,audio::sampleRate()));
    _bpmTime.push_back(QTime::currentTime());
    _bpmMidi->start();
    connect(&b_sync,SIGNAL(changeObserved(bool,bool)),this,SLOT(setSync(bool)));
    connect(song::current()->metronome,SIGNAL(reactivated()),this,SLOT(reprimeMetro()));
    QTimer* t=new QTimer(this);
    t->setInterval(10000);
    connect(t,SIGNAL(timeout()),this,SLOT(reprimeMetro()));
    t->start();
}

BPMApp::~BPMApp()
{
    delete[] m_histogram;
    while (_bpmAudio.size()) delete _bpmAudio.takeFirst();
    delete _bpmMidi;
}

const bool& BPMApp::isSynced()
{
    return b_sync;
}

void BPMApp::setSync(bool a)
{
    if (b_sync.ref()==a) {
        getBPM();
        return;
    }
    b_sync=a;
}

float BPMApp::getBPM()
{
    //prefer AudioBPM
    float ret(0.0);
    float aBPM=0;
    for (int i=0;i<_bpmAudio.size();i++) {
        if (_bpmTime[i].msecsTo(QTime::currentTime())<(aBPM?12000:4800)) continue;

        float bBPM=_bpmAudio[i]->getBpm();
        if (bBPM>20.0) aBPM=bBPM;
    }
    if (aBPM)
    {
        ret=aBPM;
    }
    else
    {
        float mBPM=(float)_bpmMidi->predictedBPM();
        ret=mBPM>0.0f?mBPM:0.0f;
    }
    float bestD=999999, bestP=-1.0;
    if (ret&&b_sync)
    {
        QList<float> possibilities;
        possibilities.push_back(ret);
        possibilities.push_back(ret/2.0);
        possibilities.push_back(ret*2.0);
        possibilities.push_back(ret/0.66667);
        possibilities.push_back(ret*0.66667);
        possibilities.push_back(ret/3);
        possibilities.push_back(ret*3);
        possibilities.push_back(ret/1.33333);
        possibilities.push_back(ret*1.33333);
        possibilities.push_back(ret/4.0);
        possibilities.push_back(ret*4.0);
        float current=song::current()->metronome->bpm();
        for (int i=0;i<possibilities.size();i++) {
            if (qAbs(possibilities[i]-current)<bestD) {
                bestD=qAbs(possibilities[i]-current);
                bestP=possibilities[i];
            }
        }
//        qDebug()<<aBPM<<bestD<<bestP;
        ret=bestP;
    }

    if (ret&&b_sync&&(bestD<10.0f))
    {
        song::current()->metronome->setBpm((int)qRound(ret));
    }

    if (ret>20.0f) {
        m_lastBPM=ret;
    }
    return qRound(ret);
}

void BPMApp::aIn(const float *data, int chan, Object*p)
{
    const quint32& nframes=audio::nFrames();
    if (m_lastBPM>20.0f) {
        int boxes=qRound(1.0f/m_lastBPM*60.0f*(float)audio::sampleRate()/(float)(audio::nFrames()*8));
        int boxLength=1.0f/m_lastBPM*60.f*(float)audio::sampleRate()/boxes;

        if (m_histoBoxes!=boxes) {
            qDebug() << "== RESET =="<<"(est:)"<<boxes;
            m_histoBoxes=boxes;
            delete[] m_histogram;
            m_histogram = new float[m_histoBoxes];
            for (int i=0;i<m_histoBoxes;i++) m_histogram[i]=0.0f;
            m_histoBox_i=m_histoBox_j=0;
            m_maxI_val=0.0f;
            m_maxI=m_maxI_prev=-1;
            m_boxLength=boxLength;
        } else m_boxLength=((float)m_boxLength)*0.75+((float)boxLength)*0.25;

//        qDebug()<< log(qAbs(data[0])*1000.0f);
        float val;
        for (unsigned k=0;k<nframes;k++) {
            val=log(qAbs(data[k])*1000.0f);
            if (val>0) m_histogram[m_histoBox_i]+=val;
            if (++m_histoBox_j==m_boxLength) {
                if (++m_histoBox_i==m_histoBoxes) {
                    m_histoBox_i=0;
                    m_maxI=-1; m_maxI_val=0.0f;
                    for (int l=0;l<m_histoBoxes;l++) {
                        if (m_histogram[l]>m_maxI_val) {
                            m_maxI=l;
                            m_maxI_val=m_histogram[l];
                        }
                        m_histogram[l]*=(float)0.95;
                    }
//                    if (m_maxI!=-1) m_histogram[m_maxI]*=1.1f;
                }
                if (m_maxI==m_histoBox_i&&(m_maxI!=m_maxI_prev)) {
                    m_maxI_prev=m_maxI;
                    qDebug() << "PULSE"<<m_histoBox_i;
                    song::current()->metronome->pulse();
                }
                m_histoBox_j=0;
            }
        }

//        for (int k=0;k<nframes;k++) {
//            m_histogram[m_histoBox_i]+=data[k];
//        }
//        if (m_histogram[m_histoBox_i]>m_maxI_val) {
//            m_maxI_val=m_histogram[m_histoBox_i];
//            m_maxI=m_histoBox_i;
//        }
//        if (++m_histoBox_j==6&&((m_histoBox_j=0),++m_histoBox_i==m_histoBoxes)) {
//            for (int i=0;i<m_histoBoxes;i++) {
//                m_histogram[i]*=0.8f;
//            }
//            m_maxI_val*=0.8f;
//            m_histoBox_i=0;
//            m_maxI_prev=m_maxI;
//        }
    }
    if (!chan)
    {
        for (int i=0;i<_bpmAudio.size();i++) {
            _bpmAudio[i]->inputSamples(data,audio::nFrames());
        }
        if (_bpmTime.back().msecsTo(QTime::currentTime())>4000) {
            _bpmAudio.push_back(new soundtouch::BPMDetect(1,audio::sampleRate()));
            _bpmTime.push_back(QTime::currentTime());
        }
        if (_bpmTime.first().msecsTo(QTime::currentTime())>21000) {
            delete _bpmAudio.takeFirst();
            _bpmTime.pop_front();
        }

    }

    aOut(data,chan,this);
}

void BPMApp::mIn(const Event *data, ObjectChain*p)
{
    p->push_back(this);
    _bpmMidi->mIn(data,p);
    mOut(data,p);         //not implemented.
    p->pop_back();
}
