/*****************************************************
app_dotlive.cpp

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

#include "livewindow.h"
#include "liveapplication.h"
#include <live_widgets/vscrollcontainer.h>
#include "trackgroupaudio.h"
#include "trackgroupmidi.h"
#include <live/core.h>
#include <live_widgets/appframe.h>
#include <QMessageBox>

#ifndef APP_DOTLIVE_CPP
#define APP_DOTLIVE_CPP
#define LOAD
#include "app_dotlive.cpp"
#undef LOAD
#undef BEGIN
#undef IS_LOAD
#undef IS_SAVE
#undef IO
#undef _THIS
#undef RETURN
#undef verify
#define SAVE
#endif

#ifdef LOAD
#define BEGIN QDataStream ret(str); QString xqstring; qint32 xint32; bool xbool; QByteArray xba
#define IS_LOAD 1
#define IS_SAVE 0
#define IO >>
#define _THIS 0
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
#define BEGIN QByteArray str; QDataStream ret(&str,QIODevice::WriteOnly); QString xqstring; qint32 xint32; bool xbool; QByteArray xba
#define IS_LOAD 0
#define IS_SAVE 1
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


#ifndef TYPES_
#define TYPES_
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
using namespace live_widgets;

#ifdef LOAD
LiveWindow* LiveWindow::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray LiveWindow::save()
#endif
{
    BEGIN;
    Q_UNUSED(xbool);
    Q_UNUSED(xint32);
    Q_UNUSED(xba);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    //Assertions...
    if (IS_LOAD)
    {
        QList<live::ObjectPtr> objs=object::singleton()->s_objects;
        for (int i=0;i<objs.size();i++)
        {
            Q_ASSERT(!objs[i]->aConnections.size()&&!objs[i]->mConnections.size());
        }
    }

    /*001*/
    verify(ret,(QString)"Creator Live Project File\n"
           "Creator Live is Copyright Joshua Netterfield\n");

    /*002*/
    verify(ret,(QString)"Not from an official/stable version of Live");

    /*003*/

    //FIXME
//    int count=liveApp->_vsts.count();
//    ret IO count;
//    if (IS_LOAD) while (liveApp->_vsts.count())
//    {
//        delete liveApp->_vsts.takeFirst();
//    }
//    for (int i=0;i<count;i++)
//    {
//        QString name;
//        if (IS_SAVE)
//        {
//            name=liveApp->_vsts[i]->filename;
//        }
//        ret IO name;
//        if (IS_LOAD)
//        {
//            if (QFile::exists(name))
//            {
//                liveApp->addVst(new Vst(name));
//            }
//            else
//            {
//                QMessageBox::warning(0,"Couldn't load VST","Live tried to open "+name+" but this VST effect could not be opened.","Ok");
//            }
//        }
//    }

    /*003*/
    verify(ret,(QString)"BEGIN global MidiFilter");
    qint32 ncount=MidiFilter::_u.size();
    ret IO ncount;
    if (IS_LOAD)
    {
        Q_ASSERT(!MidiFilter::_u.size());
    }
    for (int i=0;i<ncount;i++)
    {
        if (IS_SAVE)
        {
            xba=MidiFilter::_u[i]->save();
        }
        ret IO xba;
        if (IS_LOAD)
        {
            MidiFilter::load(xba);  //is added to universe
        }
    }

    //////////////////////////////////////////////////////////////////

    /*004*/
    verify(ret,(QString)"BEGIN Current song");
    if (IS_SAVE)
    {
        xba=song::current()->save();
    }
    ret IO xba;
    if (IS_LOAD)
    {
        song::setCurrent( song::load(xba) );
    }
    verify(ret,(QString)"END Current song");

    LiveWindow*x=LiveWindow::singleton;
#ifdef LOAD
    x->newProject(0);
    //////////////////////////////////////////////////
    //    QWidget* central=new QWidget(parent);
    //    QVBoxLayout* vblayout = new QVBoxLayout(central);
    //    central->show();
    //    VScrollContainer* hv = new VScrollContainer(1);
    //    hv->setObjectName("HathorView_core");
    //    x=new Ribbon(central,hv);
    //    x->setObjectName("ribbon");
    //    vblayout->addWidget(x);
    //    vblayout->addWidget(hv);
    //    Q_ASSERT(qobject_cast<QMainWindow*>(parent));
    //    parent->setWindowTitle("Creator Live (reloaded)");
    //    static_cast<QMainWindow*>(parent)->setCentralWidget(central);
    //    parent->setWindowState(Qt::WindowMaximized);
    //    parent->show();
    //////////////////////////////////////////////////
    //    x->unlock();
