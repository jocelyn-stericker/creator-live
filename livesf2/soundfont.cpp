/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( SF2 PLUGIN ) ) )

*******************************************************/

#include "soundfont.h"

#include <live/audio>
#include <live/midievent>

//"/home/joshua/Dropbox/Etc/mustheory2.sf2"

Soundfont::Soundfont(QString url)
  : live::Object("SOUNDFONT", false, true, 2)
  , m_settings(new_fluid_settings())
  , m_synth(0)
  , m_cache()
  , m_connection(live::audio::null(2), this, live::AudioConnection)
  { qDebug() << "Hi..." << url;

    for(int i=0;i<2;i++)
        m_cache[i]=new float[live::audio::nFrames()];
    fluid_settings_setnum(m_settings,"synth.sample-rate",live::audio::sampleRate());
    fluid_settings_setnum(m_settings,"synth.gain",3.0);
    live_mutex(x_synth) {
        m_synth = new_fluid_synth(m_settings);
        fluid_synth_sfload(m_synth,url.toLatin1(),1);
    }
}

void Soundfont::mIn(const live::Event* data, live::ObjectChain *p) {
    Q_UNUSED(p);

    if(data->cc()!=-1) {
        fluid_synth_cc(m_synth,data->chan(),data->cc(),data->data2);
    } else switch(data->simpleStatus()) {
    case live::Event::NOTE_OFF:
        fluid_synth_noteoff(m_synth, data->chan(), data->note());
        break;
    case live::Event::NOTE_ON:
        fluid_synth_noteon(m_synth, data->chan(), data->note(), data->velocity());
        break;
    case live::Event::POLYPHONIC_AFTERTOUCH:
        //IGNORED
        break;
    case live::Event::PROGRAM_CHANGE:
        setProgram(data->chan(), data->data1);
        break;
    case live::Event::CHANNEL_AFTERTOUCH:
    #ifndef __QNX__
        fluid_synth_channel_pressure(m_synth, data->chan(), data->data1);
    #endif
        break;
    case live::Event::PITCH_WHEEL:
        fluid_synth_pitch_bend(m_synth, data->chan(), data->data1);
        break;
    case live::Event::SYSEX:
        //IGNORED
        break;
    }
}

void Soundfont::aIn(const float* data, int chan, live::Object* p) {
    Q_UNUSED(data);
    Q_ASSERT(chan<2&&chan>=0);

    if (!x_synth.tryLock()) {
        aOut(data, chan, p);
        return;
    }

    if(!chan) fluid_synth_write_float(m_synth,live::audio::nFrames(),m_cache[0],0,1,m_cache[1],0,1);
    aOut(m_cache[chan],chan, p);

    x_synth.unlock();
}

void Soundfont::setProgram(int chan, int program) {
    fluid_synth_program_change(m_synth, chan, program);
    emit programChanged(chan, program);
}

