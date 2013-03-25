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

#ifndef FJPHASERFRAME_H
#define FJPHASERFRAME_H

#include <QFrame>

#include "fjphaserapp.h"
#include <live/appinterface>
#include <live_widgets/appframe.h>

namespace Ui {
class FJPhaserFrame;
}

class FJPhaserFrame : public live_widgets::AppFrame
{
    Q_OBJECT
    FJPhaserApp& m_app;

public:
    explicit FJPhaserFrame(FJPhaserApp *app, AbstractTrack *parent = 0);
    ~FJPhaserFrame();

    bool expanding() const { return false; }

public slots:
    void onFreq(int);
    void onDepth(int);
    void onRate(int);
    void onFB(int);
    void onStages(int);
    void onReset();
    void onWetDryBalance(int);
    void setMore(bool more);
    void addRounding();
    void removeRounding();
    
private:
    Ui::FJPhaserFrame *ui;
};

class FJPhaserCreator : public QObject, public live::AppInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AppInterface)
public:
    FJPhaserCreator()
    {
    }
    QString name() { return "ca.nettek.live.phaser"; }
    QString description() { return "Not the one from Star Trek..."; }
    live::ObjectPtr newBackend() { return new FJPhaserApp(); }
    live::ObjectPtr loadBackend(const QByteArray &str) {
        return FJPhaserApp::load(str);
    }
    live_widgets::AppFrame* newFrontend(live::ObjectPtr backend) { return new FJPhaserFrame(static_cast<FJPhaserApp*>(backend.data())); }
    QIcon icon() { return QIcon(":/icons/app_phaser.png"); }
    live::AppInterface* next() { return 0; }
};

#endif // FJPHASERFRAME_H
