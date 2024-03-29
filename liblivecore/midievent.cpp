#include <live/midievent>

qint16 live::Event::simpleStatus() const {
    return qint16((message/0x10)*0x10);
}

qint16 live::Event::chan() const {
    return qint16(message%0x10);
}

qint16 live::Event::note() const {
    return data1;
}

qint16  live::Event::velocity() const {
    return data2;
}

qint16  live::Event::cc() const {
    return qint16((message<175||message>190)?-1:data1);
}

void live::Event::setSimpleStatus(const qint16 &a) {
    message = qint16(a + chan());
}

void live::Event::setChan(const qint16 &a) {
    message = qint16(simpleStatus() + a);
}

void live::Event::setNote(const qint16 &a) {
    data1=a;
}

void live::Event::setVelocity(const qint16 &a) {
    data2=a;
}

void live::Event::setCC(const qint16 &a) {
    data2=a;
}
