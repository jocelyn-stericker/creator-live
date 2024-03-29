/*****************************************************
core_dotlive.cpp

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield    (joshua@nettek.ca)

                 All rights reserved.

 To ensure save-load compatability, whenever possible
 saving and loading code is kept the same. It's a bit
 ugly, but _probably_ safer than writing two distinct
 functions in each class. In each class' header file,
 comments in the form XXX indicate the order in which
 properties are written here. You can thank my OCD...
*****************************************************/

#include <live/app>
#include <live/anabeat>
#include <live/ambition>
#include <live/asyncconnect>
#include <live/audio>
#include <live/audiotrack>
#include <live/chord>
#include <live/keysignature>
#include <live/object>
#include <live/time>
#include <live/mapping>
#include <live/metronome>
#include <live/midibinding>
#include <live/midievent>
#include <live/midifilter>
#include <live/midi>
#include <live/miditrack>
#include <live/pitch>
#include <live/songsystem>
#include <live/timesignature>
#include <live/variantbinding>
#include <live/appinterface>

#ifndef CORE_DOTLIVE_CPP
#define CORE_DOTLIVE_CPP
#define LOAD
#include "core_dotlive.cpp"
#undef LOAD
#undef BEGIN
#undef Im_LOAD
#undef Im_SAVE
#undef IO
#undef _THIS
#undef RETURN
#undef verify
#define SAVE
#endif

#ifdef LOAD
#define BEGIN QDataStream ret(str); QString xqstring; qint32 xint32; qint16 xint16; quint8 xchar; bool xbool; QByteArray xba; Q_UNUSED(xint16); Q_UNUSED(xchar); Q_UNUSED(xint32)
#define Im_LOAD 1
#define Im_SAVE 0
#define IO >>
#define _THIS NULL
#define RETURN return x;
template<typename T> bool verify1(QDataStream&ret,T chk)
{
    T b=chk;
    ret IO b;
    if (b!=chk)
    {
        qCritical()<<"Found"<<b;
        qCritical()<<"Should be"<<chk;
        Q_ASSERT(0);
    }
    return b==chk;
}
#define verify(x,y) verify1(x,y)
#endif
#ifdef SAVE
#define BEGIN QByteArray str; QDataStream ret(&str,QIODevice::WriteOnly); QString xqstring; qint32 xint32; qint16 xint16; quint8 xchar; bool xbool; QByteArray xba; Q_UNUSED(xint16); Q_UNUSED(xchar); Q_UNUSED(xint32)
#define Im_LOAD 0
#define Im_SAVE 1
#define IO <<
#define _THIS this
#define RETURN return str

template<typename T> bool verify2(QDataStream&ret,T chk)
{
    T b=chk;
    ret IO b;
    if (b!=chk)
    {
        qCritical()<<"Found"<<b;
        qCritical()<<"Should be"<<chk;
        Q_ASSERT(0);
    }
    return b==chk;
}
#define verify(x,y) verify2(x,y)
#endif


#ifndef TYPEm_
#define TYPEm_
#define P_CHAR(var) \
    do { \
    xchar=var; \
    ret IO xchar; \
    var=xchar; \
    } while (0)
#define P_INT16(var) \
    do { \
    xint16=var; \
    ret IO xint16; \
    var=xint16; \
    } while (0)
#define P_INT32(var) \
    do { \
    xint32=var; \
    ret IO xint32; \
    var=xint32; \
    } while (0)

#define P_BOOL(var) \
    do { \
    xbool=var; \
    ret IO xbool; \
    var=xbool; \
    } while (0)

#define P_QSTRING(var) \
    do { \
    xqstring=var; \
    ret IO xqstring; \
    var=xqstring; \
    } while (0)
#endif

using namespace live;

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// Time //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
Time* Time::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray Time::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint16);
    Q_UNUSED(xchar);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN Time"));

    /*002*/
    (verify(ret,(qint32 )110705));

    //////////////////////////////////////////////////////////////////

    Time*x=Im_SAVE?_THIS:new Time;

    //////////////////////////////////////////////////////////////////

    /*003*/
    P_INT32(x->sec);

    /*004*/
    P_INT32(x->nsec);

    //////////////////////////////////////////////////////////////////

    /*005*/
    (verify(ret,(QString)"END Time"));

    RETURN;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// live::Event /////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
