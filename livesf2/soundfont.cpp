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
  : live::Object("SOUNDFONT", false, true)
  , s_settings(new_fluid_settings())
  , s_synth(0)
  , s_cache()
  { for(int i=0;i<2;i++)
        s_cache[i]=new float[live::audio::nFrames()];
    fluid_settings_setnum(s_settings,"synth.sample-rate",live::audio::sampleRate());
    fluid_settings_setnum(s_settings,"synth.gain",3.0);
    s_synth = new_fluid_synth(s_settings);
    fluid_synth_sfload(s_synth,url.toAscii(),1);
}

void Soundfont::mIn(const live::Event *data, live::ObjectChain &p) {
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
    case live::Event::POLYPHNOIC_AFTERTOUCH:
        //IGNORED
        break;
    case live::Event::PROGRAM_CHANGE:
        setProgram(data->chan(), data->data1);
        break;
    case live::Event::CHANNEL_AFTERTOUCH:
        fluid_synth_channel_pressure(s_synth, data->chan(), data->data1);
        break;
    case live::Event::PITCH_WHEEL:
        fluid_synth_pitch_bend(s_synth, data->chan(), data->data1);
        break;
    case live::Event::SYSEX:
        //IGNORED
        break;
    }
}

void Soundfont::aIn(const float *data, int chan, live::ObjectChain &p) {
    Q_UNUSED(data);
    Q_ASSERT(chan<2&&chan>=0);

    if(!chan) fluid_synth_write_float(s_synth,live::audio::nFrames(),s_cache[0],0,1,s_cache[1],0,1);
    aOut(s_cache[chan],chan,&p);
}

void Soundfont::setProgram(int chan, int program) {
    fluid_synth_program_change(s_synth, chan, program);
    emit programChanged(chan, program);
}

