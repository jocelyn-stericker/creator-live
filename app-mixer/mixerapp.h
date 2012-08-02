/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef MIXERAPP_H
#define MIXERAPP_H

#include "live/object.h"
#include "live/variantbinding.h"

class MixerApp : public QObject, public live::Object
{
    Q_OBJECT
    static QList<MixerApp*> _u;
public:
    live::Bound<int> b_vol; /*003*/
    live::Bound<int> b_pan; /*004*/
    live::Bound<bool> b_mute; /*005*/
    live::Bound<bool> b_solo; /*006*/
    LIVE_HYBRID
    LIVE_EFFECT
    RELOADABLE(MixerApp)
    MixerApp();
    ~MixerApp();
    void aIn(const float *data, int chan, live::ObjectChain &p);
    void mIn(const live::Event *data, live::ObjectChain &p);
};

#endif // MIXERAPP_H