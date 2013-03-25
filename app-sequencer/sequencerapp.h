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
    live::MidiTrack* m_midiTrack;                 /*003*/
    live::Connection m_midiTrackConnection;
    live::AudioTrack* m_audioTrack;               /*004*/
    live::Connection m_audioTrackConnection;
    LooperApp* m_cheat;                     /*N/A*/
    live::Bound<bool> b_clipped;                  /*005*/
    qint64 m_id;                               /*006*/ /*007 in looper*/
    bool m_audioOverdubForced;                   /*N/A*/
    qint64 m_scale;                            /*007*/

    QMutex x_sequencer;

    SequencerApp(QString m_name="SEQUENCER", live::MidiTrack*cmidiTrack=new live::MidiTrack, live::AudioTrack*caudioTrack=new live::AudioTrack(2), bool newId=1);

    virtual ~SequencerApp();

    const bool& isRecord() const;
    const bool& isOverdub() const;
    const bool& isPlaying() const;
    virtual const bool& isMute() const;
    virtual qint64 pos() const;
    bool clipped() const;

    const qint64& scale() const { return m_scale; }

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
    void setPos(qint64 pos);
    virtual void aIn(const float *data, int chan, live::Object*p);
    void setClipped(bool clipped=1);
    inline void setFree()
    {
        setClipped(0);
    }

    void setScale(int z) { setScale(qint64(z)); }
    void setScale(qint64);
    virtual void mIn(const live::Event *data, live::ObjectChain*p);

    void importAudio(QString file) {
        m_audioTrack->importFile(file);
    }

    void importMidi(QString file) {
        m_midiTrack->importFile(file);
    }

signals:
    void posSet(qint64 pos);
    void playbackStarted();
    void playbackStopped();
    void scaleChanged(qint64);
};

#endif // SEQUENCER_H
