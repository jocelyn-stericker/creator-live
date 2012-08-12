/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef TRACKGROUPAUDIO_H
#define TRACKGROUPAUDIO_H

#include "trackgroup.h"

namespace live_widgets {
    class TrackInputWidget;
}

class TrackGroupAudio : public TrackGroup
{
    Q_OBJECT
protected:
    QHBoxLayout* mainLayout;
    live_widgets::TrackInputWidget *instLabel;
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
