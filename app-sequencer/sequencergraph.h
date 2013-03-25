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
    bool m_bindMode;
    qint64 scale() { return m_scale; }
public slots:
    void setTime( double time );
    void updateAudioData( qint64 t1 = -1, qint64 t2 = -1 );
    void updateMidiData( float t1 = -1, float t2 = -1 );

    QList<live::Event> getEvents(qint64 evx,qint64 evy);
    virtual void paintEvent( QPaintEvent* event );
    void mousePressEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );
    void wheelEvent(QWheelEvent *);
    void keyPressEvent(QKeyEvent *);
    void resizeEvent(QResizeEvent *);
    void setScroll(const qint64& scroll) { m_initial=scroll; updateAudioData(); }
    const qint64& getScroll() const { return m_initial; }
    void incrScroll();
    void decrScroll();
    void setShowBindingsChanged(bool ean);
    void updatePos(qint64);
    void setScale(qint64);

    void setUpdatesDisabled(bool b) { QWidget::setUpdatesEnabled(!b); if(!b) updateAudioData(); }
private:
    float selection;
    qint64 m_initial;
    qint64 m_redrawpos_st,m_redrawpos_nd;
    qint64 m_leftMost, m_rightMost;
    qint64 m_scale;
//Audio
    live::AudioTrack* audioTrack;
    qint64 oldBoxWidth;
    QPixmap* audioOriginal[2];
    qint64 audioEstart;
    double audioLTime;
    double lastA;
    double lastB;
//Midi
    live::MidiTrack* midiTrack;
    QPixmap* midiOriginal;
    qint64 midiEstart;
    double midiLTime;
};

#endif // SEQUENCERGRAPH_H