#else
#endif

    /*005*/
    ncount=0;
    if (IS_SAVE)
    {
        for (int i=0;i<x->hathorView()->count();i++)
        {
            if (qobject_cast<TrackGroupAudio*>(x->hathorView()->at(i)))
            {
                ncount++;
            }
            else if (qobject_cast<TrackGroupMidi*>(x->hathorView()->at(i)))
            {
                ncount++;
            }
        }
    }
    ret IO ncount;
    if (IS_SAVE)
    {
        for (int i=0;i<x->hathorView()->count();i++)
        {
            if (qobject_cast<TrackGroupAudio*>(x->hathorView()->at(i)))
            {
                xqstring="NEXT IS AudioTrackGroup";
                ret IO xqstring;
                xba=qobject_cast<TrackGroupAudio*>(x->hathorView()->at(i))->save();
                ret IO xba;
            }
            else if (qobject_cast<TrackGroupMidi*>(x->hathorView()->at(i)))
            {
                xqstring="NEXT IS MidiTrackGroup";
                ret IO xqstring;
                xba=qobject_cast<TrackGroupMidi*>(x->hathorView()->at(i))->save();
                ret IO xba;
            }
        }
    }
    else if (IS_LOAD)
    {
        for (int i=0;i<ncount;i++)
        {
            ret IO xqstring;
            if (xqstring=="NEXT IS AudioTrackGroup")
            {
                ret IO xba;
                x->hathorView()->push_back(TrackGroupAudio::load(xba,x->hathorView()));
            }
            else if (xqstring=="NEXT IS MidiTrackGroup")
            {
                ret IO xba;
                x->hathorView()->push_back(TrackGroupMidi::load(xba,x->hathorView()));
            }
        }
    }
    x->hathorView()->updateItems();

    /*006*/
#ifdef SAVE
    ret IO x->s_patches.size();
    for (int i=0;i<x->s_patches;i++) {
        for (int k=0;k<x->s_patches[i]->widgets.size();k++)  {
            int l=-1;
            for (int j=0;j<x->hathorView()->count();j++) {
                if ((*x->hathorView())[j]==x->s_patches[i]->widgets[k]) {
                    k=j;
                    break;
                }
            }
            Q_ASSERT(l!=-1);
            ret IO l;
        }
    }
#endif

    /*007*/
#ifdef SAVE
    ret IO saveBindings();
#else
    ret IO xba;
    x->loadBindings(xba);
#endif

#ifdef SAVE
    ret IO x->hathorView()->saveBindings();
#else
    ret IO xba;
    x->hathorView()->loadBindings(xba);
#endif

    /*008*/
    (verify(ret,(QString)"Yay!"));

    RETURN;
}

#ifdef LOAD
TrackGroupAudio* TrackGroupAudio::load(const QByteArray&str,QWidget*c_parent)
#endif
#ifdef SAVE
QByteArray TrackGroupAudio::save()
#endif
{
    BEGIN;
    Q_UNUSED(xbool);
    Q_UNUSED(xint32);
    Q_UNUSED(xba);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN TrackGroupAudio"));

    /*002*/
    (verify(ret,(qint32 )110718));

    //////////////////////////////////////////////////////////////////

    TrackGroupAudio*x=IS_SAVE?_THIS:0;

    /*CONSTRUCT*/
    if (IS_SAVE)
    {
        xqstring=x->s_input->name();
    }
    ret IO xqstring;
    if (IS_LOAD)
    {
        ObjectPtr search=0;
        for (int j=0;j<object::get(AudioOnly|InputOnly).size();j++)
        {
            if (xqstring==object::get(AudioOnly|InputOnly)[j]->name())
            {
                search=object::get(AudioOnly|InputOnly)[j];
                break;
            }
        }
        Q_ASSERT(search);
#ifdef LOAD
        x=new TrackGroupAudio(search,c_parent,1);
#endif
    }

    /*003*/
    qint32 ncount = 0;
    if (IS_SAVE)
    {
        for (int i=0;i<x->s_hathorView->count();i++)
        {
            ncount+=(qobject_cast<Track*>(x->s_hathorView->at(i)))?1:0;
        }
    }
    ret IO ncount;
    if (IS_LOAD)
    {
        for (int i=0;i<ncount;i++)
        {
            ret IO xba;
            x->s_hathorView->insert(x->s_hathorView->count()-1,Track::load(xba));
            x->s_hathorView->updateItems();
        }
    }
    else
    {
        for (int i=0;i<x->s_hathorView->count();i++)
        {
            Track*t=qobject_cast<Track*>(x->s_hathorView->at(i));
            if (t)
            {
                xba=t->save();
                ret IO xba;
            }
        }
    }

    /*004*/
    P_INT32(x->s_id);
    if (x->s_id>x->s_lastId)
    {
        x->s_lastId=x->s_id;
    }
    x->setObjectName("TrackGroup_"+QString::number(x->s_id));

    /*005*/
    (verify(ret,(QString)"END TrackGroupAudio"));

    RETURN;
}


