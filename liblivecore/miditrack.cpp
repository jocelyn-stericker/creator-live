/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/miditrack>

#include "live/extern/midifile/MidiFile.h"

int live::MidiTrack::lastId=-1;

void live::MidiTrack::startPlayback() {
    Q_ASSERT(!b_playback);
    b_playback=1;

    b_recStart=b_curPos;
    b_systemTimeStart=live::midi::getTime_msec();
    b_curPos=live::midi::getTime_msec()-b_systemTimeStart+b_recStart;
    Time now = live::midi::getTime();

    if ( !b_mute ) {
        for ( int i = s_data->size()-1; i >=0; i-- ) {
            if ( (*s_data)[i]->time > b_curPos ) {
                Event* ev = new Event((*s_data)[i]->message,(*s_data)[i]->data1,(*s_data)[i]->data2);
                ev->time=(*s_data)[i]->time+now-b_curPos;
                ObjectChain p;
                p.push_back(this);
                mOut(ev,&p);
                p.pop_back();
                delete ev;
            }
        }
    }

    for (int i=0;i<s_cache.size();i++) {
        if (live::midi::getTime_msec()-s_cache[i].time>120) {
            s_cache.removeAt(i);
            --i;
        }
    }

//    if ( (b_record||b_overdub) ) {
//        ObjectChain p;
//        while (s_cache.size()) {
//            s_cache.front().time=Time();
//            mIn(&s_cache.front(),p);
//            s_cache.pop_front();
//        }
//    }

//    QTimer::singleShot(200,this,SLOT(timeEvent()));

    b_lastPos=b_curPos;
}

void live::MidiTrack::stopPlayback() {
    Q_ASSERT(b_playback);
    mPanic();

    //FLARE:
    Event can;
    can.flareId=0;
    ObjectChain p;
    p.push_back(this);
    mOut(&can,&p);

    for (int i=0; i<s_data->size(); i++) {
        if ((*s_data)[i]->simpleStatus()==Event::NOTE_ON||(*s_data)[i]->simpleStatus()==Event::NOTE_OFF) {
            // -> I don't get this assert. I'll look at it later.
//            Q_ASSERT(!((*s_data)[i]->velocity()&&!(*s_data)[i]->buddy));  /*all objects should have buddies via panic()*/
        }
    }

    b_playback=0;

}

void live::MidiTrack::mIn(const Event *ev, ObjectChain*p) {
    if (p->size() && cast<MidiEventCounter*>(p->back()) && s_thru) {
        mOut(ev,p);
        return;
    }
    live_mutex(x_mTrack) {

        if (isPlay()) {
            b_curPos=live::midi::getTime_msec()-b_systemTimeStart+b_recStart;
        }
        Q_ASSERT(b_curPos>=0);

        if (p->size()&&s_thru) {
            p->push_back(this);
            mOut(ev,p);   //Mute applies to outgoing only (!!)
            p->pop_back();
        }

        if ((ev->simpleStatus()==0xC0)) return;

        if (ev->message==-1) return;

        if ( (b_record||b_overdub) && b_playback && (ev->simpleStatus()==Event::NOTE_ON||ev->cc()!=-1||ev->simpleStatus()==Event::NOTE_OFF) ) {
            Time t=ev->time-b_systemTimeStart+b_recStart;
            Event* data = new Event;
            *data = *ev;

            data->time.nsec=t.nsec;
            data->time.sec=t.sec;

            int i = 0;
            for ( i = s_data->size()-1; i>=0; i-- ) {
                if ( (*s_data)[i]->time > t ) break;
                if ( (*s_data)[i]->time.toTime_ms() <= t.toTime_ms() ) {
                    if ( !b_overdub && (b_recStart<(*s_data)[i]->time)) {
                        delete s_data->takeAt( i );
                        emit dataUpdated();
                        if ( i+1 < *s_data ) {
                            i++;
                        }
                    }
                    continue;
                }
                Q_ASSERT(1);
            }
            if ((data->simpleStatus()==Event::NOTE_ON||data->simpleStatus()==Event::NOTE_OFF)&&!data->velocity()) {
                bool ok=0;
                for (int j=0;j<s_data->size();j++)   /*Where it would go*/ {
                    if ((*s_data)[j]->velocity()&&(*s_data)[j]->simpleStatus()==Event::NOTE_ON&&(*s_data)[j]->note()==data->note()&&!(*s_data)[j]->buddy) {
                        ok=1;
                        (*s_data)[j]->buddy=data;
                        data->buddy=(*s_data)[j];
                        (*s_data)[j]->buddy=data;
                        break;
                    }
                }
                if (ok) {
                    s_data->insert( i+1, data );
                }
                //else there _might_ be a bug...
            } else {
                s_data->insert( i+1, data );
            }
        } else {
            Event l=*ev;
            s_cache.push_back(l);
            s_cache.back().time=live::midi::getTime();
        }
        b_lastPos=b_curPos;
    }
    emit dataUpdated();
}

