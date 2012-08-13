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

#ifndef FJFLANGERAPP_H
#define FJFLANGERAPP_H

class FJFlangerApp : public QObject, public FJApp<calf_plugins::flanger_audio_module>
{
    Q_OBJECT
public:
    FJFlangerApp();

    virtual QByteArray save();
    static live::ObjectPtr load(const QByteArray&str);

    QObject* qoThis() { return this; }

    float getDelay() { return get_param_value(calf_plugins::flanger_metadata::par_delay); } /*003*/
    float getDepth() { return get_param_value(calf_plugins::flanger_metadata::par_depth); } /*004*/
    float getRate() { return get_param_value(calf_plugins::flanger_metadata::par_rate); } /*005*/
    float getFB() { return get_param_value(calf_plugins::flanger_metadata::par_fb); } /*006*/

    float getStereo() { return get_param_value(calf_plugins::flanger_metadata::par_stereo); } /*007*/
    float getReset() { return get_param_value(calf_plugins::flanger_metadata::par_reset); } /*008*/

    float getAmount() { return get_param_value(calf_plugins::flanger_metadata::par_amount); } /*009*/
    float getDryAmount() { return get_param_value(calf_plugins::flanger_metadata::par_amount); } /*010*/

public slots:
    void setDelay(float f) { set_param_value(calf_plugins::flanger_metadata::par_delay, f); } /*003*/
    void setDepth(float f) { set_param_value(calf_plugins::flanger_metadata::par_depth, f); } /*004*/
    void setRate(float f) { set_param_value(calf_plugins::flanger_metadata::par_rate, f); } /*005*/
    void setFB(float f) { set_param_value(calf_plugins::flanger_metadata::par_fb, f); } /*006*/

    void setStereo(float f) { set_param_value(calf_plugins::flanger_metadata::par_stereo, f); } /*007*/
    void setReset(float f) { set_param_value(calf_plugins::flanger_metadata::par_reset, f); } /*008*/

    void setAmount(float f) { set_param_value(calf_plugins::flanger_metadata::par_amount, f); } /*009*/
    void setDryAmount(float f) { set_param_value(calf_plugins::flanger_metadata::par_amount, f); } /*010*/

    void setWetDryBalance(float balance)// balance e [0,100], e Z
    {
        setAmount((balance - 50.0f) * 2.0f / 50.0f + 2.0f);
        setDryAmount((50.0f - balance) * 2.0f / 50.0f + 2.0f);
    }
};

#endif // FJFLANGERAPP_H
