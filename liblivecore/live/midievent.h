/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef EVENT_H
#define EVENT_H

#include "live/object.h"
#include "live/time.h"

namespace live {

class LIBLIVECORESHARED_EXPORT Event
{
public:
    static const qint16 NOTE_OFF = 0x80;
    static const qint16 NOTE_ON  = 0x90;
    static const qint16 POLYPHNOIC_AFTERTOUCH = 0xA0;
    static const qint16 PROGRAM_CHANGE = 0xC0;
    static const qint16 CHANNEL_AFTERTOUCH = 0xD0;
    static const qint16 PITCH_WHEEL = 0xE0;
    static const qint16 SYSEX = 0xF0;

    qint16 message;         /*003*/
    qint16 data1;           /*004*/
    qint16 data2;           /*005*/
    Time time;              /*006*/
    bool isShadow;          /*007*/
    qint16 flareId;         /*008*/ /*Deprictate...*/
        /*0=delete first in refpipeline*/

    /**
      *For an event with velocity, the event which terminates it. for an event without velocity, the event which
      *started it
      */
    Event* buddy;


    Event() : message(0), data1(0), data2(0), time(),isShadow(1), flareId(-1), buddy(0) { }

    Event(const qint16& cmessage,const qint16&cdata1,const qint16&cdata2) : message(cmessage), data1(cdata1), data2(cdata2), time(),isShadow(1), flareId(-1) { }

    virtual ~Event() {}

    qint16 simpleStatus() const;
    qint16 chan() const;
    qint16 note() const;
    qint16 velocity() const;
    qint16 cc() const;

    void setSimpleStatus(const qint16&);
    void setChan(const qint16&);
    void setNote(const qint16&);
    void setVelocity(const qint16&);
    void setCC(const qint16&);

    QByteArray save() const;
    static Event* load(const QByteArray&str);
    virtual void noOp() {}
};

}

////////////////////////////////////////////////////////////////

inline QDataStream& operator<<(QDataStream&out,const live::Event& ev)
{
    out << const_cast<live::Event&>(ev).save();
    return out;
}

inline QDataStream& operator>>(QDataStream&in,live::Event& ev)
{
    QByteArray ba;
    in>>ba;
    ev = *live::Event::load(ba);
    return in;
}

inline uint qHash(const live::Event &key)
{
    return qHash(key.data1);
}

inline bool operator==(const live::Event&e1,const live::Event&e2)
{
    return(e1.message==e2.message)&&(e1.data1==e2.data1)&&(e1.data2==e2.data2);
}

#endif // EVENT_H
