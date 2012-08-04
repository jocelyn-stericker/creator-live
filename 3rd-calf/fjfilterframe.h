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

#ifndef FJFILTERFRAME_H
#define FJFILTERFRAME_H

#include "fjfilterapp.h"
#include "fjphaserframe.h"

#include <QFrame>
#include <live/appinterface>
#include <live_widgets/appframe.h>

namespace Ui {
class FJFilterFrame;
}

class FJFilterFrame : public live_widgets::AppFrame
{
    Q_OBJECT

    FJFilterApp& s_app;
public:
    explicit FJFilterFrame(FJFilterApp* s_backend, AbstractTrack *parent = 0);
    ~FJFilterFrame();

    bool expanding() const { return false; }
    
public slots:
    void onCutoff(int);
    void onResonance(int);
    void onMode(int=-1);
    void onIntertia(int);
private:
    Ui::FJFilterFrame *ui;
};

class FJFilterCreator : public QObject, public live::AppInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AppInterface)
public:
    FJFilterCreator()
    {
    }
    QString name() { return "Filter"; }
    QString description() { return "No low quality equipment required..."; }
    live::ObjectPtr newBackend() { return new FJFilterApp(); }
    live::ObjectPtr loadBackend(const QByteArray &str) {
        return FJFilterApp::load(str);
    }
    live_widgets::AppFrame* newFrontend(live::ObjectPtr backend) { return new FJFilterFrame(static_cast<FJFilterApp*>(backend.data())); }
    QIcon icon() { return QIcon(":/icons/app_audioFilter.png"); }
    live::AppInterface* next() { return new FJPhaserCreator; }
};

#endif // FJFILTERFRAME_H
