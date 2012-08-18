/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "midisystem_p.h"

#include <live/audio>
#include <live/liblivecore_global>
#include <live/mapping>
#include <live/midi>
#include <live/midievent>
#include <live/object>
#include <QMutex>
#include <QTimer>

using namespace live_private;

LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::uiThreadId = -1;
LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::metronomeThreadId = -1;
LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::audioThreadId = -1;
LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::midiThreadId = -1;

class TheMutex {
public:
    static TheMutex* me;
    QMutex LOCK;
    QMutex joinLock;
    int recurse;
    TheMutex() : LOCK(QMutex::Recursive), joinLock(QMutex::Recursive), recurse(0) {}
};

LIBLIVECORESHARED_EXPORT TheMutex* TheMutex::me=new TheMutex();
LIBLIVECORESHARED_EXPORT bool live::Object::ss_XRUN=false;

LIBLIVECORESHARED_EXPORT void live::Object::beginProc() {
    TheMutex::me->LOCK.lock();
}

LIBLIVECORESHARED_EXPORT void live::Object::beginAsyncAction() {
    //    TheMutex::me->joinLock.lock();
    TheMutex::me->LOCK.lock();
}

LIBLIVECORESHARED_EXPORT void live::Object::endProc() {
    TheMutex::me->LOCK.unlock();

    if (ss_XRUN) {
        qCritical() << "XRUN:: PROC guilty!";
        ss_XRUN = false;
    }
}

LIBLIVECORESHARED_EXPORT void live::Object::endAsyncAction() {
    TheMutex::me->LOCK.unlock();

    if (ss_XRUN) {
        qCritical() << "XRUN:: AsyncAction guilty!";
        ss_XRUN = false;
    }
    //    TheMutex::me->joinLock.unlock();
}

LIBLIVECORESHARED_EXPORT void live::Object::XRUN() {
    if (!TheMutex::me->LOCK.tryLock(1)) {
        ss_XRUN = true;
    } else {
        TheMutex::me->LOCK.unlock();
        qDebug() << "XRUN:: Mutex innocent.";
        ss_XRUN = false;
    }
}

void live::Object::aOut(const float *data, int chan, live::ObjectChain* p) {
    live::lthread::audio();
    for (int i=0;i<aConnections.size();i++) {
        Q_ASSERT(aConnections[i]->aInverseConnections.size());
        if (aConnections[i]->aInverseConnections.size()<=1||!aConnections[i]->s_allowMixer) aConnections[i]->aIn(data,chan,p);
        else aConnections[i]->mixer_process(data,chan);
    }
}

LIBLIVECORESHARED_EXPORT QMap<QString, live::ObjectPtr*>* live::Object::zombies=new QMap<QString, live::ObjectPtr*>;

live::Object::Object(QString cname, bool isPhysical, bool allowMixer) : s_name(cname), s_aOn(1), s_mOn(1), s_temp(1), s_allowMixer(allowMixer), aConnections(), mConnections() {
    if (isPhysical) {
        for (int i=0;i<zombies->values(cname).size();i++) {
            zombies->values(cname)[i]->restore(this);
        }
    }

    mixer_data[0]=mixer_data[1]=0;
    mixer_at[0]=mixer_at[1]=0;

    zombies->remove(cname);

    QTimer::singleShot(0,live::object::singleton(),SLOT(notify()));
}

live::Object::~Object() {
    live_async {
        kill();
        for (int i=0;i<2;i++) delete[] mixer_data[i];

        QTimer::singleShot(0,live::object::singleton(),SLOT(notify()));
    }
}

void live::Object::kill() {
    live_async {
        if (!SecretMidi::me) new SecretMidi;
        SecretMidi::me->mRemoveWithheld_object_dest(this);
        while (aConnections.size()) this->audioConnect(aConnections[0]);
        while (mConnections.size()) this->midiConnect(mConnections[0]);
        while (aInverseConnections.size()) aInverseConnections[0]->audioConnect(this);
        while (mInverseConnections.size()) mInverseConnections[0]->midiConnect(this);
        Q_ASSERT(!aConnections.size());
        Q_ASSERT(!mConnections.size());
        Q_ASSERT(!aInverseConnections.size());
        Q_ASSERT(!mInverseConnections.size());
        qDebug() << "Pointer zombified"<<s_name<<s_ptrList.size();
        for (int i=0;i<s_ptrList.size();i++) {
            s_ptrList[i]->obliviate();
            zombies->insertMulti(s_name,s_ptrList[i]);
        }
    }
}

void live::Object::hybridConnect(const live::ObjectPtr &b) {
    audioConnect(b);
    midiConnect(b);
}

