/*****************************************************
                                           rev. 110720

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield  (trintith@gmail.com)

                 All rights reserved.
*****************************************************/

#include <live_widgets/pianokey.h>
#include <live/object>
#include <live/midievent>

#include <QGraphicsSceneMouseEvent>

int live_widgets::PianoKey::m_shiftTwinID = -1;
live_widgets::PianoKey* live_widgets::PianoKey::m_universe[300] = {0};

live_widgets::PianoKey::PianoKey( qreal x, qreal y, qreal width, qreal height, qint16 id, QGraphicsItem * parent )
  : live::Object("Virtual Piano Key (transient)",false,false,2)
  , QGraphicsRectItem(x, y, width, height, parent)
  , m_virtual(0)
  , m_id(id)
  , m_enabled(0)
  { setData(-999,"PianoKey");
    m_universe[id]=this;
}

void live_widgets::PianoKey::enableKey(bool c_enabled) {
    m_enabled=c_enabled;
    emit updated();         // let Hathor know and decide if this is what was intended.
}

void live_widgets::PianoKey::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    QGraphicsItem::mousePressEvent(event);

    if ( m_virtual ) {
        live::Event* ev = new live::Event;
        ev->setChan(0);
        ev->setNote(m_id);
        ev->setVelocity(127);

        live::ObjectChain p;
        p.push_back(this);
        mOut(ev,&p);
        return;
    }

    if ( this->isSelected() && ( event->button() == Qt::LeftButton) ) {
        this->setSelected( 0 );

        emit updated();

        if ( ( event->modifiers() == Qt::ShiftModifier ) && (m_shiftTwinID!=-1) ) {
            int minId=m_shiftTwinID<m_id?m_shiftTwinID:m_id, maxId=m_shiftTwinID<m_id?m_id:m_shiftTwinID;
            for (int i=minId; i<=maxId; i++) {
                m_universe[i]->enableKey(1);
            }
        } else enableKey(1);

    } else if (event->button() == Qt::RightButton ) {
        if ( ( event->modifiers() == Qt::ShiftModifier ) && m_shiftTwinID!=-1) {
            int minId=m_shiftTwinID<m_id?m_shiftTwinID:m_id, maxId=m_shiftTwinID<m_id?m_id:m_shiftTwinID;
            for (int i=minId; i<=maxId; i++) {
                m_universe[i]->enableKey(0);
            }
        } else enableKey(0);
        this->setSelected( 0 );
    }

    m_shiftTwinID = m_id;
}

void live_widgets::PianoKey::mouseReleaseEvent( QGraphicsSceneMouseEvent* ) {
    if ( m_virtual ) {
        live::Event* ev = new live::Event;
        ev->setChan(0);
        ev->setNote(m_id);
        ev->setVelocity(0); //off

        live::ObjectChain p;
        p.push_back(this);
        mOut(ev, &p);
        return;
    }
}

void live_widgets::PianoKey::setID( qint16 id ) {
    m_id = id;
}

void live_widgets::PianoKey::mIn(const live::Event *data, live::ObjectChain*p) {
    p->push_back(this);
    if (m_enabled) {
        mOut(data,p);
    }
    p->pop_back();
}

// Yay!
