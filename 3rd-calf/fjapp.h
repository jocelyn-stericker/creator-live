/* This file is part of the Calf Creator Live plugin.
 * The Calf Creator Live plugin is a fork of the Calf DSP library.
 *
 * Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "3rd-calf/giface.h"
#include "3rd-calf/modules.h"
#include "3rd-calf/utils.h"

#include <live/audio.h>
#include <live/midi.h>
#include <live/midievent.h>

#ifndef FJAPP_H
#define FJAPP_H

// Farmer John App - Derp di derp di derp...
template<class Module> class FJApp : public live::Object, public calf_plugins::plugin_ctl_iface, public Module // Wait... what?
{
    using Module::in_count;
    using Module::out_count;
    using Module::param_count;
    using Module::progress_report; // Wow, I'm learning lots about templates!

    bool s_changed;
    float s_param[param_count];

    const int& r_nFrames;
    float* s_out[2];

public:
    LIVE_HYBRID
    LIVE_EFFECT
    FJApp(QString name)
        : live::Object(name, 0, 0)
        , r_nFrames(live::audio::nFrames())
    {
        for (int i = 0; i < 2; i++) {
            Module::outs[i] = s_out[i] = new float[r_nFrames];
        }
        for (int i = 0; i < param_count; i++) {
            Module::params[i] = &s_param[i];
        }
        clear_preset(); // giface.cpp
        Module::post_instantiate();

        init_module();
    }

    virtual void init_module() {
        Module::set_sample_rate(live::audio::sampleRate());
        Module::activate();
        Module::params_changed();   // remove
    }

    void mIn(const live::Event *data, live::ObjectChain &p) {
        switch (data->simpleStatus())
        {
        case live::Event::NOTE_OFF:
            Module::note_off(data->data1, data->data2);
            break;
        case live::Event::NOTE_ON:
            if (!data->data2) Module::note_off(data->data1, 0);
            else Module::note_on(data->data1, data->data2);
            break;
        case live::Event::PROGRAM_CHANGE:
            Module::program_change(data->data1);
        case live::Event::PITCH_WHEEL:
            Module::pitch_bend(data->data1 + 128 * data->data2 - 8192);
            break;
        }

        p.push_back(this);
        mOut(data,p);
        p.pop_back();
    }

    virtual calf_plugins::parameter_properties* get_param_props(int param_no) { return Module::param_props + param_no; }

    void aIn(const float *data, int chan, live::ObjectChain &p)
    {
        Module::ins[chan] = const_cast<float*>(data); // FIXME: Module::ins should be const.
        if (!chan) return;

        if (s_changed) {
            Module::params_changed();
            s_changed = 0;
        }

        unsigned mask = Module::process(0, r_nFrames, -1, -1); // returns mask

        for (int i = 0; i < 2; ++i) {
            if (!(mask & (1 << i))) {
                dsp::zero(Module::outs[i], r_nFrames);
            }
        }

        p.push_back(this);
        for (int i = 0; i < 2; ++i) {
            aOut(s_out[i], i, p);
        }
        p.pop_back();
    }

    virtual float *get_params() { return s_param; }
    virtual bool activate_preset(int bank, int program) { return false; }
    virtual float get_param_value(int param_no) {
        return s_param[param_no];
    }
    virtual void set_param_value(int param_no, float value) {
        qDebug() << "Derp di derp on "<<param_no<<":"<<value;

        s_param[param_no] = value;
        s_changed = 1;
    }
    virtual void execute(int cmd_no) {
        Module::execute(cmd_no);
    }
    virtual char *configure(const char *key, const char *value) {
        return Module::configure(key, value);
    }
    virtual void send_configures(calf_plugins::send_configure_iface *sci) {
        Module::send_configures(sci);
    }
    virtual float get_level(unsigned int) {
        return 0.0f;
    }
};

#endif // FJAPP_H
