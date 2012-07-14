/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LOBJECT_H
#define LOBJECT_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QDebug>
#include <QStringList>
#include <algorithm>

#include "liblivecore_global.h"

class Ambition;
class LiveWindow;

namespace live_private { class SecretAudio; }

namespace live {

class Event;
class ObjectPtr;
class Object;

class LIBLIVECORESHARED_EXPORT ObjectChain : public QList<ObjectPtr>
{
public:
    ObjectChain();
    template<class T> QList<T*> ofType()
    {
        QList<T*> tlist;
        for(int i=0;i<size();i++)
        {
            tlist.push_back(operator [](i));
        }
        return tlist;
    }
    inline void push_back(const ObjectPtr& o) { QList<ObjectPtr>::push_back(o); }
    void push_back(Object* o);
};

/**
 * Object is the base class for all objects which process MIDI data, audio data, or both.
 *
 * ALWAYS AND ONLY store an object as an ObjectPtr.
 * If the Object should exist after all ObjectPtrs are gone, setTemporary(false).
 *
 * If the Object is deleted, all ObjectPtrs pointing to that Object will be zombified, i.e., set to a null audio object
 */
class LIBLIVECORESHARED_EXPORT Object
{
    friend class ::LiveWindow;
    friend class ::Ambition;
public:
    /**
     * Create a Object with no connections and a descriptive name (like "Sequencer"), cname.
     *
     * Physical devices need to have unique names. Pointers to physical devices have a special feature.
     * Pointers to removed devices regain their identity when they are plugged in again.
     */
    Object(QString cname, bool isPhysical, bool allowMixer);

    /**
     * Destroys an object erasing all connections to and from the object.
     * In a pipeline { a >> b >> c } in which b is deleted, a and c will be isolated.
     */
    virtual ~Object();
    void kill();

    ///////////////////////////////////////
    // About audio data:
    // Audio data is sent first in chan 0 (left) then in chan 1 (right) as an array of AudioSys::nFrames() floats.

    /** Reimplement this function to handle audio data. If you call aIn from outside both an aIn or an mIn, use beginProc() and endProc().*/
    virtual void aIn (const float*data,int chan,ObjectChain&p) { p.push_back(this); aOut(data,chan,p); p.pop_back(); }
    /** In aIn(...) call aOut(...) when processing is finished. This must be called EXACTLY ONCE (i.e., it must be called, and not more than once) from every aIn function. */
    void aOut(const float*data,int chan,ObjectChain&p);
    /** Use the LIVE_HYBRID, LIVE_MIDI and LIVE_AUDIO macros. */
    virtual bool isAudioObject()=0;


    /** Reimplement this function to handle midi data. If you call mIn from outside both an aIn or an mIn, use {begin|end}Proc().*/
    virtual void mIn(const Event*data,ObjectChain&p) { p.push_back(this); mOut(data,p); p.pop_back(); }
    /** In mIn(...) call mOut(...) when processing is finished. This may be called as many times (including 0) inside an mIn.
     * If you send an event which is sent early it will be withheld until it is time for it to be sent on. */
    void mOut(const Event *data, ObjectChain &p);
    /** For effects, reimplement this function to undo what would have been done in mIn. For example, if mIn would raise data by a semitone, mInverse would lower it by a semitone.
     *  Then, output your result with 'mOutverse(data, p)'
     *
     *  Do not reimplement this function for plugins/effects which react to certain events without modifying them (for example, a sampler should not reimplement this function).
     *
     *  The purpose of this function is to allow for hybrid plugins. If, in a sequencer app the user changes a note for example, the sequencer would call mInverse for all nearby notes.
     *  The sequencer would then receive the audio again, and would replace the existing audio with the new audio.
     *
     *  Ambition-like classes should reimplement this function with 'mOut(data,p);'
     */
    virtual void mInverse(const Event*data,ObjectChain&p) { Q_ASSERT(p.size()); p.pop_back(); mOutverse(data,p); p.push_back(this); }
    /** See the note on mInverse. */
    void mOutverse(const Event* data,ObjectChain &p);
    /** Use the LIVE_HYBRID, LIVE_MIDI and LIVE_AUDIO macros. */
    virtual bool isMidiObject()=0;


    /** Use LIVE_INPUT, LIVE_OUTPUT, and LIVE_EFFECT macros. */
    virtual bool isInput()=0;

    /** Use LIVE_INPUT, LIVE_OUTPUT, and LIVE_EFFECT macros. */
    virtual bool isOutput()=0;


    /** Call this function before calling aIn(...) or mIn(...) outside of an mIn(...) or aIn(...) stack. Calling this more than once will not result in a deadlock, but forgetting to undo it will
      * cause cetain actions to never be performed.*/
    static void beginProc();
    /** Call this function when finished callinging aIn(...) or mIn(...) ouside of an mIn(...) or aIn(...) stack. Forgetting to do so will cause some actions to never be performed. */
    static void endProc();

