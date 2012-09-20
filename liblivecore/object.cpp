/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "midisystem_p.h"

#include <live/audio>
#include <live/audiosecond>
#include <live/../audiosystem_p.h>
#include <live/liblivecore_global>
#include <live/mapping>
#include <live/midi>
#include <live/midievent>
#include <live/midieventcounter>
#include <live/object>

#include <QtConcurrentRun>
#include <QMutex>
#include <QTimer>

#if defined(__linux__)
#include <time.h>
#endif

using namespace live_private;

#ifndef __QNX__
LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::uiThreadId = -1;
LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::metronomeThreadId = -1;
LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::audioThreadId = -1;
LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::midiThreadId = -1;
#else
LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::uiThreadId = 0;
LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::metronomeThreadId = 0;
LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::audioThreadId = 0;
LIBLIVECORESHARED_EXPORT Qt::HANDLE live::lthread::midiThreadId = 0;
#endif

void live::lthread::assert_thread(const Qt::HANDLE& thread, const char* str) {
#ifndef __QNX__ // qnx doesn't have useful thread ids.
        if (thread != QThread::currentThreadId()) {
            std::cerr << "### Thread assertion error ###\n";
            std::cerr << "expected: " << str << "(" << QString::number(thread).toUtf8().data() << ")\n";
            QString is;
            if (QThread::currentThreadId() == uiThreadId)
                is = "user interface";
            if (QThread::currentThreadId() == metronomeThreadId)
                is = "metronome";
            if (QThread::currentThreadId() == audioThreadId)
                is = "audio";
            if (QThread::currentThreadId() == midiThreadId)
                is = "midi";

            std::cerr << "got: " << is.toUtf8().data() << "(" << QString::number(QThread::currentThreadId()).toUtf8().data() << ")\n";
            std::flush(std::cerr);
            TCRASH();
        }
#endif
}

void live::lthread::uiInit(bool really) {
    if (really) {
        if(live::AudioSecondBank::singleton) kill_kitten {
            delete live::AudioSecondBank::singleton;
            live::AudioSecondBank::singleton = new live::AudioSecondBank();
        } else live::AudioSecondBank::singleton = new live::AudioSecondBank();
    }
    uiThreadId = QThread::currentThreadId();
}

class TheMutex {
public:
    static TheMutex* me;
    QMutex LOCK;
    int recurse;
    TheMutex() : LOCK(QMutex::Recursive), recurse(0) {}
};

LIBLIVECORESHARED_EXPORT TheMutex* TheMutex::me=new TheMutex();
LIBLIVECORESHARED_EXPORT bool live::Object::ss_XRUN = false;
#if !defined(NDEBUG) && defined(__linux__)
LIBLIVECORESHARED_EXPORT std::vector<quint32> live::Object::s_asyncTime;
#endif

static QMutex x_asyncTime(QMutex::NonRecursive);
static QMutex x_mixers(QMutex::Recursive);

LIBLIVECORESHARED_EXPORT void live::Object::beginProc() {
    TheMutex::me->LOCK.lock();
#if !defined(NDEBUG) && defined(__linux__)
    live_mutex(x_asyncTime) {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        s_asyncTime.push_back(ts.tv_sec * 1000000000 + ts.tv_nsec);
    }
#endif
}

LIBLIVECORESHARED_EXPORT void live::Object::beginAsyncAction() {
    TheMutex::me->LOCK.lock();
#if !defined(NDEBUG) && defined(__linux__)
    live_mutex(x_asyncTime) {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        s_asyncTime.push_back(ts.tv_sec * 1000000000 + ts.tv_nsec);
    }
#endif
}

LIBLIVECORESHARED_EXPORT void live::Object::endProc(bool /*oversized*/) {
    TheMutex::me->LOCK.unlock();

    if (ss_XRUN) {
        qCritical() << "XRUN:: PROC guilty!";
        ss_XRUN = false;
    }
#if !defined(NDEBUG) && defined(__linux__)
    live_mutex(x_asyncTime) {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        quint32 l = ts.tv_sec * 1000000000 + ts.tv_nsec;
        if (l - s_asyncTime.back() > 1000000) {
            qCritical() << "THREADING ERROR: Audio thread killed" << (l - s_asyncTime.back()) << "kittens. The culprit has yet to be caught.\n";
//            if (live::audio::strictInnocentXruns) TCRASH();
        }
        s_asyncTime.pop_back();
    }
#endif
}

