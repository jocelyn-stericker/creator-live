/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( SF2 PLUGIN ) ) )

*******************************************************/

#ifndef SOUNDFONT_H
#define SOUNDFONT_H

#include "fluidsynth.h"
#include "fluidsynth/settings.h"

#include <live/object>

class Soundfont : public QObject, public live::Object
{
    Q_OBJECT
    fluid_settings_t* s_settings;
    fluid_synth_t* s_synth;
    float* s_cache[2];
    live::Connection s_connection;
public:
    LIVE_HYBRID
    LIVE_EFFECT
    Soundfont(QString url);

    void mIn(const live::Event* data, live::ObjectChain* p);
    void aIn(const float* data, int chan, Object *p);

    bool aOn() const { return false; }
    bool mOn() const { return true; }

public slots:
    void setProgram(int chan, int program);

signals:
    void programChanged(int chan, int program);
};

#endif // SOUNDFONT_H
