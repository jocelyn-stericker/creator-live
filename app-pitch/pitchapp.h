/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef PITCH_H
#define PITCH_H

#include "live/object.h"

class PitchAppAudioR;

class PitchApp : public QObject, public live::Object
{
    Q_OBJECT
    PitchAppAudioR* s_audioR;
public:
    LIVE_HYBRID
    LIVE_EFFECT
    friend class AppSys;
    int s_stShift;  /*003*/
    int s_id;       /*004*/
    static int s_lastId;

public:
    RELOADABLE(PitchApp)
    PitchApp();
    ~PitchApp();

    const int& shiftAmount();

public slots:
    void shiftUp();
    void shiftDown();
    void setShift(const int& s);
    void aIn(const float *data, int chan, live::ObjectChain&p);
    void mIn(const live::Event *data, live::ObjectChain&p);
};

#endif // PITCH_H
