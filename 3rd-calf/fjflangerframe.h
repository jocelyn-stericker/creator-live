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

#ifndef FJFLANGERFRAME_H
#define FJFLANGERFRAME_H

#include "fjapp.h"
#include "fjfilterframe.h"
#include "fjflangerapp.h"

#include <live/appinterface>
#include <live_widgets/appframe.h>
#include <QFrame>

namespace Ui {
class FJFlangerFrame;
}

class FJFlangerFrame : public live_widgets::AppFrame
{
    Q_OBJECT
    FJFlangerApp& s_app;

public:
    explicit FJFlangerFrame(FJFlangerApp* backend, AbstractTrack *parent = 0);
    ~FJFlangerFrame();

    bool expanding() const { return false; }

public slots:
    void onDelay(int);
    void onDepth(int);
    void onRate(int);
    void onFB(int);

    void onStereo(int);
    void onReset();

    void onWetDryBalance(int);

private:
    Ui::FJFlangerFrame *ui;
};


class FJFlangerCreator : public QObject, public live::AppInterface
{
    Q_OBJECT
    Q_INTERFACES(live::AppInterface)
public:
    FJFlangerCreator()
    {
    }
    QString name() { return "Flanger"; }
    QString description() { return "Wah wah wah..."; }
    live::ObjectPtr newBackend() { return new FJFlangerApp(); }
    live::ObjectPtr loadBackend(const QByteArray &str) {
        return FJFlangerApp::load(str);
    }
    live_widgets::AppFrame* newFrontend(live::ObjectPtr backend) { return new FJFlangerFrame(static_cast<FJFlangerApp*>(backend.data())); }
    QIcon icon() { return QIcon(":/icons/app_flanger.png"); }
    live::AppInterface* next() { return new FJFilterCreator; }
};

#endif // FJFLANGERFRAME_H
