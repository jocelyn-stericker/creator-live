/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

#ifndef VST_WIN_RH_
#define VST_WIN_RH_

#include "vst.h"
#include "live/object"
#include "live/midievent"
#include <QWidget>
#include <QTimer>

class AEffect;
class VstEvents;
class VstSidekick;
class VstR;

class VstEditor : public QWidget
{
    Q_OBJECT
private:
    AEffect*m_fx;
    QTimer*m_timer;
    VstR*m_parent;

public:
    VstEditor(AEffect*cfx,VstR* cparent) : QWidget(),m_fx(cfx),m_timer(new QTimer),m_parent(cparent)
    {
        init();
    }

    virtual ~VstEditor()
    {
        unInit();
        deletem_timer;
    }

    void init();
    void unInit();
protected:
    void hideEvent(QHideEvent * event);

public slots:
    void timeEvent();
};

class VstR : public live::Object
{
    LIVE_EFFECT
    LIVE_HYBRID
public:
    Vst& me;
    AEffect* _vst;
    VstEditor* vstEditor;
    VstEvents* midiEvents;
    int midiEventCount;
    bool midiEventsProcessed;

    QList<live::Event*> midiQueue;
    QList<live::ObjectPtr> sourceQueue;

    QList<VstSidekick*>m_sidekicks;

    VstR(Vst* cme,AEffect* cvst) :
        live::Object("VSTR", true, false, 2),
        me(*cme),
        _vst(cvst),
        vstEditor(0),
        midiEvents(0),
        midiEventCount(-1),
        midiEventsProcessed(1)
    {
        setTemporary(0);
    }

    static void silenceChannel( float **channelData, int numChannels, long numFrames );
    void processAudio( float **inputs, float **outputs, long numFrames );
};

class SecretVst
{
    friend class Vst;
protected:
    static SecretVst* singleton;
private:
    QList<VstR> _info;

public:
    SecretVst() : _info()
    {
        singleton=this;
    }

    void deleteVst(int vst);

    AEffect*m_loadPlugin(QString path);
    static bool isValid(QString path);
private:
    voidm_resumePlugin(AEffect*plugin);
    voidm_suspendPlugin(AEffect*plugin);
    voidm_initPlugin(AEffect*&plugin);
    voidm_getProperties(AEffect*plugin);
    boolm_canPluginDo(AEffect*plugin,char* canDoString);

};

#endif

#endif //VST_WIN_RH_