void live::Object::midiConnect(const live::ObjectPtr &b) {
    doMidiConnect(b.data(),this);   //josh, you're fired! why is this backwards!?
}

void live::Object::audioConnect(const live::ObjectPtr& b) {
    doAudioConnect(b.data(),this);  //josh, you're fired! why is this backwards!?
}

void live::Object::mOut(const live::Event *data, live::ObjectChain* p) {
    if (!SecretMidi::me) SecretMidi::me=new SecretMidi;
    if (!live::lthread::isMidi() || (data->time.sec!=-1&&(data->time.toTime_ms()-5>live::midi::getTime_msec()))) {
        live::Event* x=new live::Event;
        *x=*data;
        SecretMidi::me->mWithhold(x,*p,this);    //now owner.
    } else for (int i=0;i<mConnections.size();i++) {
        live::Event*x=new live::Event;
        *x=*data;
        x->buddy=0;
        mConnections[i]->mIn(x,p);
        delete x;
    }
}

void live::Object::mOutverse(const live::Event *data, live::ObjectChain* p) {
    if (!SecretMidi::me) SecretMidi::me=new SecretMidi;
    if (!live::lthread::isMidi() || (data->time.sec!=-1&&(data->time.toTime_ms()-5>live::midi::getTime_msec()))) {
        live::Event* x=new live::Event;
        *x=*data;
        SecretMidi::me->mWithhold(x,*p,this,1);    //now owner.
    } else for (int i=0;i<mConnections.size();i++) {
        live::Event*x=new live::Event;
        *x=*data;
        x->buddy=0;
        mConnections[i]->mInverse(x,p);
        delete x;
    }
}


void live::doAudioConnect(const live::Object* al,const live::Object* bl) {
    live::Object* a=const_cast<live::Object*>(al);
    live::Object* b=const_cast<live::Object*>(bl);

    live_async {

        bool ok=1;
        for (int i=0;i<b->aConnections.size();i++) if (b->aConnections[i].data()==a) ok=0;

        if (ok) {
            a->newConnection();

            b->aConnections.push_back(a);
            a->aInverseConnections.push_back(b);
            a->mixer_resetStatus();
        } else {
            for (int i=0;i<b->aConnections.size();i++) {
                if (b->aConnections[i].data()==a) {
                    b->aConnections.removeAt(i);
                    break;
                }
            }
            for (int i=0;i<a->aInverseConnections.size();i++) {
                if (a->aInverseConnections[i].data()==b) {
                    a->aInverseConnections.removeAt(i);
                    a->mixer_resetStatus();
                    break;
                }
            }
            a->deleteConnection();

        }

    }

}

void live::doMidiConnect(const live::Object* al, const live::Object* bl) {
    live::Object* a=const_cast<live::Object*>(al);
    live::Object* b=const_cast<live::Object*>(bl);

    bool ok=1;
    for (int i=0;i<b->mConnections.size();i++) if (b->mConnections[i].data()==a) ok=0;

    live_async {
        if (ok) {
            b->mConnections.push_back(a);
            a->mInverseConnections.push_back(b);
        } else {
            for (int i=0;i<b->mConnections.size();i++) {
                if (b->mConnections[i].data()==a) {
                    b->mConnections.removeAt(i);
                    i--;
                }
            }
            for (int i=0;i<a->mInverseConnections.size();i++) {
                if (a->mInverseConnections[i].data()==b) {
                    a->mInverseConnections.removeAt(i);
                    i--;
                }
            }
        }
    }
}

void live::Object::mixer_resetStatus() {
    live_async {
        mixer_nframes=live::audio::nFrames();
        mixer_at[0]=mixer_at[1]=0;
        for (int i=0;i<2;i++) { if (mixer_data[i]) delete[] mixer_data[i]; mixer_data[i]=0; }
        if (aInverseConnections.size()>1) {
            for (int i=0;i<2;i++) mixer_data[i] = new float[mixer_nframes];
        }
    }
}

void live::ObjectPtr::obliviate() {
    live_async {
        live::ObjectPtr x=live::midi::getNull();
        x.data()->setTemporary(0);
        s_obj=x.data();
    }
}

bool live::ObjectPtr::restore(live::Object* a) {
    live_async {
        if (dynamic_cast<MidiNull*>(s_obj)) delete s_obj;
        s_obj=a;
        a->s_ptrList.push_back(this);
        live::Object::endAsyncAction();
    }
    return 1;
}

void audioConnect(live::Object&a,live::Object&b) {
    doAudioConnect(&b,&a);

}

void midiConnect(live::Object&a,live::Object&b) {
    doMidiConnect(&b,&a);
}

