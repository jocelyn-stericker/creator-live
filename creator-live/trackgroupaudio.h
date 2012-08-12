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
    QWidget* actionw_parent;
public:
    explicit TrackGroupAudio(live::ObjectPtr  c_input, QWidget* c_parent,bool empty=0);
    ~TrackGroupAudio();

public slots:
    void newHathorAuto();

public:
    static TrackGroupAudio* load(const QByteArray&,QWidget*parent);
    QByteArray save();

};

#endif // METAHATHORAUDIO_H
