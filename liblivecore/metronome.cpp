/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/metronome>
#include <cmath>
#include <math.h>

void live::Metronome::clock() {
    live::lthread::metronomeInit();
    if (s_isActive) {
        float time( float(startTime.msecsTo(QTime::currentTime())) );
        float bpm( (s_ts.denomenator==4) ? b_bpm : b_bpm*2.0f );
        float ppq( (s_ts.denomenator==4) ? s_ppq : s_ppq/2.0f );

        if (s_lastTime==-1) {
            float pbeat = 60000.0f/(float)bpm;
            s_lastTime = float(time);
            s_barStart = float(time) - pbeat;
        }

        if (time!=s_lastTime) {
            float pbeat=60000.0f/(float)bpm;
            float curBeat=(float)(time-s_barStart)/pbeat;
            while ((int)curBeat>s_ts.numerator) { // truncate
                s_barStart+=float(pbeat)*(float)s_ts.numerator;
                curBeat-=(float)s_ts.numerator;
            }

            float ppb = ((float)ppq*float(fmod(curBeat,1.0f)));

            if (s_lastPpq != ppb) {
                s_lastPpq=ppb;

                SyncData data(s_ts,
                              b_bpm,
                              s_ppq,

                              curBeat,
                              ppb
                             );

                for (int i=0;i<_as.size();i++) {
                    _as[i]->sync(data);
                }
            }
        }

        time=s_lastTime;
    }
}
