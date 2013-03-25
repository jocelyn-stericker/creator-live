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

#include "app-calf/giface.h"
#include "app-calf/modules.h"
#include "app-calf/utils.h"

#include <live/audio>
#include <live/midi>
#include <live/midievent>

#ifndef FJAPP_H
#define FJAPP_H

// Farmer John App - Derp di derp di derp...
template<class Module> class FJApp : public live::Object, public calf_plugins::plugin_ctl_iface, public Module // Wait... what?
{
    using Module::in_count;
    using Module::out_count;
    using Module::param_count;
    using Module::progress_report; // Wow, I'm learning lots about templates!

    bool m_changed;
    float m_param[param_count];

    float* m_out[2];

public:
    LIVE_AUDIO
    LIVE_EFFECT
    FJApp(QString name)
        : live::Object(name, 0, 0, 2)
    {
        for (int i = 0; i < param_count; ++i)
            Module::params[i] = &m_param[i];

        clear_preset(); // giface.cpp
        Module::post_instantiate();

        init_module();

        for (int i = 0; i < 2; ++i)
            Module::outs[i] = m_out[i] = new float[live::audio::nFrames()];
    }

    virtual ~FJApp() {
        qDebug() << "~FJAPP";
        for (int i = 0; i < 2; ++i)
            delete[] m_out[i];
    }

    virtual void init_module() {
        Module::set_sample_rate(live::audio::sampleRate());
        Module::activate();
        Module::params_changed();   // remove
    }

    void mIn(const live::Event *data, live::ObjectChain* p) {
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

        p->push_back(this);
        mOut(data,p);
        p->pop_back();
    }

    virtual calf_plugins::parameter_properties* get_param_props(int param_no) { return Module::param_props + param_no; }

    void aIn(const float *data, int chan, live::Object* p)
    {
        Module::ins[chan] = const_cast<float*>(data); // FIXME: Module::ins should be const.
        if (!chan) return;

        if (m_changed) {
            Module::params_changed();
            m_changed = 0;
        }

        unsigned mask = Module::process(0, live::audio::nFrames(), -1, -1); // returns mask

        for (int i = 0; i < 2; ++i) {
            if (!(mask & (1 << i))) {
                dsp::zero(Module::outs[i], live::audio::nFrames());
            }
        }

        Module::params_reset();

        for (int i = 0; i < 2; ++i) {
            aOut(Module::outs[i], i, this);
        }
    }

    virtual float *get_params() { return m_param; }
    virtual bool activate_preset(int bank, int program) { return false; }
    virtual float get_param_value(int param_no) {
        return m_param[param_no];
    }
    virtual void set_param_value(int param_no, float value) {
        m_param[param_no] = value;
        m_changed = 1;
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