live::Event* live::Event::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray live::Event::save()
#endif
{
    BEGIN;
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN live::Event"));

    /*002*/
    (verify(ret,(qint32 )110806));

    //////////////////////////////////////////////////////////////////

    live::Event*x=Im_SAVE?_THIS:new live::Event;

    //////////////////////////////////////////////////////////////////

    /*003*/
    P_INT16(x->message);

    /*004*/
    P_INT16(x->data1);

    /*005*/
    P_INT16(x->data2);

    //////////////////////////////////////////////////////////////////

    /*006*/
    if (Im_SAVE)
    {
        xba=x->time.save();
    }
    ret IO xba;
    if (Im_LOAD)
    {
        x->time = *Time::load(xba);
    }

    //////////////////////////////////////////////////////////////////

    /*007*/
    P_INT16(x->flareId);

    /*008*/ (verify(ret,(QString)"END live::Event"));

    RETURN;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// TIME SIGNATURE ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
TimeSignature* TimeSignature::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray TimeSignature::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN TimeSignature"));

    /*002*/
    (verify(ret,(qint32 )110402));

    //////////////////////////////////////////////////////////////////

    TimeSignature*x=Im_SAVE?_THIS:NULL;
    x=Im_SAVE?x:new TimeSignature;

    /*003*/
    P_INT32(x->numerator);
    /*004*/
    P_INT32(x->denomenator);


    //////////////////////////////////////////////////////////////////

    /*005*/
    (verify(ret,(QString)"END TimeSignature"));

    RETURN;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// PITCH /////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
Pitch* Pitch::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray Pitch::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN Pitch"));

    /*002*/
    (verify(ret,(qint32 )110507));

    //////////////////////////////////////////////////////////////////

    Pitch*x=Im_SAVE?_THIS:NULL;
    x=Im_SAVE?x:new Pitch;

    /*003*/
    P_CHAR(x->m_root);

    /*004*/
    int accidental=x->m_accidental;
    P_INT32(accidental);
    x->m_accidental=static_cast<Pitch::Accidental>(accidental); //!?!*!@#@?

    /*005*/
    P_INT32(x->m_octave);


    //////////////////////////////////////////////////////////////////

    /*006*/
    (verify(ret,(QString)"END Pitch"));

    RETURN;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// KeySig /////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
KeySignature* KeySignature::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray KeySignature::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN KeySignature"));

    /*002*/
    (verify(ret,(qint32 )110507));

    //////////////////////////////////////////////////////////////////

    KeySignature*x=Im_SAVE?_THIS:NULL;
    x=Im_SAVE?x:new KeySignature;

    /*003*/
    P_CHAR(x->m_root);

    /*004*/
    qint32 accidental=x->m_accidental;
    P_INT32(accidental);
    x->m_accidental=static_cast<Pitch::Accidental>(accidental);

    /*005*/
    P_INT32(x->m_octave);

    /*006*/
    qint32 tonality=x->m_tonality;
    P_INT32(tonality);
    x->m_tonality=static_cast<KeySignature::Tonality>(tonality);


    //////////////////////////////////////////////////////////////////

    /*007*/
    (verify(ret,(QString)"END KeySignature"));

    RETURN;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// SpecificChord ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
SpecificChord* SpecificChord::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray SpecificChord::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN SpecificChord"));

    /*002*/
    (verify(ret,(qint32 )110507));

    //////////////////////////////////////////////////////////////////

    SpecificChord*x=Im_SAVE?_THIS:new SpecificChord();

    int count=x?x->count():0;
    P_INT32(count);   /* NOT A PROPERTY */
    for (int i=0;i<count;i++)
    {

        if (Im_SAVE)
        {
            xba=(*x)[i].save();
        }
        /*006*/ ret IO xba;
        if (Im_LOAD)
        {
            Pitch* p=Pitch::load(xba);
            x->push_back(*p);
            delete p;
        }

    }
    /*003*/


    //////////////////////////////////////////////////////////////////

    /*007*/
    (verify(ret,(QString)"END GuitarChord"));

    RETURN;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// AudioSys //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
void live::audio::loadVerify(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray audio::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN AudioSys VERIFY"));

    /*002*/
    (verify(ret,(qint32 )110507));

    //////////////////////////////////////////////////////////////////

    int sampleRate=audio::sampleRate();
    ret IO sampleRate;  /* NOT A PROPERTY */
    if (Im_LOAD)
    {
        if (sampleRate!=audio::sampleRate())
        {
//            QMessageBox::critical(0,"Bad sample rate",
//                                  "The file you are loading requires a "
//                                  "sample rate of "+QString::number(sampleRate) +
//                                  "but it is set at "+QString::number(audio::sampleRate())+"! "+
//                                  "Please adjust your settings!",QMessageBox::Abort,QMessageBox::NoButton);
            //qFatal( (QString("The file you are loading requires a ") + 
              //      QString("sample rate of ")+QString::number(sampleRate) +
                //    QString("but it is set at ")+QString::number(audio::sampleRate())+QString("! ")+
                  //  QString("Please adjust your settings!")).toLatin1());
            qFatal("error.");
            Q_ASSERT(0);
        }
    }

    //////////////////////////////////////////////////////////////////

    quint32 nFrames=audio::nFrames();
    ret IO nFrames;  /* NOT A PROPERTY */
    if (Im_LOAD)
    {
        if (nFrames!=audio::nFrames())
        {
//            QMessageBox::critical(0,"Bad value for nFrames",
//                                  "The file you are loading requires an "
//                                  "nFrame constant of "+QString::number(nFrames) +
//                                  "but it is set at "+QString::number(audio::nFrames())+"! "+
//                                  "Please adjust your settings!",QMessageBox::Abort,QMessageBox::NoButton);
              qFatal("error.");
            Q_ASSERT(0);
        }
    }

    //////////////////////////////////////////////////////////////////

    for (int i=0;i<2;i++)
    {
        int incount=(i?object::get(live::AudioOnly|live::InputOnly):object::get(live::AudioOnly|live::OutputOnly)).size();
        ret IO incount;  /* NOT A PROPERTY */
        if (Im_SAVE)
        {
            QList<live::ObjectPtr> inlist=i?object::get(live::AudioOnly|live::InputOnly):object::get(live::AudioOnly|live::OutputOnly);
            for (int i=0;i<inlist.size();i++)
            {
                QString xstr=inlist[i]->name();
                ret IO xstr;
            }
        }
        else
        {
            QList<live::ObjectPtr> inlist=i?object::get(live::AudioOnly|live::InputOnly):object::get(live::AudioOnly|live::OutputOnly);
            for (int i=0;i<incount;i++)
            {
                QString xstr;
                ret IO xstr;

                bool ok=0;
                for (int j=0;j<inlist.size();j++)
                {
                    if (inlist[i]->name()==xstr)
                    {
                        ok=1;
                        break;
                    }
                }
                if (!ok)
                {
//                    QMessageBox::critical(0,"Missing "+QString(i?"Input":"Output"),
//                                          "The file you are loading requires an "+QString(i?"input":"output")+" named"+
//                                          xstr + "but it does not exist "+
//                                          "Please adjust your settings!",QMessageBox::Abort,QMessageBox::NoButton);
			qFatal("error.");
                    //qCritical(QString("The file you are loading requires an "+QString(i?"input":"output")+" named"+
                    //          xstr + "but it does not exist "+
                    //          "Please adjust your settings!").toLatin1());
                    Q_ASSERT(0);
                }
            }
        }
    }

    /*016*/
    (verify(ret,(QString)"END AudioSys VERIFY"));

#ifdef LOAD
    return;
#else
    RETURN;
#endif

}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// MIDI FILTER ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


#ifdef LOAD
MidiFilter* MidiFilter::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray MidiFilter::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN MidiFilter"));

    /*002*/
    (verify(ret,(qint32 )110707));


    //////////////////////////////////////////////////////////////////


    MidiFilter*x=Im_SAVE?_THIS:new MidiFilter();

    /*003*/

    for (int i=0;i<200;i++)
    {
        P_INT32(x->b_filterForNote[i]);
    }

    //////////////////////////////////////////////////////////////////

    /*004*/ (verify(ret,(QString)"END MidiFilter"));

    RETURN;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// SongSys ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


#ifdef LOAD
song* song::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray song::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN SongSys"));

    /*002*/
    (verify(ret,(qint32 )110707));


    //////////////////////////////////////////////////////////////////


    song*x=Im_SAVE?_THIS:new song();


    //////////////////////////////////////////////////////////////////

    if (Im_SAVE)
    {
        xba=x->metronome->save();
    }
    /*003*/ ret IO xba;
    if (Im_LOAD)
    {
        x->metronome = Metronome::load(xba);
    }

    //////////////////////////////////////////////////////////////////

    if (Im_SAVE)
    {
        xba=x->keySignature->save();
    }
    /*004*/ ret IO xba;
    if (Im_LOAD)
    {
        x->keySignature = KeySignature::load(xba);
    }

    //////////////////////////////////////////////////////////////////

    Q_ASSERT(MidiFilter::sm_loadState==-1);

    qint32 mapsize=x->midiMaps.count();
    /*005A*/ ret IO mapsize;

    //////////////////////////////////////////////////////////////////

    /*005B*/
    midi::refresh();

    for (int i=0;i<mapsize;i++)
    {
        qint32 ns=Im_SAVE?x->midiMaps.keys()[i]:0;
        /*005B_1*/ P_INT32(ns);
        qint32 ni=Im_SAVE?x->midiMaps.values()[i]->m_id:-1;
        /*005B_2*/ P_INT32(ni);
        if (Im_LOAD)
        {
            Q_ASSERT(ni!=-1&&ni<MidiFilter::_u.size());
            x->midiMaps.insertMulti(ns,MidiFilter::_u[ni]);
        }
    }

    //////////////////////////////////////////////////////////////////

    /*006*/
    P_QSTRING(x->name);

    //////////////////////////////////////////////////////////////////


    /*007*/ P_QSTRING(x->songName);

    /*008*/ (verify(ret,(QString)"END SongSys"));

    RETURN;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// METRONOME /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


#ifdef LOAD
Metronome* Metronome::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray Metronome::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN Metronome"));

    /*002*/
    (verify(ret,(qint32 )110402));


    //////////////////////////////////////////////////////////////////


    Metronome*x=Im_SAVE?_THIS:NULL;

    float xbpm = Im_SAVE?x->b_bpm:0.0f;
    /*003*/
    ret IO xbpm;
    float xppq = Im_SAVE?x->m_ppq:0.0f;
    /*004*/
    ret IO xppq;

    x=Im_SAVE?x:new Metronome(TimeSignature(4,4),xbpm,xppq);
    //We change the time signature below...


    //////////////////////////////////////////////////////////////////


    xbool=x->m_isActive;
    /*005*/
    ret IO xbool;
    if (Im_LOAD&&xbool)
    {
        x->start();
    }


    //////////////////////////////////////////////////////////////////


    if (Im_SAVE)
    {
        xba=x->m_ts.save();
    }
    /*006*/ ret IO xba;
    if (Im_LOAD)
    {
        TimeSignature* newts=TimeSignature::load(xba);
        x->m_ts = *newts;
        delete newts;
    }

    //////////////////////////////////////////////////////////////////

    /*007*/ (verify(ret,(QString)"END Metronome"));

    RETURN;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// Ambition //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
Ambition* Ambition::load(const QByteArray &str)
#endif
#ifdef SAVE
QByteArray Ambition::save()
#endif
{
    BEGIN;
    Q_UNUSED(xbool);
    Q_UNUSED(xint32);
    Q_UNUSED(xba);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN Ambition"));

    /*002*/
    (verify(ret,(qint32 )110905));


    //////////////////////////////////////////////////////////////////

    Ambition* x=Im_SAVE?_THIS:NULL;
#ifdef LOAD
    QString cin_str,cout_str,cout_loopback_str;
    int cchain_size;
    QStringList cchain_str;

    ret IO cin_str;
    ObjectPtr cinput=object::request(cin_str,InputOnly);

    ret IO cout_str;
    ObjectPtr coutput=object::request(cout_str,Output);

    ret IO cout_loopback_str;
    ObjectPtr loopback=(cout_loopback_str=="__NO_LOOPBACK__")?0:object::request(cout_loopback_str,Output);

    ret IO cchain_size;
    ObjectChain chain;
    for (int i=0;i<cchain_size;i++)
    {
        QByteArray name;
        ret IO name;
        ret IO xba;
        chain.push_back(app::loadBackend(name,xba));
    }
    x=new Ambition(cinput,chain,coutput,loopback);

#else
    Q_UNUSED(x);
    ret IO this->m_input->name();
    ret IO this->m_output->name();
    if (m_loopbackOut.valid())
    {
        ret IO this->m_loopbackOut->name();
    }
    else
    {
        ret IO (QString)"__NO_LOOPBACK__";
    }
    ret IO this->m_chain.size();
    for (int i=0;i<this->m_chain.size();i++)
    {
        xba = x->at(i)->name().toLatin1();
        ret IO xba;
        ret IO x->at(i)->save();
    }
#endif

    /*003*/
    (verify(ret,(QString)"END Ambition"));

    RETURN;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// AnaBeat ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
AnaBeat* AnaBeat::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray AnaBeat::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xqstring);
    Q_UNUSED(xbool);
    Q_UNUSED(xba);
    AnaBeat* x=Im_SAVE?_THIS:new AnaBeat();
    Q_UNUSED(x);    //in saving
    RETURN;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// AUDIO SECOND ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
AudioSecond* AudioSecond::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray AudioSecond::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN AudioSecond"));

    /*002*/
    (verify(ret,(qint32 )110711));


    //////////////////////////////////////////////////////////////////


    AudioSecond*x=0;
    x = Im_SAVE?_THIS:NULL;
    /*003*/
    qint32 rate=audio::sampleRate();  //qint32
    ret IO rate;
    Q_ASSERT(rate==audio::sampleRate());

    /*004*/
    qint32 offset=Im_SAVE?x->m_sampleOffset:0;
    ret IO offset;
    x = Im_SAVE?x:new AudioSecond(offset);
    /*005*/
    for (int i=0;i<audio::sampleRate();i++)
    {
        ret IO x->m_data[i];        //float
    }

    /*006*/
    (verify(ret,(QString)"END AudioSecond"));

    RETURN;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// AUDIO CONTAINER ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
AudioContainer* AudioContainer::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray AudioContainer::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN AudioContainer"));

    /*002*/
    (verify(ret,(qint32 )110711));


    //////////////////////////////////////////////////////////////////


    AudioContainer*x=0;
    x = Im_SAVE?_THIS:new AudioContainer();
    /*003*/
    qint32 rate=audio::sampleRate();  //qint32
    ret IO rate;
    Q_ASSERT(rate==audio::sampleRate());

    /*004*/
    qint32 listSize=Im_SAVE?x->m_data.size():0;
    ret IO listSize;
    for (int i=0;i<listSize;i++)
    {
        xba=Im_SAVE?
                    (x->m_data[i]?x->m_data[i]->save():QByteArray("NULL")):
                    QByteArray();

        ret IO xba;
        if (Im_LOAD)
        {
            x->m_data.push_back((xba==QByteArray("NULL"))?0:AudioSecond::load(xba));
        }
    }
    x->m_dataSize=x->m_data.size();

    /*005*/
    (verify(ret,(QString)"END AudioContainer"));

    RETURN;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// AUDIO TRACK ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
AudioTrack* AudioTrack::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray AudioTrack::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN AudioTrack"));

    /*002*/
    (verify(ret,(qint32 )110711));


    //////////////////////////////////////////////////////////////////


    AudioTrack*x=0;
    if (Im_SAVE)
    {
        x = _THIS;
        /*003*/
        ret IO x->m_chans;
    }
    else if (Im_LOAD)
    {
        qint32 cchans;

        /*003*/
        ret IO cchans;
        x = new AudioTrack(cchans);
    }
    else
    {
        qFatal("Invalid IO type");
    }


    //////////////////////////////////////////////////////////////////


    /*004*/
    xint32=x->volume();
    ret IO xint32;
    x->setVol(xint32);


    //////////////////////////////////////////////////////////////////


    /*005*/
    xint32=x->pan();
    ret IO xint32;
    if (Im_LOAD&&(x->m_chans==2))
    {
        x->setPan(xint32);
    }


    //////////////////////////////////////////////////////////////////


    /*006*/
    xbool=x->isRecord();
    ret IO xbool;
    if (Im_LOAD&&xbool)
    {
        x->startRecord();
    }


    //////////////////////////////////////////////////////////////////


    /*007*/
    xbool=x->isOverdub();
    ret IO xbool;
    if (Im_LOAD&&xbool)
    {
        x->startOverdub();
    }


    //////////////////////////////////////////////////////////////////


    /*008*/
    xbool=x->isPlay();
    ret IO xbool;
    if (Im_LOAD&&xbool)
    {
        x->startPlayback();
    }


    //////////////////////////////////////////////////////////////////


    /*009*/
    xbool=x->isMute();
    ret IO xbool;
    if (Im_LOAD&&xbool)
    {
        x->startMute();
    }


    //////////////////////////////////////////////////////////////////

    /*010*/
    float f =x->pos();
    ret IO f;
    if (Im_LOAD)
    {
        x->setPos(f);
    }


    //////////////////////////////////////////////////////////////////


    //    /*011*/ (verify(ret,(QString)"BEGIN AudioTrack raw data"));

    //    /*012*/
    //    for (int i=0; i<x->m_chans; i++)
    //    {
    //        for (int j=0; j<x->m_dataEnd; j++)
    //        {
    //            float a=x->m_data[i][j];
    //            ret IO a;
    //            if (Im_LOAD)
    //            {
    //                x->m_data[i][j]=a;
    //            }
    //        }
    //    }

    /*011*/
    for (int i=0;i<x->m_chans;i++)
    {
        xba=Im_SAVE?x->m_container[i]->save():xba;
        ret IO xba;
        if (Im_LOAD)
        {
            delete x->m_container[i];
            x->m_container[i]=AudioContainer::load(xba);
        }
    }


    //////////////////////////////////////////////////////////////////


    /*012*/
    (verify(ret,(QString)"END AudioTrack"));

    RETURN;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// MIDITRACK ////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOAD
MidiTrack* MidiTrack::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray MidiTrack::save()
#endif
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN MidiTrack"));

    /*002*/
    (verify(ret,(qint32 )110707));


    //////////////////////////////////////////////////////////////////


    MidiTrack*x=Im_SAVE?_THIS:new MidiTrack;
    Q_UNUSED(x);


    //////////////////////////////////////////////////////////////////

    /*003*/ P_INT32(x->b_curPos);
    /*004*/ P_INT32(x->b_lastPos);
    /*005*/ P_INT32(x->b_recStart);
    /*006*/ P_INT32(x->b_systemTimeStart);

    //////////////////////////////////////////////////////////////////


    /*009*/
    xbool=x->isRecord();
    ret IO xbool;
    if (Im_LOAD&&xbool)
    {
        x->startRecord();
    }


    //////////////////////////////////////////////////////////////////


    /*010*/
    xbool=x->isOverdub();
    ret IO xbool;
    if (Im_LOAD&&xbool)
    {
        x->startOverdub();
    }


    //////////////////////////////////////////////////////////////////


    /*011*/
    xbool=x->isPlay();
    ret IO xbool;
    if (Im_LOAD&&xbool)
    {
        x->startPlayback();
    }


    //////////////////////////////////////////////////////////////////


    /*012*/
    xbool=x->isMute();
    ret IO xbool;
    if (Im_LOAD&&xbool)
    {
        x->startMute();
    }


    //////////////////////////////////////////////////////////////////


    /*013A*/ (verify(ret,(QString)"BEGIN MidiTrack data"));

    /*013B*/
    xint32=x->m_data->size();
    ret IO xint32;

    /*013C*/
    for (int i=0; i<xint32; i++)
    {
        xba=Im_SAVE?(*x->m_data)[i]->save():QByteArray();
        ret IO xba;
        if (Im_LOAD)
        {
            x->m_data->push_back(Event::load(xba));
        }
    }

    /*013D*/
    verify(ret,(QString)"END MidiTrack data");

    //////////////////////////////////////////////////////////////////


    /*014*/ P_INT32(x->mTrack_id);

    if (Im_LOAD)
    {
        lastId=qMax(lastId,x->mTrack_id);
    }

    /*015*/
    (verify(ret,(QString)"END MidiTrack"));

    RETURN;
}

