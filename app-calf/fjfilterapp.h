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

#include "fjapp.h"
#include "modules.h"

#ifndef FJFILTER_H
#define FJFILTER_H

class FJFilterApp : public QObject, public FJApp<calf_plugins::filter_audio_module>
{
    Q_OBJECT
public:
    enum Filter {
        F12LowPass = 0,
        F24LowPass,
        F36LowPass,
        F12HighPass,
        F24HighPass,
        F36HighPass,
        F6BandPass,
        F12BandPass,
        F18BandPass,
        F6BandRegect,
        F12BandReject,
        F18BandReject,
        NumberOfFilterTypes
    };

    FJFilterApp();

    virtual QByteArray save();
    static live::ObjectPtr load(const QByteArray&str);

    QObject* qoThis() { return this; }

public:
    float getCutoff() { return get_param_value(calf_plugins::filter_metadata::par_cutoff); } /*003*/
    float getResonance() { return get_param_value(calf_plugins::filter_metadata::par_resonance); } /*004*/
    Filter getMode() { return (Filter)(int)get_param_value(calf_plugins::filter_metadata::par_mode); } /*005*/
    float getInertia() { return get_param_value(calf_plugins::filter_metadata::par_inertia); } /*006*/

public slots:
    void setCutoff(float f) { set_param_value(calf_plugins::filter_metadata::par_cutoff, f); }
    void setResonance(float f) { set_param_value(calf_plugins::filter_metadata::par_resonance, f); }
    void setMode(Filter f) { set_param_value(calf_plugins::filter_metadata::par_mode, (float)f); }
    void setIntertia(float f) { set_param_value(calf_plugins::filter_metadata::par_inertia, f); }
};

#endif // FJFILTER_H