LIBLIVECORESHARED_EXPORT void live::Object::endAsyncAction(const char* file, int line) {
    TheMutex::me->LOCK.unlock();

#if !defined(NDEBUG) && defined(__linux__)
    live_mutex(x_asyncTime) {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        quint32 l = ts.tv_sec * 1000000000 + ts.tv_nsec;
        if (l - s_asyncTime.back() > 1000000) {
            qCritical() << "THREADING ERROR: Lock at" << file << "line" << line << "killed" << (l - s_asyncTime.back() ) << "kittens.";
            if (live::audio::strictInnocentXruns)
                TCRASH();
        }
        s_asyncTime.pop_back();
    }
#endif

    if (ss_XRUN) {
        qCritical() << "XRUN:: AsyncAction guilty?";
        ss_XRUN = false;
    }
}

LIBLIVECORESHARED_EXPORT void live::Object::XRUN() {
    if (!TheMutex::me->LOCK.tryLock(1)) {
        ss_XRUN = true;
    } else {
        TheMutex::me->LOCK.unlock();
        qDebug() << "XRUN:: Mutex innocent.";
        if (live::audio::strictInnocentXruns) {
            TCRASH();
        }
        ss_XRUN = false;
    }
}

void live::Object::aOut(const float *data, int chan, Object *prev) {
    live::lthread::assertAudio();
    for (std::set<live::ObjectPtr>::iterator it = aConnections.begin(); it != aConnections.end(); ++it) {
//    for (int i=0;i<aConnections.size();i++) {
        Q_ASSERT((*it)->aInverseConnections.size());
        if ((*it)->aInverseConnections.size()<=1||!(*it)->s_allowMixer) (*it)->aIn(data,chan,prev);
        else (*it)->mixer_process(data,chan);
    }
}

LIBLIVECORESHARED_EXPORT QMap<QString, live::ObjectPtr*>* live::Object::zombies=new QMap<QString, live::ObjectPtr*>;

live::Object::Object(QString cname, bool isPhysical, bool allowMixer, int chans)
  : x_ptr(QMutex::Recursive)
  , s_name(cname)
  , s_aOn(1)
  , s_mOn(1)
  , s_temp(1)
  , s_allowMixer(allowMixer)
  , s_ptrList()
  , aConnections()
  , mConnections()
  , aInverseConnections()
  , mInverseConnections()
  , s_isDestroying(false)
  , s_ec((cname == "Midi Event Counter") ? new live::MidiEventCounter() : 0)
  , mixer_nframes(-1)
  , s_chans(chans)
  {

    mixer_data = new float*[s_chans];
    mixer_at = new int[s_chans];

    mixer_nframes = live::audio::nFrames();

    if (isPhysical) {
        for (int i=0;i<zombies->values(cname).size();i++) {
            zombies->values(cname)[i]->restore(this);
        }
    }

    for (int i = 0; i < s_chans; ++i) {
        mixer_data[i] = new float[mixer_nframes];
        mixer_at[i]=0;
    }

    zombies->remove(cname);

    QTimer::singleShot(0,live::object::singleton(),SLOT(notify()));
}

live::Object::~Object() {
    s_isDestroying = 1;
    kill();
    for (int i=0;i<s_chans;i++) delete[] mixer_data[i];
    QTimer::singleShot(0,live::object::singleton(),SLOT(notify()));
}

void live::Object::kill() {
    kill_kitten {
        while (aConnections.size()) this->audioConnect(*aConnections.begin());
        while (mConnections.size()) this->midiConnect(*mConnections.begin());
        while (aInverseConnections.size()) (*aInverseConnections.begin())->audioConnect(this);
        while (mInverseConnections.size()) (*mInverseConnections.begin())->midiConnect(this);
        Q_ASSERT(!aConnections.size());
        Q_ASSERT(!mConnections.size());
        Q_ASSERT(!aInverseConnections.size());
        Q_ASSERT(!mInverseConnections.size());
    }
    if (!SecretMidi::me) new SecretMidi;
    SecretMidi::me->mClearEventsTo(this);
    for (QSet<ObjectPtr*>::iterator it = s_ptrList.begin(); it != s_ptrList.end(); ++it) {
        (*it)->obliviate();
        zombies->insertMulti(s_name,*it);
    }
}

