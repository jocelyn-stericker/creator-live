/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#include "vst.h"
#include "vstlinux_p.h"
#include "vstsidekick.h"

using namespace live;

#ifndef _WIN32

int VstR::_lastId=0;
int Vst::lastVstId=-1;
QList<Vst*> Vst::m_vst;
QStringList* Vst::m_vstCache=0;
QStringList Vst::m_vstpaths_linux;
QMultiMap<QString, QPair<QString, live::ObjectPtr> > Vst::m_map;

QStringList Vst::getVstPaths()
{
    return m_vstpaths_linux;
}

void Vst::init()
{
    rep = new VstR(this,this->filename);
    connections.push_back(live::Connection(rep->audioFromVst,rep,live::HybridConnection));
    if (!rep->audioFromVst)
    {
        return;
    }
    if (!rep->midiOut)
    {
        return;
    }
}

bool Vst::ok() const
{
    return rep->_ok;
}

void Vst::show()
{
    // fixme
}

void Vst::hide()
{
    // fixme
}

Vst::~Vst()
{
    if (rep) {
        rep->setParent(0);
        rep->deleteLater();
    }
    qDebug() << "Delete VST";
}

void Vst::aIn(const float *data, int chan, Object* p)
{
    Q_ASSERT(chan<2);
    Q_ASSERT(rep->audioFromVst);
    if (p == rep->audioFromVst.data())
    {
        aOut(data,chan,this);
    }
    else if (rep->audioToVst.valid())
    {
        rep->audioToVst->aIn(data,chan,this);
    }
}

void Vst::mIn(const Event *data, ObjectChain*p)
{
    Q_ASSERT(rep->midiOut);
    p->push_back(this);
    rep->midiOut->mIn(data,p);
    p->pop_back();
}

void VstR::addSidekicks(QStringList to, QStringList from)
{
    for (int i=0;(i<to.size()||i<from.size())&&i<15;i++)
    {
        QList<ObjectPtr> ninl=object::get(AudioOnly|InputOnly);
        bool ok=0;
        ObjectPtr audioFromVst=0;
        for (int j=0;j<ninl.size();j++)
        {
            if (ninl[j]->name()==from[i])
            {
                audioFromVst=ninl[j];
                ok=1;
            }
        }
        if (!ok)
        {
            qCritical() << "VstR (linux) could not find match for"<<from[i];
        }
        Q_ASSERT(ok);
        ObjectPtr audioToVst=0;
        if (i<to.size()) {
            ok=0;
            ninl=object::get(AudioOnly|OutputOnly);
            for (int j=0;j<ninl.size();j++)
            {
                if (ninl[j]->name()==to[i])    //to[j]?
                {
                    audioToVst=ninl[j];
                    ok=1;
                }
            }
            if (!ok)
            {
                audioToVst=audio::null(2);
                qWarning() << "VstR (linux) could not find match for"<<to[i];
            }
//            Q_ASSERT(ok);
        }
        VstSidekick* l=new VstSidekick(this,i,audioToVst,audioFromVst);
        object::set(l);
        m_sidekicks.push_back(l);
    }
}

#endif
