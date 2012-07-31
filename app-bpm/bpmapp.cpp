/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "bpmapp.h"
#include "live/object.h"
#include "live/audio.h"
#include "live/songsystem.h"

#include <math.h>
#include <QTimer>
#include <cmath>
#include <math.h>

using namespace live;

int BPMApp::s_lastId=-1;

BPMApp::BPMApp() :
    Object("BPMDetect", 0, 0),
    _bpmAudio(),
    _bpmMidi(new AnaBeat()),
    s_id(++s_lastId),
    s_histogram(0),
    s_histoBoxes(-1),
    s_lastBPM(0.0f),
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
    delete[] s_histogram;
    while(_bpmAudio.size()) delete _bpmAudio.takeFirst();
    delete _bpmMidi;
}

const bool& BPMApp::isSynced()
{
    return b_sync;
}

void BPMApp::setSync(bool a)
{
    if(b_sync.ref()==a) {
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
    for(int i=0;i<_bpmAudio.size();i++) {
        if(_bpmTime[i].msecsTo(QTime::currentTime())<(aBPM?12000:4800)) continue;

        float bBPM=_bpmAudio[i]->getBpm();
        if(bBPM>20.0) aBPM=bBPM;
    }
    if(aBPM)
    {
        ret=aBPM;
    }
    else
    {
        float mBPM=(float)_bpmMidi->predictedBPM();
        ret=mBPM>0.0f?mBPM:0.0f;
    }
    float bestD=999999, bestP=-1.0;
    if(ret&&b_sync)
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
        for(int i=0;i<possibilities.size();i++) {
            if(qAbs(possibilities[i]-current)<bestD) {
                bestD=qAbs(possibilities[i]-current);
                bestP=possibilities[i];
            }
        }
//        qDebug()<<aBPM<<bestD<<bestP;
        ret=bestP;
    }

    if(ret&&b_sync&&(bestD<10.0f))
    {
        song::current()->metronome->setBpm((int)qRound(ret));
    }

    if(ret>20.0f) {
        s_lastBPM=ret;
    }
    return qRound(ret);
}

void BPMApp::aIn(const float *data, int chan, ObjectChain&p)
{
    const int& nframes=audio::nFrames();
    if(s_lastBPM>20.0f) {
        int boxes=qRound(1.0f/s_lastBPM*60.0f*(float)audio::sampleRate()/(float)(audio::nFrames()*8));
        int boxLength=1.0f/s_lastBPM*60.f*(float)audio::sampleRate()/boxes;

        if(s_histoBoxes!=boxes) {
            qDebug()<<"== RESET =="<<"(est:)"<<boxes;
            s_histoBoxes=boxes;
            delete[] s_histogram;
            s_histogram = new float[s_histoBoxes];
            for(int i=0;i<s_histoBoxes;i++) s_histogram[i]=0.0f;
            s_histoBox_i=s_histoBox_j=0;
            s_maxI_val=0.0f;
            s_maxI=s_maxI_prev=-1;
            s_boxLength=boxLength;
        } else s_boxLength=((float)s_boxLength)*0.75+((float)boxLength)*0.25;

//        qDebug()<< log(qAbs(data[0])*1000.0f);
        float val;
        for(int k=0;k<nframes;k++) {
            val=log(qAbs(data[k])*1000.0f);
            if(val>0) s_histogram[s_histoBox_i]+=val;
            if(++s_histoBox_j==s_boxLength) {
                if(++s_histoBox_i==s_histoBoxes) {
                    s_histoBox_i=0;
                    s_maxI=-1; s_maxI_val=0.0f;
                    for(int l=0;l<s_histoBoxes;l++) {
                        if(s_histogram[l]>s_maxI_val) {
                            s_maxI=l;
                            s_maxI_val=s_histogram[l];
                        }
                        s_histogram[l]*=(float)0.95;
                    }
//                    if(s_maxI!=-1) s_histogram[s_maxI]*=1.1f;
                }
                if(s_maxI==s_histoBox_i&&(s_maxI!=s_maxI_prev)) {
                    s_maxI_prev=s_maxI;
                    qDebug()<<"PULSE"<<s_histoBox_i;
                    song::current()->metronome->pulse();
                }
                s_histoBox_j=0;
            }
        }

//        for(int k=0;k<nframes;k++) {
//            s_histogram[s_histoBox_i]+=data[k];
//        }
//        if(s_histogram[s_histoBox_i]>s_maxI_val) {
//            s_maxI_val=s_histogram[s_histoBox_i];
//            s_maxI=s_histoBox_i;
//        }
//        if(++s_histoBox_j==6&&((s_histoBox_j=0),++s_histoBox_i==s_histoBoxes)) {
//            for(int i=0;i<s_histoBoxes;i++) {
//                s_histogram[i]*=0.8f;
//            }
//            s_maxI_val*=0.8f;
//            s_histoBox_i=0;
//            s_maxI_prev=s_maxI;
//        }
    }
    if(!chan)
    {
        for(int i=0;i<_bpmAudio.size();i++) {
            _bpmAudio[i]->inputSamples(data,audio::nFrames());
        }
        if(_bpmTime.back().msecsTo(QTime::currentTime())>4000) {
            _bpmAudio.push_back(new soundtouch::BPMDetect(1,audio::sampleRate()));
            _bpmTime.push_back(QTime::currentTime());
        }
        if(_bpmTime.first().msecsTo(QTime::currentTime())>21000) {
            delete _bpmAudio.takeFirst();
            _bpmTime.pop_front();
        }

    }

    p.push_back(this);
    aOut(data,chan,p);
    p.pop_back();
}

void BPMApp::mIn(const Event *data, ObjectChain&p)
{
    p.push_back(this);
    _bpmMidi->mIn(data,p);
    mOut(data,p);         //not implemented.
    p.pop_back();
}
