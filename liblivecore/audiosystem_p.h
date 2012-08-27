/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef AUDIOSYSTEM_P_H
#define AUDIOSYSTEM_P_H

#include <QCoreApplication>
#include "live/object"
#include "live/audio"
#include "live/audiointerface"

#ifndef __QNX__

#include <jack/jack.h>

namespace live_private {

LIBLIVECORESHARED_EXPORT jack_client_t* getJackClient();

// ONLY CALL AUDIO FUNCTIONS FROM THE AUDIO THREAD!
class LIBLIVECORESHARED_EXPORT AudioIn : public live::Object
{
public:
    LIVE_AUDIO
    LIVE_INPUT
    static int lastDeviceInternalID;
    int     chans;
    QList<jack_port_t*> s_port_;
    QStringList s_realnames;
    bool s_map;
    bool s_suspend;

public:
    bool mOn() const{ return 0; } bool aOn() const { return 1; }
    AudioIn(QStringList cnames, QString cname, bool cmap) : live::Object(cname,true,false), chans(cnames.size()), s_port_(), s_realnames(cnames),s_map(cmap),s_suspend(0)
    {
        setTemporary(0);

        for(int i=0; i<s_realnames; i++)
        {
            QString& a=s_realnames[i];
            if (a.indexOf(':')==-1)
            {
                if(a.indexOf('\"')!=-1)
                {
                    qDebug() << "Hathor does not support audio clients with quotes.";
                    Q_ASSERT(a.indexOf('\"')==-1);
                }
                Q_ASSERT(a.indexOf('_')!=-1);
                a[a.indexOf('_',a.contains("vst")?5:0)]=':';
            }
        }
        s_name.replace(":", "_");

        init();
    }

    void init(); //Audio.cpp
    void proc();

    virtual void suspend() { s_suspend=1; }
    virtual void resume() { s_suspend=0; }
    virtual void aIn (const float*,int, live::ObjectChain*) {}
};

// ONLY CALL AUDIO FUNCTIONS FROM THE AUDIO THREAD!
class LIBLIVECORESHARED_EXPORT AudioOut : public live::Object
{
public:
    LIVE_AUDIO
    LIVE_OUTPUT
    bool mOn() const{ return 0; } bool aOn() const { return 1; }
    int     chans;
    QList<jack_port_t*> s_port_[32];    //No more than 32 ports

    int     s_counter;
    int     s_i;
    QStringList s_realnames;
    bool    s_map;
    bool    s_tracked[16][32];  //16 ports and 32 chans max

public:
    AudioOut(QStringList cnames, QString name,bool cmap) : live::Object(name,true,true), chans(cnames.size()), s_counter(0), s_i(-1), s_realnames(cnames), s_map(cmap)
    {
        setTemporary(0);

        name.replace(":", "_");
        for(int i=0;i<16;i++)
        {
            for(int j=0;j<32;j++)
            {
                s_tracked[i][j]=0;
            }
        }

        initConnection();
    }
    virtual ~AudioOut() {
        destroyConnection();
    }

    virtual void aIn(const float*data,int chan, live::ObjectChain*p); //Audio.cpp

    void initConnection();
    void destroyConnection();
};

class LIBLIVECORESHARED_EXPORT AudioNull : public QObject, public live::Object
{
    Q_OBJECT
public:
    LIVE_AUDIO
    LIVE_INPUT
    bool mOn() const{ return 0; } bool aOn() const { return 1; }
    int     chans;
    QMutex p;

    AudioNull(int cchans)
      : live::Object("Null Audio Device",false,false)
      , chans(cchans)
      , p(QMutex::NonRecursive)
      {
    }

    virtual void aIn(const float*data,int chan, live::Object*)
    {
        aOut(data,chan,this);
    }

