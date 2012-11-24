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
  , s_settings(new_fluid_settings())
  , s_synth(0)
  , s_cache()
  , s_connection(live::audio::null(2), this, live::AudioConnection)
  { qDebug() << "Hi..." << url;

    for(int i=0;i<2;i++)
        s_cache[i]=new float[live::audio::nFrames()];
    fluid_settings_setnum(s_settings,"synth.sample-rate",live::audio::sampleRate());
    fluid_settings_setnum(s_settings,"synth.gain",3.0);
    live_mutex(x_synth) {
        s_synth = new_fluid_synth(s_settings);
        fluid_synth_sfload(s_synth,url.toLatin1(),1);
    }
}

void Soundfont::mIn(const live::Event* data, live::ObjectChain *p) {
    Q_UNUSED(p);

    if(data->cc()!=-1) {
        fluid_synth_cc(s_synth,data->chan(),data->cc(),data->data2);
    } else switch(data->simpleStatus()) {
    case live::Event::NOTE_OFF:
        fluid_synth_noteoff(s_synth, data->chan(), data->note());
        break;
    case live::Event::NOTE_ON:
        fluid_synth_noteon(s_synth, data->chan(), data->note(), data->velocity());
        break;
    case live::Event::POLYPHONIC_AFTERTOUCH:
        //IGNORED
        break;
    case live::Event::PROGRAM_CHANGE:
        setProgram(data->chan(), data->data1);
        break;
    case live::Event::CHANNEL_AFTERTOUCH:
    #ifndef __QNX__
        fluid_synth_channel_pressure(s_synth, data->chan(), data->data1);
    #endif
        break;
    case live::Event::PITCH_WHEEL:
        fluid_synth_pitch_bend(s_synth, data->chan(), data->data1);
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

    if(!chan) fluid_synth_write_float(s_synth,live::audio::nFrames(),s_cache[0],0,1,s_cache[1],0,1);
    aOut(s_cache[chan],chan, p);

    x_synth.unlock();
}

void Soundfont::setProgram(int chan, int program) {
    fluid_synth_program_change(s_synth, chan, program);
    emit programChanged(chan, program);
}

