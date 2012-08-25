/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <live/audiotrack>
#include <live/miditrack>

class LooperApp;

class SequencerApp : public QObject, public live::Object
{
    Q_OBJECT
public:
    LIVE_HYBRID
    LIVE_EFFECT
    live::MidiTrack* s_midiTrack;                 /*003*/
    live::AudioTrack* s_audioTrack;               /*004*/
    LooperApp* s_cheat;                     /*N/A*/
        //null unless LooperApp
    live::MidiEventCounter* s_counter;            /*005*/
    live::Bound<bool> b_clipped;                  /*006*/
    int s_id;                               /*007*/ /*006 in looper*/
    bool s_audioOverdubForced;                   /*N/A*/
    int s_scale;                            /*008*/

    QMutex x_sequencer;

    SequencerApp(QString s_name="SEQUENCER",live::MidiTrack*cmidiTrack=new live::MidiTrack,live::AudioTrack*caudioTrack=new live::AudioTrack(2),
                 live::MidiEventCounter*cmidicounter=new live::MidiEventCounter(),bool newId=1);

    virtual ~SequencerApp();

    const bool& isRecord() const;
    const bool& isOverdub() const;
    const bool& isPlaying() const;
    virtual const bool& isMute() const;
    virtual int pos() const;
    bool clipped() const;

    const int& scale() const { return s_scale; }

    virtual QByteArray save();
    static live::ObjectPtr load(const QByteArray&str);

    QObject* qoThis() { return this;}

public slots:
    virtual void startRecord();
    void stopRecord();
    void startOverdub();
    void stopOverdub();
    virtual void startPlayback();
    void stopPlayback();
    virtual void startMute();
    virtual void stopMute();
    void setPos(int pos);
    virtual void aIn(const float *data, int chan, live::Object*p);
    void setClipped(bool clipped=1);
    inline void setFree()
    {
        setClipped(0);
    }

    void setScale(int);
    virtual void mIn(const live::Event *data, live::ObjectChain*p);
signals:
    void posSet(quint32 pos);
    void playbackStarted();
    void playbackStopped();
    void scaleChanged(int);
};

#endif // SEQUENCER_H