    /** Call this function before you do something which could mess up audio or midi processing, such as changing connections. These actions must be efficient! Calling this from inside
      * {begin|end}proc() will result in a deadlock. Instead, take advantage of QtConcurrent::run(...). Calling this more than once _WILL_ result in a deadlock.
      */
    static void beginAsyncAction();
    /** Call this function when done doing things which cannot be done concurrently with audio/midi processing. */
    static void endAsyncAction();


    virtual QByteArray save() { qCritical()<<"Hybrid::save() called on "<<s_name; qCritical("Hybrid::save() called on a non-savable type!!"); return QByteArray(); }
    friend class SecretMidi;

    virtual void newConnection() {}
    virtual void deleteConnection() {}

    /** Connect both audio and midi, creating a pipeline (Note: except for real audio outputs, objects may usually only recieve one audio connection. they must recieve this one connection to function correctly. */
    void hybridConnect(const ObjectPtr &b);

    /** Connect just audio, creating a pipeline (Note: except for real audio outputs, objects may usually only recieve one audio connection. they must recieve this one connection to function correctly. )*/
    void audioConnect(const ObjectPtr& b);

    /** Connect just midi, creating a pipeline. (Note: some objects require audio for sync. midi inputs will all give ) */
    void midiConnect(const ObjectPtr &b);


    const QString& name() const { return s_name; }

    bool processingAudio() const { return s_aOn; }
    bool processingMidi() const { return s_mOn; }

    /** Temporary objects are deleted when there are no references to them. Default: true. */
    void setTemporary(bool temp) { s_temp=temp; }
    const bool& isTemporary() const { return s_temp; }

    /** Apps should reimplement this to avoid crashes from incorrect deletion. */
    virtual QObject* qoThis() { return 0; }

protected:
    QString s_name;
    bool s_aOn;
    bool s_mOn;
    bool s_temp;
    bool s_allowMixer;

    friend class ObjectPtr;
    friend class live_private::SecretAudio;
    friend class Ambition;
    friend void doAudioConnect(const Object* a, const Object* b);
    friend void doMidiConnect(const Object* a, const Object* b);
    QList<ObjectPtr*> s_ptrList;
    static QMap<QString, ObjectPtr*>* zombies;

    QList<ObjectPtr> aConnections;
    QList<ObjectPtr> mConnections;

    QList<ObjectPtr> aInverseConnections;
    QList<ObjectPtr> mInverseConnections;

    /***** MINI MIXER *****/
        float* mixer_data[2];
        int mixer_nframes;
        int mixer_at[2];
    void mixer_resetStatus();
    inline void mixer_clear() {
        Q_ASSERT(mixer_data[0]&&mixer_data[1]);
        for(int i=0;i<2;i++) std::fill(mixer_data[i],&mixer_data[i][mixer_nframes],0.0f);
    }

    inline void mixer_process(const float* data, int chan) {
        Q_ASSERT(mixer_data[chan]);
        for(int i=0;i<mixer_nframes;i++)
            mixer_data[chan][i]+=data[i];
        if(++mixer_at[chan]==aInverseConnections.size()) {
            ObjectChain p;
            aIn(mixer_data[chan],chan,p);
            mixer_clear();
            mixer_at[chan]=0;
        }
    }
};

void doAudioConnect(const Object* a, const Object* b);
void doMidiConnect(const Object* a, const Object* b);

#define LIVE_HYBRID public: virtual bool isAudioObject(){return 1;} virtual bool isMidiObject() {return 1;}
#define LIVE_MIDI public: virtual bool isAudioObject(){return 0;} virtual bool isMidiObject() {return 1;}
#define LIVE_AUDIO public: virtual bool isAudioObject(){return 1;} virtual bool isMidiObject() {return 0;}

#define LIVE_INPUT public: virtual bool isInput(){return 1;} virtual bool isOutput() {return 0;}
#define LIVE_OUTPUT public: virtual bool isOutput(){return 1;} virtual bool isInput() {return 0;}
#define LIVE_EFFECT public: virtual bool isInput(){return 1;} virtual bool isOutput() {return 1;}

#define NOSYNC QMutexLocker lock(&csMutex);Q_UNUSED(lock)

#define RELOADABLE(x) \
    public:QByteArray save(); \
    static x* load(const QByteArray&str);

/**
 * Never use Object*. Becomes null upon object deletion.
 */
class LIBLIVECORESHARED_EXPORT ObjectPtr
{
    Object* s_obj;
protected:
    void obliviate();
    bool restore(Object *a);
    friend class Object;
//    bool s_isInput, s_isOutput, s_isAudio, s_isMidi;
    ObjectPtr(QString,Object* o) : s_obj(o) {
        s_obj->s_ptrList.push_back(this);
    }

public:
    ObjectPtr(Object* o=0) : s_obj(o) {
        if(s_obj) {
            s_obj->s_ptrList.push_back(this);
//            s_isInput=s_obj->isInput();
//            s_isOutput=s_obj->isOutput();
//            s_isAudio=s_obj->isAudioObject();
//            s_isMidi=s_obj->isMidiObject();
        }
    }

