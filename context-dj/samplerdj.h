/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/audiotrack.h>
#include <live/miditrack.h>

#ifndef SAMPLERDJ_H
#define SAMPLERDJ_H

class SamplerDJ : public QObject, public live::Object
{
    Q_OBJECT
    static int s_lastId;
    live::MidiTrack* s_midiTracks[8];            /*003*/
    live::AudioTrack* s_audioTracks[8];          /*004*/
    QMap<live::ObjectPtr,int*> s_customBindings;  /*008*/
    int s_bindingMode;
    bool s_record;                          /*006*/
    bool s_play;                            /*^^^*/
    bool s_multi;                           /*007*/
    int s_id;                               /*005*/
    QMutex csMutex;
public:
    LIVE_HYBRID
    LIVE_EFFECT
    friend class SamplerFrame;
    SamplerDJ(live::MidiTrack**cmidiTracks=0,live::AudioTrack**caudioTracks=0,bool newId=1);

    virtual ~SamplerDJ();

    const bool& isRecordMode();
    const bool& isPlayMode();
    const bool& isMultiMode();

    // FIXME
//    virtual QByteArray save();
//    static live::ObjectPtr load(const QByteArray&str);

    QObject* qoThis() { return this; }

public slots:
    void setRecordMode(bool r=1);
    void setPlayMode();

    void setMultiMode(bool r=1);
    void unsetMultiMode();

    void hit(int button);   //button e (0,15)
    void release(int button);

    void setBindingMode(int);

    virtual void aIn(const float *data, int chan, live::ObjectChain&p);
    virtual void mIn(const live::Event *data, live::ObjectChain&p);

signals:
    void recordModeSet(bool);
    void playModeSet(bool);
    void multiModeSet(bool);
};

#endif // SAMPLERDJ_H