void live::Object::hybridConnect(const live::ObjectPtr &b) {
    audioConnect(b);
    midiConnect(b);
}

void live::Object::midiConnect(const live::ObjectPtr &bl) {
    live::Object* a=const_cast<live::Object*>(this);
    live::Object* b=const_cast<live::Object*>(bl.data());
    a->mPanic();
    live::Object::beginProc(); {
        bool ok=1;
        for (std::set<live::ObjectPtr>::iterator it = a->mConnections.begin(); it != a->mConnections.end(); ++it)
            if ((*it).data()==b) ok=0;

        kill_kitten {
            if (ok) {
                a->mConnections.insert(b);
                b->mInverseConnections.insert(a);
            } else {
                for (std::set<live::ObjectPtr>::iterator it = a->mConnections.begin(); it != a->mConnections.end(); ++it) {
                    if((*it).data() == b)
                        a->mConnections.erase(it);
                }
                for (std::set<live::ObjectPtr>::iterator it = b->mInverseConnections.begin(); it != b->mInverseConnections.end(); ++it) {
                    if ((*it).data()==a) {
                        b->mInverseConnections.erase(it);
                    }
                }
            }
        }
    }
    live::Object::endProc();
}

void live::Object::audioConnect(const live::ObjectPtr& bl) {
    // We need both locks on al and bl.

    live::Object* a=const_cast<live::Object*>(this);
    live::Object* b=const_cast<live::Object*>(bl.data());

    while(1) {
        bool ok_a = b->x_ptr.tryLock();
        if(!ok_a) continue;
        bool ok_b = a->x_ptr.tryLock();
        if(!ok_b) {
            if(ok_a) b->x_ptr.unlock();
            continue;
        }
        break;
    }

    {

        bool ok=1;
        for (std::set<live::ObjectPtr>::iterator it = a->aConnections.begin(); it != a->aConnections.end(); ++it)
            if ((*it).data()==b) ok=0;

        if (ok) {
            b->newConnection();

            kill_kitten {
                a->aConnections.insert(b);
                b->aInverseConnections.insert(a);
                b->mixer_resetStatus();
            }
        } else {
            kill_kitten {
                for (std::set<live::ObjectPtr>::iterator it = a->aConnections.begin(); it != a->aConnections.end(); ++it) {
                    if ((*it).data()==b) {
                        a->aConnections.erase(it);
                        break;
                    }
                }
                for (std::set<live::ObjectPtr>::iterator it = b->aInverseConnections.begin(); it != b->aInverseConnections.end(); ++it) {
                    if ((*it).data()==a) {
                        b->aInverseConnections.erase(it);
                        break;
                    }
                }
            }
            b->mixer_resetStatus();

            b->deleteConnection();
        }
    }
    b->x_ptr.unlock();
    a->x_ptr.unlock();
}

void live::Object::mOut(const live::Event *data, live::ObjectChain* p, bool backwards) {
    if (!SecretMidi::me) SecretMidi::me=new SecretMidi;
    if (!live::lthread::isMidi() || (data->time.sec!=-1&&(data->time.toTime_ms()-5>live::midi::getTime_msec()))) {
        live::Event* x=new live::Event;
        *x=*data;
        SecretMidi::me->mWithhold(x,*p,this, backwards);    //now owner.
    } else {
        if(s_ec) s_ec->mIn(data,p);

        for(std::set<live::ObjectPtr>::iterator it = mConnections.begin(); it != mConnections.end(); ++it) {
            live::Event*x=new live::Event(*data);
            x->buddy=0;
            if (backwards)
                (*it)->mInverse(x,p);
            else
                (*it)->mIn(x,p);
            delete x;
        }
    }
}

void live::Object::mPanic() {
    if (!s_ec)
        return;

    QList<live::Event> ons = s_ec->flush();
    live::ObjectChain me;
    me.push_back(this);
    for (int i = 0; i < ons.size(); ++i)
        mOut(&ons[i], &me);
}

class MixerDataDestroyer {
    float* m_data;
public:
    MixerDataDestroyer(float* data)
      : m_data(data)
      {
    }

    void go() {
        delete[] m_data;
    }
};

void live::Object::mixer_resetStatus() {
//    live_mutex(x_mixers) {
        for (int i = 0; i < s_chans; ++i)
            mixer_at[i] = 0;
//    }
}