    QObject* qoThis() { return this; }
};

class LIBLIVECORESHARED_EXPORT SecretAudio : public QObject, public live::AudioInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AudioInterface)
public:
    static int XRUNS;
    QString s_error;
    static SecretAudio* singleton;
    static QMutex x_sa;

    QList< jack_port_t*> s_availInPorts;
    QList< QString > s_availInPortIds;

    QList< jack_port_t*> s_availOutPorts;
    QList< QString > s_availOutPortIds;

    jack_port_t* getInputPort() {
        Q_ASSERT(s_availInPorts.size());
        if (!s_availInPorts.size())
            return 0;
        return s_availInPorts.takeFirst();
    }
    QString getInputPortId() {
        Q_ASSERT(s_availInPortIds.size());
        if (!s_availInPortIds.size())
            return "NULL";
        return s_availInPortIds.takeFirst();
    }
    jack_port_t* getOutputPort() {
        Q_ASSERT(s_availOutPorts.size());
        if (!s_availOutPorts.size())
            return 0;
        return s_availOutPorts.takeFirst();
    }
    QString getOutputPortId() {
        Q_ASSERT(s_availOutPortIds.size());
        if (!s_availOutPortIds.size())
            return "NULL";
        return s_availOutPortIds.takeFirst();
    }

    bool s_paused;

    quint32 nframes;
    QList< AudioIn* > inputs;
    QList< AudioOut* > outputs;
    QList< AudioNull* > nulls;
    QList< QStringList > inputMappings;
    QStringList inputMappingsName;
    QList< QStringList > outputMappings;
    QStringList outputMappingsName;
    jack_client_t* client;
    QList< int > asioDeviceTypes;

    QObject* qobject() { return this; }

    SecretAudio();

    ~SecretAudio()
    {
        while (inputs.size())
        {
            delete inputs.takeFirst();
        }
        while (outputs.size())
        {
            delete outputs.takeFirst();
        }
        delClient();
    }

    static int jackCallback( jack_nframes_t nframes, void* )
    {
        if (SecretAudio::singleton->nframes != nframes)
            live::lthread::audioInit();

        SecretAudio::singleton->nframes=nframes;
        singleton->process();
        return 0;
    }

    static int xrunCallback( void * )
    {
        ++XRUNS;
        std::cerr << "xrun\n";
        live::Object::XRUN();
        return 0;
    }
    static void QUITCALLBACK(jack_status_t code, const char* reason, void *)
    {
        std::cerr << "CODE: " << code <<", reason: "<<reason<< std::endl;
//        TCRASH();
    }

    const quint32& nFrames() { return nframes; }
    qint32 sampleRate();

    bool valid() { return client; }

public slots:
    bool delClient();
    bool makeClient();
    bool refresh();
    void process();
    void jack_disconnect(QString readPort,QString writePort);

    void removeNull(QObject*);

    void pause() { kill_kitten s_paused = true; }
    void resume() { kill_kitten s_paused = false; }

public:
    virtual QString name() { return "Jack Audio"; }
    virtual QString description() { return "Jack is a multi-platform system for handling real-time low latency audio."; }

    virtual QObject* settingsWidget() { return 0; }

    virtual bool shouldDisplaySettingsWidget() { return 0; }

    virtual QString errorString() { QString l=s_error; s_error=""; return l; }

    virtual live::ObjectPtr getNull(int chans);

    virtual bool resetMappings();
    virtual bool addMapping(QStringList mapping, bool input,QString name);
    virtual int mappingCount(bool input);

    virtual QStringList getInputChanStringList();
    virtual QStringList getOutputChanStringList();

private:
    SecretAudio(const SecretAudio&);
    SecretAudio& operator=(const SecretAudio&);
};

class LIBLIVECORESHARED_EXPORT SecretAudioShutdownHandler : public QObject
{
    Q_OBJECT
public:
    static SecretAudioShutdownHandler *singleton;
public slots:
    void byeBye();
};

}
#else  // not __QNX__
#include "audiosystem_qnx_p.h"
#endif // __QNX__

#endif // AUDIOSYSTEM_P_H
