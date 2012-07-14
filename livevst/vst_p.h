/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#ifndef VST_RH
#define VST_RH

#ifdef _WIN32

#include "vst.h"
#include "live/object.h"
#include "live/midievent.h"
#include "vstsidekick.h"
#include <QWidget>
#include <QTimer>

class AEffect;
class VstEvents;
class VstR;

class VstEditor : public QWidget
{
    Q_OBJECT
private:
    AEffect* s_fx;
    QTimer* s_timer;
    VstR* s_parent;

public:
    VstEditor(AEffect*cfx,VstR* cparent) : QWidget(), s_fx(cfx), s_timer(new QTimer), s_parent(cparent)
    {
        init();
    }

    ~VstEditor()
    {
        unInit();
        delete s_timer;
    }

    void init();
    void unInit();
protected:
    void hideEvent(QHideEvent * event);

public slots:
    void timeEvent();
};

class VstR
{
public:
    Vst& me;
    AEffect* _vst;
    VstEditor* vstEditor;
    VstEvents* midiEvents;
    int midiEventCount;
    bool midiEventsProcessed;

    QList<live::Event*> midiQueue;
    QList<live::ObjectPtr> sourceQueue;

    QList<VstSidekick*> sideKicks;

    VstR(Vst* cme,AEffect* cvst) :
        me(*cme),
        _vst(cvst),
        vstEditor(0),
        midiEvents(0),
        midiEventCount(-1),
        midiEventsProcessed(1)
    {

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

    AEffect* s_loadPlugin(QString path);
    static bool isValid(QString path);
private:
    void s_resumePlugin(AEffect*plugin);
    void s_suspendPlugin(AEffect*plugin);
    void s_initPlugin(AEffect*&plugin);
    void s_getProperties(AEffect*plugin);
    bool s_canPluginDo(AEffect*plugin,char* canDoString);

};

#endif

#endif //VST_RH
