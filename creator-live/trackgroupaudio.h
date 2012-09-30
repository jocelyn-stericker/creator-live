/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef TRACKGROUPAUDIO_H
#define TRACKGROUPAUDIO_H

#include "trackgroup.h"

#include <live_widgets/trackinputselect.h>

class TrackGroupAudio : public TrackGroup
{
    Q_OBJECT
protected:
    QHBoxLayout* mainLayout;
public:
    explicit TrackGroupAudio(live::ObjectPtr  c_input, QWidget* c_parent,bool empty=0, live_widgets::ObjectChooser* oc=0);
    ~TrackGroupAudio();

public slots:
    void newHathorAuto();

public:
    static TrackGroupAudio* load(const QByteArray&,QWidget*parent);
    QByteArray save();

private:
    TrackGroupAudio(const TrackGroupAudio&);
    TrackGroupAudio& operator=(TrackGroupAudio&) {
        TCRASH();
        return *this;
    }

};

#endif // METAHATHORAUDIO_H
