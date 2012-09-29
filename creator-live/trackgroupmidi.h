/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef TRACKGROUPMIDI_H
#define TRACKGROUPMIDI_H

#include "trackgroup.h"
class QGraphicsView;
class QGraphicsScene;
typedef QColor QColour;

class TrackGroupMidi : public TrackGroup
{
    Q_OBJECT
public:
    explicit TrackGroupMidi(live::ObjectPtr  c_input, QWidget* c_parent, bool empty=0);

    QHBoxLayout* ui_mainLayout;
signals:

public slots:
    void newHathorAuto();

public:
    static TrackGroupMidi* load(const QByteArray&str,QWidget*c_parent);
    QByteArray save();

private:
    TrackGroupMidi(const TrackGroupMidi&);
    TrackGroupMidi& operator=(const TrackGroupMidi&);
};

#endif // METAHATHORMIDI_H
