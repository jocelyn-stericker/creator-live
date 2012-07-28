/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef AUDIOSECOND_H
#define AUDIOSECOND_H

#include "live/audio.h"
#include "live/object.h"
#include <QReadWriteLock>
#include <QtConcurrentRun>

namespace live {

class LIBLIVECORESHARED_EXPORT AudioSecond
{
       /*SAMPLE RATE->VERIFY->003*/
    quint64 s_sampleOffset;     /*004*/
    float* s_data;          /*005*/
public:
    friend class AudioContainer;
    AudioSecond(quint64 cbegin) : s_sampleOffset(cbegin), s_data(new float[live::audio::sampleRate()])
    {
        for(int i=0;i<live::audio::sampleRate();i++)
        {
            s_data[i]=0.0f;
        }
    }
    ~AudioSecond() { delete[]s_data; }

    float& operator[](const int&sample)
    {
        return s_data[sample-s_sampleOffset];
    }

    static AudioSecond* load(const QByteArray&);
    QByteArray save();
};

class LIBLIVECORESHARED_EXPORT AudioContainer
{
protected:
    float zero;                 /*N/A*/
    quint64 sampleRate;     /*VERIFY->003*/
    QList<AudioSecond*> s_data; /*004*/
    quint64 s_dataSize;             /*004*/
    QReadWriteLock s_readLock;          /*N/A*/
public:
    friend class AudioTrack;
    AudioContainer() : zero(0.0f),sampleRate(live::audio::sampleRate()),s_dataSize(0) {}

    ~AudioContainer()
    {
        clear(1);
        s_dataSize=0;
    }

    void setData(const quint64&frame,const float&data)
    {
        if(data==0.0f)
        {
            return;
        }

        for(quint64 i=s_data.size()-1;i<=frame/sampleRate;i++)
        {
            s_data.push_back(0);
            s_dataSize=s_data.size();
        }

        if(!s_data[(int)(frame/sampleRate)])
        {
            s_data[(int)(frame/sampleRate)]=new AudioSecond((frame/sampleRate)*sampleRate);
        }

        (*s_data[(int)(frame/sampleRate)])[(int)frame]=data;
    }

    const float& getData(const int&frame)
    {
        if(s_dataSize<=frame/sampleRate||!s_data[(int)(frame/sampleRate)])
        {
            return zero;
        }
        else
        {
            return (*s_data[(int)(frame/sampleRate)])[frame];
        }
    }

    //The return is the count of data returned through "data". There may be more
    //if called for the next frame.
    //By default, if no data exists(i.e., it is all null), data will be returned as
    //a null pointer. If instead, data should be created, forceCreation should be true.
    int getRawPointer(const quint64&frame,float*& data,bool forceCreation=0)
    {
        sampleRate=audio::sampleRate();
        if((s_dataSize<=frame/sampleRate||!s_data[frame/sampleRate])&&!forceCreation)
        {
            data=0;
            return (int)((frame/sampleRate)*sampleRate+sampleRate-frame);
        }
        else if(s_dataSize<=frame/sampleRate)
        {
            for(long i=s_data.size()-1;i<=(long)frame/(long)sampleRate;i++)
            {
                s_data.push_back(0);
                s_dataSize=s_data.size();
            }
        }

        if(!s_data[frame/sampleRate])
        {
            s_data[(int)(frame/sampleRate)]=new AudioSecond((frame/sampleRate)*sampleRate);
        }

        int id=(int)(frame/sampleRate);
        data=&s_data[id]->s_data[frame-s_data[id]->s_sampleOffset];
        return (int)(s_data[id]->s_sampleOffset+sampleRate-frame);
    }

    class AudioDataDestroyer {
    public:
        AudioSecond* s_s;
        QReadWriteLock* s_l;
        AudioDataDestroyer(AudioSecond* s, QReadWriteLock* l) : s_s(s), s_l(l) {}
        void run() { if(s_l) s_l->lockForWrite(); delete s_s; if(s_l) s_l->unlock(); }
    };

    void clear(bool dying=0)
    {
        if(!s_readLock.tryLockForWrite()&&!dying) {
            QtConcurrent::run(this,&AudioContainer::clear,0);
            return;
        }
        s_dataSize=0;
        while(s_data.size())
        {
            AudioDataDestroyer* add = new AudioDataDestroyer(s_data.takeFirst(),(dying?0:&s_readLock));
            QtConcurrent::run(add,&AudioDataDestroyer::run);
        }
        if(!dying) {
            s_readLock.unlock();
        }
    }

    float length() const
    {
        float ret = s_data.size();
        int i;
        for(i = audio::sampleRate() - 1;i >= 0;++i) {
            if (s_data.back()->s_data[i]!=0.0f) break;
        }
        ret -= float(audio::sampleRate() - i)/float(audio::sampleRate());
        return ret;
    }

    void lock() {
        s_readLock.lockForRead();
    }

    void unlock() {
        s_readLock.unlock();
    }

    static AudioContainer* load(const QByteArray&);
    QByteArray save();
};

}

#endif // AUDIOSECOND_H