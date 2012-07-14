/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef PIANO_KEY_H
#define PIANO_KEY_H

#include <QtGlobal>
#include <QGraphicsRectItem>
#include "live/object.h"
#include "liblivewidgets_global.h"

class LiveInputMIDI;
class LiveOutputMIDI;

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT PianoKey : public QObject, public live::Object, public QGraphicsRectItem
{
    Q_OBJECT
private:
    static int s_shiftTwinID;
    static live_widgets::PianoKey* s_universe[300];
    bool s_virtual;
    int s_id;
    bool s_enabled;

public:
    LIVE_MIDI
    LIVE_EFFECT //fixme==> hidden
    bool mOn() const{ return 0; } bool aOn() const { return 1; }
    explicit PianoKey( qreal x, qreal y, qreal width, qreal height, int id, QGraphicsItem * parent );

    void enableKey(bool enabled);
    void mousePressEvent( QGraphicsSceneMouseEvent* event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );
    void setID( int id );
    void setVirtual( bool isVirtual )
    {
        s_virtual = isVirtual;
    }
    void mIn(const live::Event *data, live::ObjectChain&p);

    int id() const
    {
        return s_id;
    }
    int isEnabled() const
    {
        return s_enabled;
    }
    bool isVirtual() const
    {
        return s_virtual;
    }

signals:
    void updated();
};

}

#endif//PIANO_KEY_H

// Yay!