LIBLIVECORESHARED_EXPORT live::object* live::object::me=0;

QList<live::ObjectPtr> live::object::get(int flags) {
    QList<live::ObjectPtr> ret;
    live_async {
        me=me?me:new object;

        if ( (((flags&Input)||(flags&Output))&&!((flags&Input)&&(flags&Output))) && !(flags&NoRefresh) ) {   //Grr...
            //Debug<<"REFRESH!!";
            live::audio::refresh();
            live::midi::refresh();
        }

        for (int i=0;i<me->s_objects.size();i++) {
            if (!me->s_objects[i]) {   // cleanup
                me->s_objects.takeAt(i);
                i--;
                continue;
            }
            if ((flags&Instrument)&&me->s_objects[i]->isInput()&&me->s_objects[i]->isOutput()) {
                ret<<me->s_objects[i];
            } else if ( (!(flags&Input)&&!(flags&Output)) ||
                        (((flags&Input  && me->s_objects[i]->isInput()  && (!(flags&NotOutput) || !me->s_objects[i]->isOutput() )) ||
                          (flags&Output && me->s_objects[i]->isOutput() && (!(flags&NotInput)  || !me->s_objects[i]->isInput()  ))) &&
                         (!((flags&Input) && (flags&Output)) || (me->s_objects[i]->isInput()&&me->s_objects[i]->isOutput())))) {   // satisfies I/O requirements
                if ( (!(flags&Midi)&&!(flags&Audio)) ||
                     ((((flags&Audio) && me->s_objects[i]->isAudioObject() && (!(flags&NotMidi)  || !me->s_objects[i]->isMidiObject() )) ||
                       ((flags&Midi)  && me->s_objects[i]-> isMidiObject() && (!(flags&NotAudio) || !me->s_objects[i]->isAudioObject()))) &&
                      (!((flags&Audio) && (flags&Midi)) || (me->s_objects[i]->isAudioObject()&&me->s_objects[i]->isMidiObject())))) {   // satisfies A/M requirements
                    ret<<me->s_objects[i];
                }
            }
        }
    }
    return ret;
}

void live::object::clear(int flags) {
    live_async {
        QList<live::ObjectPtr> toClear=get(flags);
        while (toClear.size()) {
            me->s_objects.removeOne(toClear.takeFirst());
        }
    }
}

void live::object::set(live::ObjectPtr o) {
    me=me?me:new object;
    for (int i=0;i<me->s_objects.size();i++) {
        if (me->s_objects[i]==o) {
            return;
        }
    }
    me->s_objects.push_back(o);
}

live::ObjectPtr live::object::request(QString req, int flags) {
    QList<live::ObjectPtr> objList=get(flags);
    for (int i=0;i<objList.size();i++) {
        if (objList.at(i)->name()==req) {
            return objList[i];
        }
    }

    // no such object.
    QStringList b;
    b<<objList;
    bool ok=0;
    QString v;
    while (!ok) {
        Q_ASSERT(1);
        qFatal("Unimplemented functionality");
        //        v=QInputDialog::getItem(0L,"Choose a mapping","Creator Live requires a device which does not exist. Which device should take "+req+"'s role?",b,0,1,&ok);
    }
    Mapping*m=new Mapping(req,objList[b.indexOf(v)]);
    me->s_objects.push_back(m);
    return m;
}

live::ObjectPtr live::object::fetch(QString req, int flags) {
    QList<live::ObjectPtr> objList=get(flags);
    for (int i=0;i<objList.size();i++) {
        if (objList.at(i)->name()==req) {
            return objList[i];
        }
    }

    // no such object.
    return live::ObjectPtr(0);
}

void live::ObjectChain::push_back(live::Object *o) {
    push_back(live::ObjectPtr(o));
}

live::ObjectChain::ObjectChain() {
}

void live::ObjectPtr::detach() {
    if (!valid()) return;
    if (s_obj) {
        s_obj->s_ptrList.removeOne(this);
        if (!s_obj->s_ptrList.size()&&s_obj->isTemporary()) {
            if (s_obj->qoThis())
                s_obj->qoThis()->deleteLater();
            else
                delete s_obj;
        }
    }
    for (QMap<QString,ObjectPtr*>::iterator it=Object::zombies->begin(); it!=Object::zombies->end(); ++it) {
        if (it.value()==this) {
            it=Object::zombies->erase(it);
        }
    }
    s_obj=0;
}

bool live::ObjectPtr::valid() const {
    return s_obj?(!dynamic_cast<live_private::MidiNull*>(s_obj)):((bool)s_obj);
}
