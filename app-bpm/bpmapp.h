/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef BPMDETECT_H
#define BPMDETECT_H

#include <live/object>
#include <live/variantbinding>
#include <live/anabeat>
#include <BPMDetect.h>
#include <QList>

class BPMApp : public QObject, public live::Object
{
    Q_OBJECT
    LIVE_HYBRID
    LIVE_EFFECT

    QList<QTime> _bpmTime;
    QList<soundtouch::BPMDetect*> _bpmAudio;
    live::AnaBeat* _bpmMidi;
    int m_id;
    static int m_lastId;            /*003*/
    float* m_histogram;
    int m_histoBoxes;
    int m_histoBox_i;
    int m_histoBox_j;
    float m_lastBPM;
    float m_maxI_val;
    int m_maxI;
    int m_maxI_prev;
    int m_boxLength;
public:
    live::Bound<bool> b_sync;

    friend class BPMDetectFrame;

    BPMApp();
    ~BPMApp();

    const bool& isSynced();
public slots:
    void setSync(bool);
    void reprimeMetro() { m_maxI_prev=-1; }
public:
    float getBPM();

    virtual void aIn(const float *data, int chan, Object *p);
    virtual void mIn(const live::Event *data, live::ObjectChain*p);

public:
    QByteArray save();
    static BPMApp* load(const QByteArray&);
};

#endif // BPMDETECT_H
