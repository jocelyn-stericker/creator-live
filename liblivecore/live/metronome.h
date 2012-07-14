/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef METRONOME_H
#define METRONOME_H

#include "live/object.h"
#include "live/timesignature.h"
#include "live/variantbinding.h"
#include <QTime>

namespace live {

// Metronome calls are from it's own thread, so you need to read the documentation
// for QObject::connect and Qt::connectionType. Usually you want to explictly set
// the connection type to direct.

class LIBLIVECORESHARED_EXPORT SyncData
{
public:
    const TimeSignature& ts;
    const int& bpm;
    const int& ppb;

    const int& beat;
    const int& pulses;

    SyncData(const TimeSignature&cts,const int&cbpm,const int&cppb,const int&cbeat,const int&cpulses) :
        ts(cts),
        bpm(cbpm),
        ppb(cppb),
        beat(cbeat),
        pulses(cpulses)
    {

    }
};

class LIBLIVECORESHARED_EXPORT AbstractSync {public: void* abst_t; AbstractSync(void* abstt) : abst_t(abstt) {} virtual void sync(const SyncData &)=0; virtual ~AbstractSync() {} };
template<class T> class LIBLIVECORESHARED_EXPORT ConcreteSync : public AbstractSync { public: ConcreteSync(T* at) : AbstractSync(at), t(at) {} T* t; void sync(const SyncData&s) { t->sync(s); } };

class LIBLIVECORESHARED_EXPORT Metronome : public QObject
{
    Q_OBJECT
private:
    QList<AbstractSync*> _as;
//    QMutex csMutex;
    QTime startTime;    //not saved
    TimeSignature s_ts;
public:
    Bound<int> b_bpm;
private:
    int s_ppq;
    long s_lastTime;    //not saved
    long s_barStart;    //not saved
    int s_lastPpq;
    bool s_isActive;

public:
    friend class SecretAudio;
    Metronome(TimeSignature cts,int cbpm,int cppq=96) : /*csMutex(QMutex::Recursive),*/ startTime(QTime::currentTime()), s_ts(cts), b_bpm((const int&)cbpm), s_ppq(cppq), s_lastTime(startTime.msecsTo(QTime::currentTime())), s_barStart(startTime.msecsTo(QTime::currentTime())), s_lastPpq(0),
        s_isActive(1)
    {
        pause();
    }

    ~Metronome()
    {
        qDebug()<<"DELMETR";
    }

    const TimeSignature& ts()
    {
        Q_ASSERT(this);
        return s_ts;
    }

    const int& bpm()
    {
        Q_ASSERT(this);
        return b_bpm;
    }

    const int& ppq()
    {
        Q_ASSERT(this);
        return s_ppq;
    }

    const bool& active()
    {
        Q_ASSERT(this);
        return s_isActive;
    }

    void clock();

    QByteArray save();
    static Metronome* load(const QByteArray&str);

    void setTs(const TimeSignature& cts)
    {
        Q_ASSERT(this);
        if(cts.denomenator!=4&&cts.denomenator!=8)
        {
            return;
        }
        Object::beginAsyncAction();
        s_ts=cts;
        Object::endAsyncAction();
    }

    void setBpm(const int& cbpm)
    {
        Q_ASSERT(this);
        Object::beginAsyncAction();
        b_bpm=cbpm;
        Object::endAsyncAction();
    }

    void setPpq(const int& cppq)
    {
        Q_ASSERT(this);
        Object::beginAsyncAction();
        s_ppq=cppq;
        Object::endAsyncAction();
    }

    void pause()
    {
        Q_ASSERT(this);
        Object::beginAsyncAction();
        Q_ASSERT(s_isActive);
        s_lastTime=( startTime.msecsTo(QTime::currentTime()) );

        s_barStart-=s_lastTime;
        s_isActive=0;
        Object::endAsyncAction();
    }

    void start()
    {
        Q_ASSERT(this);
        Object::beginAsyncAction();
        Q_ASSERT(!s_isActive);
        s_lastTime=( startTime.msecsTo(QTime::currentTime()) );
        s_barStart+=s_lastTime;
        //DEBUG
        s_lastPpq=-1;

        s_isActive=1;
        Q_ASSERT(b_bpm);
        Object::endAsyncAction();
        emit reactivated();
    }

    void pulse()
    {
        int time( startTime.msecsTo(QTime::currentTime()) );
        int bpm( (s_ts.denomenator==4) ? b_bpm : b_bpm*2 );
//        int ppq( (s_ts.denomenator==4) ? s_ppq : s_ppq/2 );
        double pbeat=60000.0/(double)bpm;

        double curBeat=qRound((double)(time-s_barStart)/pbeat);
        s_barStart=time-curBeat*pbeat;
    }

    template<class T> void registerSync(T* t)
    {
        Object::beginAsyncAction();
        _as.push_back(new ConcreteSync<T>(t));
        Object::endAsyncAction();
    }
    template<class T> void unregisterSync(T* t)
    {
        Object::beginAsyncAction();
        for(int i=0;i<_as.size();i++)
        {
            if(_as[i]->abst_t==t) {
                delete _as.takeAt(i);
                --i;
            }
        }
        Object::endAsyncAction();
    }
signals:
    void reactivated();
};

}

#endif // METRONOME_H
