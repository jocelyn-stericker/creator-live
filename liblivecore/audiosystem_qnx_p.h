/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <QCoreApplication>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "live/object"
#include "live/audio"
#include "live/audiointerface"

#ifndef AUDIOSYSTEM_QNX_P_H
#define AUDIOSYSTEM_QNX_P_H

namespace live_private {

class AudioOut;
class SecretAudio;

class LIBLIVECORESHARED_EXPORT AudioIn : public QThread, public live::Object
{

   /****************************************************************
    * The implementation of QNX Audio has a completely different   *
    * thread system to jack's, DESPITE THE SIMILAR NAMES. AudioIn  *
    * and AudioOut are singletons and take care of themselves for  *
    * now. This WILL change if RIM makes USB OTG official.         *
    ****************************************************************/

public:
    LIVE_AUDIO
    LIVE_INPUT
    int     chans;
    AudioOut* master;

    AudioIn(AudioOut* cmaster) : live::Object("Microphone",true,false,2), chans(2), master(cmaster) { setTemporary(0); }


    void run(); // <== this function is all you care about.

    bool mOn() const{ return 0; } bool aOn() const { return 1; }
    virtual void aIn (const float*,int, live::Object*) {}
};

class LIBLIVECORESHARED_EXPORT AudioOut : public QThread, public live::Object
{

   /****************************************************************
    * The implementation of QNX Audio has a completely different   *
    * thread system to jack's, DESPITE THE SIMILAR NAMES. AudioIn  *
    * and AudioOut are singletons and take care of themselves for  *
    * now. This WILL change if RIM makes USB OTG official.         *
    ****************************************************************/

public:
    LIVE_AUDIO
    LIVE_OUTPUT
    bool mOn() const{ return 0; } bool aOn() const { return 1; }

    int     chans;
    short*  mSampleBfr2;
    QMutex* lock;
    QWaitCondition* dataRead;
    SecretAudio* master;

    AudioOut(SecretAudio* cmaster) : live::Object("Speaker or Headphones",true,true,2), chans(2), mSampleBfr2(new qint16[3072/sizeof(qint16)]), lock(new QMutex), dataRead(new QWaitCondition), master(cmaster) { setTemporary(0); }

    void run(); // <== this function processes data
    virtual void aIn(const float*data,int chan, live::Object*p); // <== that it got here.
};

class LIBLIVECORESHARED_EXPORT AudioNull : public live::Object
{
public:
    LIVE_AUDIO
    LIVE_INPUT
    bool mOn() const{ return 0; } bool aOn() const { return 1; }
    int     chans;

    AudioNull(int cchans) : live::Object("Null Audio Device",false,false,2), chans(cchans) { setTemporary(0); }

    virtual void aIn(const float*data,int chan, live::Object*p)
    {
        aOut(data,chan,this);
    }
};

class LIBLIVECORESHARED_EXPORT SecretAudio : public QObject, public live::AudioInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AudioInterface)
public:
    QString s_error;
    static SecretAudio* singleton;

    quint32 nframes;
    QList< AudioIn* > inputs;
    QList< live::ObjectPtr> solidIn;
    QList< AudioOut* > outputs;
    QList< live::ObjectPtr> solidOut;
    QList< AudioNull* > nulls;
    QList< live::ObjectPtr> solidNull;
    QList< QStringList > inputMappings;
    QStringList inputMappingsName;
    QList< QStringList > outputMappings;
    QStringList outputMappingsName;
//    jack_client_t* client;
    QList< int > asioDeviceTypes;
    bool valid() { return true; }

    QObject* qobject() { return this; }

    SecretAudio();

    virtual ~SecretAudio()
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

//    static int jackCallback( jack_nframes_t nframes, void* )
//    {
//        SecretAudio::nframes=nframes;
//        singleton->process();
//        return 0;
//    }

    static int xrunCallback( void * )
    {
        qDebug() << "XRUN!!!";
        return 0;
    }

    const quint32& nFrames() { return nframes; }
    qint32 sampleRate();

public slots:
    bool delClient();
    bool makeClient();
    bool refresh();
    void process();
    void jack_disconnect(QString readPort,QString writePort);

public:
    virtual QString name() { return "QNX Audio"; }
    virtual QString description() { return "Audio for the BlackBerry PlayBook and BlackBerry 10."; }

    virtual QObject* settingsWidget() { return 0; }

    virtual bool shouldDisplaySettingsWidget() { return 0; }

    virtual QString errorString() { QString l=s_error; s_error=""; return l; }

    virtual live::ObjectPtr getNull(int chans);

    virtual bool resetMappings();
    virtual bool addMapping(QStringList mapping, bool input,QString name);
    virtual int mappingCount(bool input);

    virtual QStringList getInputChanStringList();
    virtual QStringList getOutputChanStringList();
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

#endif // AUDIOSYSTEM_QNX_P_H