live::MidiTrack::MidiTrack()
  : Object("MIDI Track",false,false)
  , x_mTrack(QMutex::Recursive)
  , b_curPos(0)
  , b_lastPos(0)
  , b_recStart(-1)
  , b_systemTimeStart(-1)
  , b_record(0)
  , b_overdub(0)
  , b_playback(0)
  , b_mute(0)
  , s_data(new QList<Event*>)
  , mTrack_id(++lastId)
  , s_cache()
  , s_thru(1)
  { setTemporary(0);
}

const bool& live::MidiTrack::isRecord() const {
    return b_record.ref();
}

const bool& live::MidiTrack::isOverdub() const {
    return b_overdub.ref();
}

const bool& live::MidiTrack::isPlay() const {
    return b_playback.ref();
}

const bool& live::MidiTrack::isMute() const {
    return b_mute.ref();
}

const int& live::MidiTrack::pos() {
    if (isPlay()) {
        b_curPos=live::midi::getTime_msec()-b_systemTimeStart+b_recStart;
    }
    return b_curPos.ref();
}

void live::MidiTrack::startRecord() {
    Q_ASSERT(!b_record);
    Q_ASSERT(!b_overdub);
    b_record=1;
}

void live::MidiTrack::  stopRecord() {
    Q_ASSERT(b_record);
    Q_ASSERT(!b_overdub);
    b_record=0;
}

void live::MidiTrack::startOverdub() {
    Q_ASSERT(!b_overdub);
    Q_ASSERT(!b_record);
    b_overdub=1;
}

void live::MidiTrack::stopOverdub() {
    Q_ASSERT(b_overdub);
    Q_ASSERT(!b_record);
    b_overdub=0;
}

void live::MidiTrack::startMute() {
    Q_ASSERT(!b_mute);
    b_mute=1;
}

void live::MidiTrack::stopMute() {
    Q_ASSERT(b_mute);
    b_mute=0;
}

void live::MidiTrack::setPos(int pos) {
    Q_ASSERT(pos>=0);
    if (isPlay()) live_mutex(x_mTrack) {
        stopPlayback();
        b_curPos=pos;
        startPlayback();
    } else {
        b_curPos=pos;
    }
}

void live::MidiTrack::importFile(QString path) {
    if (!QFile::exists(path)) {
        return;
    }

    MidiFile mf;
    mf.read(path.toAscii());
    mf.absoluteTime();
    if (!mf.getNumTracks()) {
        return;
    }

    live_mutex(x_mTrack) for (int i=0;i<mf.getNumEvents(0);i++) {
        MFEvent ev=mf.getEvent(0,i);
        if (ev.data.getSize()!=3) {
            continue;
        }
        Event* mev=new Event(ev.data[0],ev.data[1],ev.data[2]);
        mev->time=Time((qint32)(mf.getTimeInSeconds(0,i)*1000.0f));
        s_data->push_back(mev);
    }
}

void live::MidiTrack::exportFile(QString path) {
    MidiFile mf(path.toAscii());
    mf.absoluteTime();
    Array<uchar> midievent;
    midievent.setSize(3);
    int tpq=96;
    mf.setTicksPerQuarterNote(tpq);

    double secondsPerTick = 60.0 / (120.0f * (double) tpq);

    live_mutex(x_mTrack) for (int i=0;i<s_data->size();i++) {
        midievent[0] = (uchar)(*s_data)[i]->message;     // store a note on command (MIDI channel 1)
        midievent[1] = (uchar)(*s_data)[i]->data1;
        midievent[2] = (uchar)(*s_data)[i]->data2;
        mf.addEvent(0, (int)(((double)(*s_data)[i]->time.toTime_ms()/1000.0f)/secondsPerTick), midievent);
    }
    mf.sortTracks();
    mf.write(path.toAscii());
    return;
}

void live::MidiTrack::timeEvent() {
    live_mutex(x_mTrack) {
        if (isPlay()) {
            //        QTimer::singleShot(20,this,SLOT(timeEvent()));
        } else {
            return;
        }

        if (isRecord()) {
            Time t=pos();

            int i = 0;
            for ( i = s_data->size()-1; i>=0; i-- ) {
                if ( (*s_data)[i]->time > t ) break;
                if ( (*s_data)[i]->time.toTime_ms() <= t.toTime_ms() ) {
                    if ( !b_overdub && (b_recStart<(*s_data)[i]->time)) {
                        delete s_data->takeAt( i );
                        emit dataUpdated();
                        if ( i+1 < *s_data ) {
                            i++;
                        }
                    }
                    continue;
                }
                Q_ASSERT(1);
            }
        }
    }
}