void live::Object::mixer_process(const float* data, int chan) {
    live_mutex(x_mixers) {
        Q_ASSERT(mixer_data[chan]);
        for(unsigned i=0;i<mixer_nframes;i++)
            mixer_data[chan][i]+=data[i];
        if(++mixer_at[chan]==aInverseConnections.size()) {
            aIn(mixer_data[chan],chan, 0);
            mixer_clear();
            mixer_at[chan]=0;
        }
    }
}

uint live::qHash(const live::ObjectPtr &p) {
    return ::qHash((quintptr)p.data());
}

void live::ObjectPtr::obliviate() {
    kill_kitten {
        live::ObjectPtr x=live::midi::null();
        x.data()->setTemporary(0);
        s_obj=x.data();
    }
}

bool live::ObjectPtr::restore(live::Object* a) {
    live_mutex(a->x_ptr) {
        if (dynamic_cast<MidiNull*>(s_obj)) delete s_obj;
        s_obj=a;
        a->s_ptrList.insert(this);
    }
    return 1;
}

LIBLIVECORESHARED_EXPORT live::object* live::object::me=0;

QList<live::ObjectPtr> live::object::get(int flags) {
    QList<live::ObjectPtr> ret;
    me=me?me:new object;

    if ( (((flags&Input)||(flags&Output))&&!((flags&Input)&&(flags&Output))) && !(flags&NoRefresh) ) {   //Grr...
        live::audio::refresh();
        live::midi::refresh();
    }

    /* FIXME */ {
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
    kill_kitten {
        QList<live::ObjectPtr> toClear=get(flags);
        while (toClear.size()) {
            me->s_objects.removeOne(toClear.takeFirst());
        }
    }
}

void live::object::set(live::ObjectPtr o) {
    kill_kitten {
        me=me?me:new object;
        for (int i=0;i<me->s_objects.size();i++) {
            if (me->s_objects[i]==o) {
                return;
            }
        }
        me->s_objects.push_back(o);
    }
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
    push_back(live::ObjectPtr(o, false));
}

live::ObjectChain::ObjectChain()
  : s_identity()
  {
}

void live::ObjectPtr::detach() {
    if (!s_obj)
        return;

    live_mutex(s_obj->x_ptr) {
        s_obj->s_ptrList.remove(this);
    }

    // FIXME: we should assert that we're not currently killing kittens.
    live::Object* toDelete = 0;

    live_mutex(s_obj->x_ptr) {
        if (!valid()) return;
        if (s_obj) {
            if (!s_obj->s_ptrList.size()&&s_obj->isTemporary()) { // FIXME
                if (s_obj->qoThis() && !s_obj->s_isDestroying)
                    s_obj->qoThis()->deleteLater();
                else if (!s_obj->s_isDestroying)
                    toDelete = s_obj;
            }
        }
        for (QMap<QString,ObjectPtr*>::iterator it=Object::zombies->begin(); it!=Object::zombies->end(); ++it) {
            if (it.value()==this) {
                it=Object::zombies->erase(it);
            }
        }
        s_obj=0;
    }
    delete toDelete;
}

bool live::ObjectPtr::valid() const {
    return s_obj?(!dynamic_cast<live_private::MidiNull*>(s_obj)):((bool)s_obj);
}

live::Connection::Connection(live::ObjectPtr ca, live::ObjectPtr cb, const ConnectionType &ct)
  : a(ca)
  , b(cb)
  , t(ct)
  { connect();
    qDebug() << "[CONNECT]"<<a->name() << "at" << a.data()<< "->"<<b->name() << "at" << b.data() << "| type:"<<((t==0)?"Audio":(t==1?"Midi":"Hybrid"));
}

live::Connection::~Connection()
{
    qDebug() << "[DISCONNECT]"<<a->name() << "at" << a.data() << "->"<<b->name() << "at" << b.data() << "| type:"<<((t==0)?"Audio":(t==1?"Midi":"Hybrid"));
    connect();
}

void live::Connection::connect()
{
    if (!a || !b)
        return;

    if(t==AudioConnection)
    {
        a->audioConnect(b);
    }
    else if(t==MidiConnection)
    {
        a->midiConnect(b);
    }
    else if(t==HybridConnection)
    {
        a->hybridConnect(b);
    }
}