#ifdef LOAD
MidiEventCounter* MidiEventCounter::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray MidiEventCounter::save()
#endif
{
    BEGIN;
    Q_UNUSED(xbool);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN MidiEventCounter"));

    /*002*/
    (verify(ret,(qint32 )110707));

    //////////////////////////////////////////////////////////////////

    MidiEventCounter*x=Im_SAVE?_THIS:new MidiEventCounter;

    //////////////////////////////////////////////////////////////////

    /*003*/
    for (int i=0;i<200;i++)
    {
        P_INT32(x->on[i]);
    }

    //////////////////////////////////////////////////////////////////

    /*004*/ (verify(ret,(QString)"END Event"));

    RETURN;
}

#ifdef LOAD
ObjectPtr app::loadBackend(const QByteArray& name, const QByteArray&str)
{
    BEGIN;
    Q_UNUSED(xint32);
    Q_UNUSED(xbool);

    for (int i=0;i<appNames().size();i++) {
        qDebug() << "Checking" << name << "against" << appNames()[i];
        if (appNames()[i]==name) {
            ObjectPtr optr=interfaces()[i]->loadBackend(str);
            optr->setTemporary(0);
            return optr;
        }
    }
    qFatal("Hmm... you're trying to add an app which doesn't exist...");
    return 0;
}
#endif
