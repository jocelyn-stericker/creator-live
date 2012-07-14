/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include <QtConcurrentRun>
#include "live/object.h"
#include "live/audiosecond.h"
#include <QMutex>

#ifndef __QNX__
#include "sndfile.hh"
#endif

namespace live {

class LIBLIVECORESHARED_EXPORT AudioTrack : public QObject, public Object
{
    Q_OBJECT
public:
    LIVE_AUDIO
    LIVE_EFFECT
    bool aOn() const { return 1; }
    bool mOn() const{ return 0; }
    const int&nframes;
    AudioContainer** acquireData() {
        for(int i=0;i<s_chans;i++) {
            if(s_container[i]) s_container[i]->lock();
        }
        return s_container;
    }
    void releaseData() {
        for(int i=0;i<s_chans;i++) {
            if(s_container[i]) s_container[i]->unlock();
        }
    }

    QObject* qoThis() { return this; }

private:
    AudioContainer** s_container;
    qint32 s_chans;
    quint64 s_curPos;
    qint32 s_vol;          //s_vol e (0,100)
    qint32 s_pan;          //s_pan e (0,100) i.e., (L to R)
    bool s_record;
    bool s_overdub;
    bool s_playback;
    bool s_mute;
    QMutex csMutex;
    int s_simpleCount;
    int s_updateCounter;
    int s_boringCounter;

public:
    AudioTrack(int cchans);
    ~AudioTrack();
    const int& volume() const;
    const int& pan() const;
    const bool& isRecord() const;
    const bool& isOverdub() const;
    const bool& isPlay() const;
    const bool& isMute() const;
    float pos() const;

private:
    class AudioDataDestroyer {

    };

public slots:
    void setVol(int vol);
    void setPan(int pan);
    void startRecord();
    void stopRecord();
    void startOverdub();
    void stopOverdub();
    void startPlayback();
    void stopPlayback();
    void startMute();
    void stopMute();
    void setPos(long pos);
    void aIn(const float *in, int chan, ObjectChain&p);
    void aThru(float*proc,int chan);
    void clearData();
    void clearData(const float& a,const float& b);
    static int formatForString(QString s,bool verifyAvailable=0);
    static int rateForInt(int i);
    bool exportFile(QString filename,QString format="guess",int depth=24);
    bool importFile(QString filename);
    void async();
    QByteArray save();
    static AudioTrack* load(const QByteArray&str);

signals:
    void dataUpdated(int start=-1, int end=-1);
    void locationChanged(quint64);
};

}

#endif // AUDIOTRACK_H
