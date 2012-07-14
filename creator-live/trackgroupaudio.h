/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef TRACKGROUPAUDIO_H
#define TRACKGROUPAUDIO_H

#include "trackgroup.h"

class TrackGroupAudio : public TrackGroup
{
    Q_OBJECT
protected:
    QHBoxLayout* mainLayout;
    live_widgets::RotatedLabel *instLabel;
    QWidget* actionw_parent;
public:
    explicit TrackGroupAudio(live::ObjectPtr  c_input, QWidget* c_parent,bool empty=0);
    ~TrackGroupAudio();

signals:

public slots:
    void setLastOutput(live::ObjectPtr);
    void newHathorAuto();

public:
    static TrackGroupAudio* load(const QByteArray&,QWidget*parent);
    QByteArray save();

};

#endif // METAHATHORAUDIO_H
