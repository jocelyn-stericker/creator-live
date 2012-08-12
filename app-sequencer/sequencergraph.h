/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef SEQUENCERGRAPH_H
#define SEQUENCERGRAPH_H

#include "sequencerapp.h"
#include <QWidget>

class SequencerGraph : public QWidget
{
    Q_OBJECT
public:
    SequencerApp* app;
    SequencerGraph( QWidget* parent,SequencerApp* app );
    ~SequencerGraph();
    bool s_bindMode;
    int scale() { return s_scale; }
public slots:
    void setTime( double time );
    void updateAudioData( int t1 = -1, int t2 = -1 );
    void updateMidiData( float t1 = -1, float t2 = -1 );

    QList<live::Event> getEvents(int evx,int evy);
    virtual void paintEvent( QPaintEvent* event );
    void mousePressEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );
    void wheelEvent(QWheelEvent *);
    void keyPressEvent(QKeyEvent *);
    void setScroll(const int& scroll) { s_initial=scroll; updateAudioData(); }
    const int& getScroll() const { return s_initial; }
    void incrScroll();
    void decrScroll();
    void setShowBindingsChanged(bool ean);
    void updatePos(quint64);
    void setScale(int);

    void setUpdatesDisabled(bool b) { QWidget::setUpdatesEnabled(!b); if(!b) updateAudioData(); }
private:
    float selection;
    int s_initial;
    int s_redrawpos_st,s_redrawpos_nd;
    int s_leftMost, s_rightMost;
    int s_scale;
//Audio
    live::AudioTrack* audioTrack;
    QPixmap* audioOriginal[2];
    int audioEstart;
    int simpleCounter;
    double audioLTime;
    double lastA;
    double lastB;
//Midi
    live::MidiTrack* midiTrack;
    QPixmap* midiOriginal;
    int midiEstart;
    double midiLTime;
};

#endif // SEQUENCERGRAPH_H
