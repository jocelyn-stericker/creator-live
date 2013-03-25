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
    if (m_isActive) {
        float time( float(startTime.msecsTo(QTime::currentTime())) );
        float bpm( (m_ts.denomenator==4) ? b_bpm : b_bpm*2.0f );
        float ppq( (m_ts.denomenator==4) ? m_ppq : m_ppq/2.0f );

        if (m_lastTime==-1) {
            float pbeat = 60000.0f/(float)bpm;
            m_lastTime = float(time);
            m_barStart = float(time) - pbeat;
        }

        if (time!=m_lastTime) {
            float pbeat=60000.0f/(float)bpm;
            float curBeat=(float)(time-m_barStart)/pbeat;
            while ((int)curBeat>m_ts.numerator) { // truncate
                m_barStart+=float(pbeat)*(float)m_ts.numerator;
                curBeat-=(float)m_ts.numerator;
            }

            float ppb = ((float)ppq*float(fmod(curBeat,1.0f)));

            if (m_lastPpq != ppb) {
                m_lastPpq=ppb;

                SyncData data(m_ts,
                              b_bpm,
                              m_ppq,

                              curBeat,
                              ppb
                             );

                for (int i=0;i<_as.size();i++) {
                    _as[i]->sync(data);
                }
            }
        }

        time=m_lastTime;
    }
}