    ~ObjectPtr() {
        detach();
    }

    void detach();
    ObjectPtr(const ObjectPtr& other) {
        s_obj=const_cast<ObjectPtr&>(other).data();
        if(other.data()) {
            s_obj->s_ptrList.push_back(this);
//            s_isInput=s_obj->isInput();
//            s_isOutput=s_obj->isOutput();
//            s_isAudio=s_obj->isAudioObject();
//            s_isMidi=s_obj->isMidiObject();
        }
    }

    ObjectPtr& operator=(Object* other) {
        detach();
        s_obj=other;
        if(other) {
            s_obj->s_ptrList.push_back(this);
//            s_isInput=s_obj->isInput();
//            s_isOutput=s_obj->isOutput();
//            s_isAudio=s_obj->isAudioObject();
//            s_isMidi=s_obj->isMidiObject();
        }
        return* this;
    }
    ObjectPtr& operator=(const ObjectPtr& other) {
        detach();
        s_obj=const_cast<ObjectPtr&>(other).data();
        if(other.data()) {
            s_obj->s_ptrList.push_back(this);
//            s_isInput=s_obj->isInput();
//            s_isOutput=s_obj->isOutput();
//            s_isAudio=s_obj->isAudioObject();
//            s_isMidi=s_obj->isMidiObject();
        }
        return* this;
    }

    bool operator!() const { return !s_obj; }
    bool operator==(const ObjectPtr&b) const { return data()==b.data(); }
    bool operator!=(const ObjectPtr&b) const { return data()!=b.data(); }
    bool operator<(const ObjectPtr&b) const { return data()<b.data(); }   //for QMap
    bool valid() const;

    Object* data() { return s_obj; }
    const Object* data() const { return s_obj; }
    const Object& operator*() const  { Q_ASSERT(s_obj); return *s_obj; }
    Object& operator*()              { Q_ASSERT(s_obj); return *s_obj; }
    const Object* operator->() const { Q_ASSERT(s_obj); return s_obj; }
    Object* operator->()             { Q_ASSERT(s_obj); return s_obj; }
};

enum StoreFlags {
    Input=1<<0,
    NotInput=1<<1,
    Output=1<<2,
    NotOutput=1<<3,

    InputOnly=Input|NotOutput,
    OutputOnly=Output|NotInput,

    App=Input|Output, //i.e., both input and output


    Midi=1<<4,
    NotMidi=1<<5,
    Audio=1<<6,
    NotAudio=1<<7,

    MidiOnly=Midi|NotAudio,
    AudioOnly=Audio|NotMidi,

    Hybrid = Midi|Audio,  //i.e., both midi and audio


    NoRefresh=1<<8,

    Instrument=1<<9
};

template <class T>
inline T cast(const Object *object)
{
    return dynamic_cast<T>(object);
}

template <class T>
inline T cast(ObjectPtr& object)
{
    return dynamic_cast<T>(object.data());
}

template <class T>
inline T cast_to_cptr(ObjectPtr& object)
{
    return dynamic_cast<T>(object.data());
}

}
class LiveWindow;

namespace live {
class LIBLIVECORESHARED_EXPORT object : public QObject
{
    Q_OBJECT
protected:
    QList<ObjectPtr> s_objects;
    static object* me;
    friend class Ribbon;
    friend class ::LiveWindow;
public:
    static object* singleton() { return me; }
    static QList<ObjectPtr> get(int flags); //i.e., Flags
    static void clear(int flags);   //i.e., Flags
    static void set(ObjectPtr o);
    static ObjectPtr request(QString req,int flags=0);  //This functions _will_ return something. If none exist, a mapping will be created.
signals:
    void stockChanged();
public slots:
    void notify() { Object::beginAsyncAction(); emit stockChanged(); Object::endAsyncAction(); }
};

}

template<typename T> bool operator >(QList<T> list, int a)
{
    return list.size() > a;
}

template<typename T> bool operator >(int a, QList<T> list)
{
    return a > list.size();
}

template<typename T> bool operator <(QList<T> list, int a)
{
    return list.size() < a;
}

template<typename T> bool operator <(int a, QList<T> list)
{
    return a < list.size();
}

template<class T> QStringList& operator<<(QStringList&a,const QList<T>&b)
{
    for(int i=0; i<b; i++)
    {
        a.push_back(b[i]->name());
    }
    return a;
}

template<typename T> bool logical_xor(const T& lhs, const T& rhs )
{
    return !( lhs && rhs ) && ( lhs || rhs );
}

#endif // LOBJECT_H
