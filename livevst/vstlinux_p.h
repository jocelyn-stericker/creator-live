/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#if defined(__linux__) && __linux__
#ifndef VST_RH
#define VST_RH

#include "vst.h"
#include "live/object"
#include "live/audio"
#include "live/audiointerface"
#include <unistd.h>
#include <QWidget>
#include <QTimer>
#include <QProcess>
#include <QApplication>

class VstSidekick;

class VstR : public QObject, public live::Object
{
public:
    Q_OBJECT
    LIVE_HYBRID
    LIVE_EFFECT
    Vst& me;
    QList<live::Connection> connections;
    QString _vst;
    live::ObjectPtr midiOut;
    live::ObjectPtr audioFromVst;
    live::ObjectPtr audioToVst;
    int _id;
    static int _lastId;
    QProcess* _magic;
    bool _ok;

    bool aOn() const { return 1; } bool mOn() const{ return 1; }

    VstR(Vst* cme,QString cvst)
      : live::Object("VST Internal",false,false,2)
      , me(*cme)
      , _vst(cvst)
      , midiOut(0)
      , audioFromVst(0)
      , audioToVst(0)
      , _id(++_lastId)
      , _magic(new QProcess(this))
    {
        setTemporary(0);
        _ok=1;
        setParent(cme);
//        _vst.replace(" ","\\ ");
        QStringList origAudioIn = live::audio::getInputChanStringList();
        QStringList origAudioOut = live::audio::getOutputChanStringList();
        QStringList origMidi;
        QList<live::ObjectPtr> tmpMidi=live::object::get(live::OutputOnly|live::MidiOnly);
        origMidi << tmpMidi;

        qDebug()<<_vst;
        _magic->start("/usr/local/bin/vsthost",QStringList(_vst));
        connect(_magic,SIGNAL(finished(int)),this,SLOT(killParent()));
        qDebug() << "STARTING...";
        if(_magic->state()==QProcess::NotRunning)
        {
            qDebug() << "Forking vsthost did not work!";
        }
        int count=0;
        QStringList lastAudioIn;
        do
        {
            QStringList newAudioIn = live::audio::getInputChanStringList();
            QStringList newAudioOut = live::audio::getOutputChanStringList();
            QList<live::ObjectPtr> newMidi = live::object::get(live::OutputOnly|live::MidiOnly);
            bool goOn=(lastAudioIn==newAudioIn);
            lastAudioIn=newAudioIn;
            if(goOn&& (newAudioIn!=origAudioIn))
            {
                QStringList xAudioIn,xAudioOut;
                QList<live::ObjectPtr> xMidi;
                for(int i=0; i<newAudioIn; i++)
                {
                    if(!origAudioIn.contains(newAudioIn[i]))
                    {
                        xAudioIn.push_back(newAudioIn[i]);
                        QStringList l=live::audio::getOutputChanStringList();
                        for(int j=0;j<l.size();j++) {
                            if(!xAudioIn.back().contains("system")&&!l[j].contains("system")) continue;
                            qDebug()<<xAudioIn.back()<<l[j];
                            QMetaObject::invokeMethod(live::audio::getCurrentInterface(),"jack_disconnect",Qt::DirectConnection,
                                Q_ARG(QString,xAudioIn.back()), Q_ARG(QString,l[j]) );
                        }
                    }
                }
                for(int i=0; i<newAudioOut; i++)
                {
                    if(!origAudioOut.contains(newAudioOut[i]))
                    {
                        xAudioOut.push_back(newAudioOut[i]);
                    }
                }
                for(int i=0; i<newMidi; i++)
                {
                    if(!origMidi.contains(newMidi[i]->name()))
                    {
                        xMidi.push_back(newMidi[i]);
                    }
                }

                if((xMidi.size())&&xAudioIn.size())
                {
                    midiOut=xMidi[0];

                    if(xAudioIn.size()%2==1) {
                        qDebug() << "New inputs not modulo 2";
                        _ok=0;
                        return;
                    }

                    QStringList sidekicks_from;
                    for(int i=0;i<xAudioIn.size();i+=2)
                    {
                        QStringList dual;
                        dual<<xAudioIn[i]<<xAudioIn[i+1];
                        qDebug() << "VVVVV"<<dual;
                        live::audio::addMapping(dual,1,"In from VST Audio ("+QString::number(_id)+","+QString::number(i/2)+")");
                        sidekicks_from.push_back("In from VST Audio ("+QString::number(_id)+","+QString::number(i/2)+")");
                    }

                    QList<live::ObjectPtr> ninl=live::object::get(live::InputOnly|live::AudioOnly);
                    bool ok=0;
                    for(int i=0;i<ninl.size();i++)
                    {
                        if(ninl[i]->name()=="In from VST Audio ("+QString::number(_id)+",0)")
                        {
                            audioFromVst=ninl[i];
                            ok=1;
                        }
                    }
                    if(!ok)
                    {
                        _ok=0;
                        qDebug() << "VstLinux_p: Failed err 1.";
                        return;
                    }

                    ok=0;
                    QStringList sidekicks_to;
                    for(int i=0;i<xAudioOut.size();i+=2)
                    {
                        ok=1;
                        QStringList dual;
                        dual<<xAudioOut[i]<<xAudioOut[i+1];
                        qDebug() << "Out to VST Audio ("+QString::number(_id)+","+QString::number(i/2)+")"<<dual;
                        live::audio::addMapping(dual,0,"Out to VST Audio ("+QString::number(_id)+","+QString::number(i/2)+")");
                        sidekicks_to.push_back("Out to VST Audio ("+QString::number(_id)+","+QString::number(i/2)+")");
                    }

                    if(ok)
                    {
                        live::audio::refresh();
                        QList<live::ObjectPtr> noutl=live::object::get(live::OutputOnly|live::AudioOnly);

                        bool ok=0;
                        int l=0;
                        for(int i=0;i<noutl.size();i++)
                        {
                            qDebug()<<xAudioOut.size() << "#"<<noutl[i]->name()<<("Out to VST Audio ("+QString::number(_id)+","+QString::number(l)+")");
                            if(noutl[i]->name()==("Out to VST Audio ("+QString::number(_id)+","+QString::number(l)+")"))
                            {
                                ++l;
                                ok=1;
                                audioToVst=noutl[i];
                                audioToVst->newConnection();
                            }
                        }
//                        Q_ASSERT(ok);
                        if(!ok)
                        {
                            _ok=0;
                            qDebug() << "VstLinux_p: Failed err 2.";
                            return;
                        }
                    }
                    connections.push_back(live::Connection(this,&me,live::HybridConnection));
                    addSidekicks(sidekicks_to, sidekicks_from);
                    _ok=1;
                    break;
                }
            }
            live::Object::endAsyncAction(__FILE__, __LINE__);
            qApp->processEvents();
            QTimer timer;
            timer.setSingleShot(1);
            timer.setInterval(250);
            QEventLoop l;
            connect(&timer,SIGNAL(timeout()),&l,SLOT(quit()));
            timer.start();
            l.exec();
            live::Object::beginAsyncAction();

        } while (++count<80);
    }
    ~VstR()
    {
        live::Object::beginAsyncAction();
        kill();
//        while (s_sidekicks.size()) deletem_sidekicks.takeFirst().data();
        _magic->kill();
        live::Object::endAsyncAction(__FILE__,__LINE__);
    }

    void addSidekicks(QStringList to, QStringList from);

    void aIn(const float *data, int chan, live::Object*p)
    {
        if(p == audioFromVst.data())
        {
            aOut(data,chan,this);
        }
        else
        {
            if(audioToVst.valid())
            {
                //DO NOT REGISTER ME
                audioToVst->aIn(data,chan,p);
            }
        }
    }

public slots:
    void killParent() {
        me.deleteLater();
    }

public:
    QList<live::ObjectPtr>m_sidekicks;
};

#endif //VST_RH
#endif

