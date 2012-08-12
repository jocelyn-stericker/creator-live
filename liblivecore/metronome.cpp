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
        int time( startTime.msecsTo(QTime::currentTime()) );
        int bpm( (s_ts.denomenator==4) ? b_bpm : b_bpm*2 );
        int ppq( (s_ts.denomenator==4) ? s_ppq : s_ppq/2 );

        if (s_lastTime==-1) {
            double pbeat=60000.0/(double)bpm;
            s_lastTime=time;
            s_barStart=time-pbeat;
        }

        if (time!=s_lastTime) {
            double pbeat=60000.0/(double)bpm;
            double curBeat=(double)(time-s_barStart)/pbeat;
            while ((int)curBeat>s_ts.numerator) { // truncate
                s_barStart+=pbeat*(double)s_ts.numerator;
                curBeat-=(double)s_ts.numerator;
            }

            int ppb = ((double)ppq*fmod(curBeat,1.0));

            if (s_lastPpq != ppb) {
                s_lastPpq=ppb;

                SyncData data(s_ts,
                              b_bpm,
                              s_ppq,

                              (int)curBeat,   //truncate
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
