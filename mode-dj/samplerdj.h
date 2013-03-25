/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/audiotrack>
#include <live/miditrack>
#include <live_widgets/bindableparent.h>

#ifndef SAMPLERDJ_H
#define SAMPLERDJ_H

class SamplerDJ : public QObject, public live::Object, public live_widgets::BindableParent
{
    Q_OBJECT
    static int m_lastId;
    live::MidiTrack* m_midiTracks[8];            /*003*/
    live::AudioTrack* m_audioTracks[8];          /*004*/
    QMap<live::ObjectPtr,int*> m_customBindings;  /*008*/
    int m_bindingMode;
    bool m_record;                          /*006*/
    bool m_play;                            /*^^^*/
    bool m_multi;                           /*007*/
    int m_id;                               /*005*/
    QList<live::Connection> m_connections;
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

    virtual void aIn(const float *data, int chan, Object *p);
    virtual void mIn(const live::Event *data, live::ObjectChain*p);

signals:
    void recordModeSet(bool);
    void playModeSet(bool);
    void multiModeSet(bool);
};

#endif // SAMPLERDJ_H
