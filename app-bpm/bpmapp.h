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
    int s_id;
    static int s_lastId;            /*003*/
    float* s_histogram;
    int s_histoBoxes;
    int s_histoBox_i;
    int s_histoBox_j;
    float s_lastBPM;
    float s_maxI_val;
    int s_maxI;
    int s_maxI_prev;
    int s_boxLength;
public:
    live::Bound<bool> b_sync;

    friend class BPMDetectFrame;

    BPMApp();
    ~BPMApp();

    const bool& isSynced();
public slots:
    void setSync(bool);
    void reprimeMetro() { s_maxI_prev=-1; }
public:
    float getBPM();

    virtual void aIn(const float *data, int chan, live::ObjectChain*p);
    virtual void mIn(const live::Event *data, live::ObjectChain*p);

public:
    QByteArray save();
    static BPMApp* load(const QByteArray&);
};

#endif // BPMDETECT_H