#ifdef LOAD
TrackGroupMidi* TrackGroupMidi::load(const QByteArray&str,QWidget*c_parent)
#endif
#ifdef SAVE
QByteArray TrackGroupMidi::save()
#endif
{
    BEGIN;
    Q_UNUSED(xbool);
    Q_UNUSED(xint32);
    Q_UNUSED(xba);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN TrackGroupMidi"));

    /*002*/
    (verify(ret,(qint32 )110718));

    //////////////////////////////////////////////////////////////////

    TrackGroupMidi*x=IS_SAVE?_THIS:0;

    /*CONSTRUCT*/
    if (IS_SAVE)
    {
        xqstring=x->s_input->name();
    }
    ret IO xqstring;
    if (IS_LOAD)
    {
        ObjectPtr search=0;
        for (int j=0;j<object::get(MidiOnly|InputOnly).size();j++)
        {
            if (xqstring==object::get(MidiOnly|InputOnly)[j]->name())
            {
                search=object::get(MidiOnly|InputOnly)[j];
                break;
            }
        }
        Q_ASSERT(search);
#ifdef LOAD
        x=new TrackGroupMidi(search,c_parent,1);
#endif
    }

    /*003*/
    qint32 ncount = 0;
    if (IS_SAVE)
    {
        for (int i=0;i<x->s_hathorView->count();i++)
        {
            if ((*x->s_hathorView)[i]->objectName()=="xpush")
            {
                continue;
            }
            ncount+=(qobject_cast<Track*>(x->s_hathorView->at(i)))?1:0;
        }
    }
    ret IO ncount;
    if (IS_LOAD)
    {
        for (int i=0;i<ncount;i++)
        {
            ret IO xba;
            x->s_hathorView->insert(x->s_hathorView->count()-1,Track::load(xba) );
            x->s_hathorView->updateItems();
        }
    }
    else
    {
        for (int i=0;i<x->s_hathorView->count();i++)
        {
            Track*t=qobject_cast<Track*>(x->s_hathorView->at(i));
            if (t)
            {
                xba=t->save();
                ret IO xba;
            }
        }
    }

    /*004*/
    P_INT32(x->s_id);
    if (x->s_id>x->s_lastId)
    {
        x->s_lastId=x->s_id;
    }
    x->setObjectName("TrackGroup_"+QString::number(x->s_id));

    /*005*/
    P_INT32(x->s_selectedFilter);

    /*006*/
    P_INT32(x->s_selectedMode);

    /*007*/
    for (int i=0;i<5;i++) {
        if (IS_SAVE) {
            xba=x->s_midiFilters[i]->save();
        }
        ret IO xba;
        if (IS_LOAD) {
            x->s_midiFilters[i]=MidiFilter::load(xba);
        }
    }
    x->changeActiveMode_2(x->s_selectedMode);

    /*008*/
    (verify(ret,(QString)"END TrackGroupMidi"));

    RETURN;
}

#ifdef LOAD
Track* Track::load(const QByteArray&str)
#endif
#ifdef SAVE
QByteArray Track::save()
#endif
{
    BEGIN;
    Q_UNUSED(xbool);
    Q_UNUSED(xint32);
    Q_UNUSED(xba);

    ret.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*001*/
    (verify(ret,(QString)"BEGIN Track"));

    /*002*/
    (verify(ret,(qint32 )110708));


    //////////////////////////////////////////////////////////////////

    /*003*/
    Track*x=IS_SAVE?_THIS:0;
    if (IS_SAVE)
    {
        xba=x->s_ambition.save();
    }
    ret IO xba;
    x=IS_SAVE?x:new Track(Ambition::load(xba));

    /*003B*/
    qint32 size=x->s_ambition.chainSize();   //Already found
    if (IS_LOAD)
    {
        for (int i=0;i<size;i++)
        {
            x->addApp(i+1,dynamic_cast<AppFrame*>(app::newFrontend(x->s_ambition.at(i)->name(),x->s_ambition.at(i))),0);
            //            app::newFrontend(x->s_ambition.at(i)->name,x->s_ambition.at(i))->show();
        }
    }

    /*004*/
    P_INT32(x->s_id);
    if (x->s_id>x->s_lastId)
    {
        x->s_lastId=x->s_id;
    }
    x->setObjectName("Track_"+QString::number(x->s_id));

    /*005*/
    (verify(ret,(QString)"END Track"));

    RETURN;
}
