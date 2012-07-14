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
    int s_selectedFilter;       /*005*/
    int s_selectedMode;         /*006*/
public:
    explicit TrackGroupMidi(live::ObjectPtr  c_input, QWidget* c_parent, bool empty=0);

    QHBoxLayout* ui_mainLayout;
    QVBoxLayout* ui_subLayout1;
    live_widgets::RotatedLabel *ui_instLabel;
    QGraphicsView* ui_instView;
    QGraphicsScene* ui_instScene;
    QWidget* ui_actionw_parent;
    live_widgets::ToolButton* ui_colourSelect[5];
    live_widgets::ToolButton* ui_modeSelect[5];
    live::MidiFilter* s_midiFilters[5];   /*007*/
signals:

public slots:
    void newHathorAuto();
    void reactToPianoKeyUpdate();
    void drawKeyboard();
    void changeActiveFilter(bool really);
    void changeActiveMode(bool really);
    void changeActiveMode_2(int mode);

public:
    static TrackGroupMidi* load(const QByteArray&str,QWidget*c_parent);
    QByteArray save();
};

#endif // METAHATHORMIDI_H
