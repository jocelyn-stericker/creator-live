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

#ifndef FJREVERBFRAME_H
#define FJREVERBFRAME_H

#include <QWidget>

#include <live_widgets/appframe.h>
#include <live/appinterface>
#include "fjreverbapp.h"
#include "fjflangerframe.h"

namespace Ui {
class FJReverbFrame;
}

class FJReverbFrame : public live_widgets::AppFrame
{
    Q_OBJECT
    FJReverbApp& s_app;
    
public:
    explicit FJReverbFrame(FJReverbApp* backend, AbstractTrack *parent = 0);
    ~FJReverbFrame();

    bool expanding() const { return false; }

public slots:
    void onRoomSize(int);
    void onDiffusion(int);
    void onDecayTime(int);
    void onHFDamp(int);
    void onTrebelCut(int);
    void onBassCut(int);
    void onPredelay(int);

    void onWetDryBalance(int);

    void setMore(bool);

    void addRounding();
    void removeRounding();
    
private:
    Ui::FJReverbFrame *ui;
};

class FJReverbCreator : public QObject, public live::AppInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AppInterface)
public:
    FJReverbCreator()
    {
    }
    QString name() { return "Reverb"; }
    QString description() { return "The gaussian blur of sound..."; }
    live::ObjectPtr newBackend() { return new FJReverbApp(); }
    live::ObjectPtr loadBackend(const QByteArray &str) {
        return FJReverbApp::load(str);
    }
    live_widgets::AppFrame* newFrontend(live::ObjectPtr backend) { return new FJReverbFrame(static_cast<FJReverbApp*>(backend.data())); }
    QIcon icon() { return QIcon(":/icons/app_reverb.png"); }
    live::AppInterface* next() { return new FJFlangerCreator; }
};

#endif // FJREVERBFRAME_H
