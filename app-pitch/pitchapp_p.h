/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef PITCHAPP_R_H
#define PITCHAPP_R_H

#ifndef BOOL
#define BOOL bool
#endif

#include "pitchapp.h"
#include <soundtouch/SoundTouch.h>
#include <QObject>

class PitchAppAudioR : public QObject, public live::Object {
    Q_OBJECT
    soundtouch::SoundTouch* s_soundTouch;
    int s_latency;
    float* s_inCache;
    float* s_outCache;
    QAtomicInt s_shiftPitchAction;
public:
    LIVE_HYBRID
    LIVE_EFFECT
    bool mOn() const{ return 0; }
    bool aOn() const { return 1; }
    PitchAppAudioR();
    ~PitchAppAudioR();
    float latency_msec();

public slots:
    void aIn(const float *data, int chan, live::ObjectChain*p);
    void setPitchShift(int x);
};

#endif // PITCHAPP_R_H
