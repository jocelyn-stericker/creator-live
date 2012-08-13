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

#ifndef FJREVERBAPP_H
#define FJREVERBAPP_H

class FJReverbApp : public QObject, public FJApp<calf_plugins::reverb_audio_module>
{
    Q_OBJECT
public:
    FJReverbApp();

    enum RoomSize {
        Small = 0,
        Medium,
        Large,
        TunnelLike,
        LargeSmooth,
        Experimental,
        NumberOfRoomSizes
    };

    virtual QByteArray save();
    static live::ObjectPtr load(const QByteArray&str);

    QObject* qoThis() { return this; }

    float getRoomSize() { return get_param_value(calf_plugins::reverb_metadata::par_roomsize); } /*003*/
    float getDiffusion() { return get_param_value(calf_plugins::reverb_metadata::par_diffusion); } /*004*/
    float getDecayTime() { return get_param_value(calf_plugins::reverb_metadata::par_decay); } /*005*/
    float getHFDamp() { return get_param_value(calf_plugins::reverb_metadata::par_hfdamp); } /*006*/

    float getWet() { return get_param_value(calf_plugins::reverb_metadata::par_amount); } /*007*/
    float getDry() { return get_param_value(calf_plugins::reverb_metadata::par_dry); } /*008*/

    float getTrebleCut() { return get_param_value(calf_plugins::reverb_metadata::par_treblecut); } /*009*/
    float getBassCut() { return get_param_value(calf_plugins::reverb_metadata::par_basscut); } /*010*/

    float getPredelay() { return get_param_value(calf_plugins::reverb_metadata::par_predelay); } /*011*/

public slots:
    void setRoomSize(RoomSize roomSize) { set_param_value(calf_plugins::reverb_metadata::par_roomsize, roomSize); }
    void setDiffusion(float diff) { set_param_value(calf_plugins::reverb_metadata::par_diffusion, diff); }
    void setDecayTime(float time) { set_param_value(calf_plugins::reverb_metadata::par_decay, time); }
    void setHFDamp(float damp) { set_param_value(calf_plugins::reverb_metadata::par_hfdamp, damp); }

    void setWet(float wet) { set_param_value(calf_plugins::reverb_metadata::par_amount, wet); }
    void setDry(float dry) { set_param_value(calf_plugins::reverb_metadata::par_dry, dry); }

    void setTrebleCut(float cut) { set_param_value(calf_plugins::reverb_metadata::par_treblecut, cut); }
    void setBassCut(float cut) { set_param_value(calf_plugins::reverb_metadata::par_basscut, cut); }

    void setPredelay(float delay) { set_param_value(calf_plugins::reverb_metadata::par_predelay, delay); }

    void setWetDryBalance(float balance) { // balance e [0,100], e Z
        setWet((balance - 50.0f) * 1.0f / 50.0f + 1.0f);
        setDry((50.0f - balance) * 1.0f / 50.0f + 1.0f);
    }
};

#endif // FJREVERBAPP_H
