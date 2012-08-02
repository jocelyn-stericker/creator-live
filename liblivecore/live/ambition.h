/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef AMBITION_H
#define AMBITION_H

#include "live/object.h"
#include "live/variantbinding.h"
#include <iostream>

namespace live {

class Connection
{
    live::ObjectPtr a, b;
public:
    enum Type {
        Audio=0,
        Midi=1,
        Hybrid=2
    } t;
    Connection(live::ObjectPtr a,live::ObjectPtr b,const Connection::Type&t)
    {
        qDebug()<<"[CONNECT]"<<a->name()<<"->"<<b->name()<<"| type:"<<(int)t;
        this->a=a;
        this->b=b;
        this->t=t;
        connect();
    }

    ~Connection()
    {
        qDebug()<<"[DISCONNECT]"<<a->name()<<"->"<<b->name()<<"| type:"<<(int)t;
        connect();
    }
private:
    void connect()
    {
        if(t==Audio)
        {
            a->audioConnect(b);
        }
        else if(t==Midi)
        {
            a->midiConnect(b);
        }
        else if(t==Hybrid)
        {
            a->hybridConnect(b);
        }
    }
};

class Ambition
{
    live::ObjectPtr s_input;          /*003*/
    live::ObjectChain s_chain;        /*004*/
    live::ObjectPtr s_output;         /*005*/
    live::ObjectPtr s_loopbackOut;    /*006*/
    QList<Connection*> s_connections;
    friend class live::Object;
public:
    RELOADABLE(Ambition)
    live::Bound<QString> b_output;
    Ambition(live::ObjectPtr cinput=0,live::ObjectChain chain=live::ObjectChain(), live::ObjectPtr coutput=0, live::ObjectPtr cloop=0);
    ~Ambition();

    void setInput(live::ObjectPtr o)
    {
        if(s_connections.size())
        {
            unset();
        }
        s_input=o;
        setChain(s_chain);
    }

    void setOutput(live::ObjectPtr o)
    {
        if(s_connections.size())
        {
            unset();
        }
        s_output=o;
        setChain(s_chain);
        b_output=o->name();
    }

    void setLoopbackOut(live::ObjectPtr o)
    {
        if(s_connections.size())
        {
            unset();
        }
        s_loopbackOut=o;
        setChain(s_chain);
    }

    bool setChain(live::ObjectChain c)
    {
        if(s_connections.size())
        {
            unset();
        }
        s_chain=c;

        if(s_input.valid()&&s_output.valid()&&s_loopbackOut.valid()&&s_output->isMidiObject()&&s_output->isAudioObject()&&s_input->isMidiObject()&&s_loopbackOut->isAudioObject())
        {
            // Prefered type of connection for hybrid situations.

            // Process MIDI only until first hybrid. (0)
            // Process hybrid only until first audio.(1)
            // Process audio until end.              (2)

            bool ok=1;
            int stage=0;
            for(int i=0;i<s_chain.size();i++) {
                if(stage==0&&s_chain[i]->isAudioObject()) {
                    if(s_chain[i]->isMidiObject()) {
                        stage=1;
                    } else {
                        stage=2;
                    }
                } else if(stage==1&&!s_chain[i]->isMidiObject()) {
                    if(s_chain[i]->isAudioObject()) {
                        stage=2;
                    } else {
                        ok=0;
                        break;
                    }
                } else if(stage==2&&!s_chain[i]->isAudioObject()) {
                    ok=0;
                    break;
                }
            }

            if(ok) {
                qDebug()<<"Hybrid.";
                stage=0;
                live::ObjectPtr prev=s_input;
                for(int i=0;i<s_chain.size();i++) {
                    if(stage==0&&s_chain[i]->isAudioObject()) {
                        s_connections.push_back(new Connection(prev,s_output,Connection::Midi));
                        s_connections.push_back(new Connection(s_output,s_chain[i],Connection::Audio));

                        s_chain[i]->s_aOn=1;
                        if(s_chain[i]->isMidiObject()) {
                            s_connections.push_back(new Connection(prev,s_chain[i],Connection::Midi));
                            s_chain[i]->s_mOn=1;
                            stage=1;
                        } else {
                            s_chain[i]->s_mOn=0;
                            stage=2;
                        }
                    } else if(stage==1&&!s_chain[i]->isMidiObject()) {
                        s_connections.push_back(new Connection(prev,s_chain[i],Connection::Audio));
                        s_chain[i]->s_aOn=1;
                        s_chain[i]->s_mOn=0;

                        if(s_chain[i]->isAudioObject()) {
                            stage=2;
                        } else {
                            ok=0;
                            break;
                        }
                    } else if(stage==2&&!s_chain[i]->isAudioObject()) {
                        ok=0;
                        break;
                    } else {
                        if(stage==0||stage==1) {
                            s_chain[i]->s_mOn=1;
                            s_connections.push_back(new Connection(prev,s_chain[i],Connection::Midi));
                        } else s_chain[i]->s_mOn=0;

                        if(stage==1||stage==2) {
                            s_connections.push_back(new Connection(prev,s_chain[i],Connection::Audio));                        s_chain[i]->s_aOn=1;
                            s_chain[i]->s_aOn=1;
                        } else s_chain[i]->s_aOn=0;
                    }
                    prev=s_chain[i];
                }
                if(stage==0) {
                    s_connections.push_back(new Connection(prev,s_output,Connection::Midi));
                    s_connections.push_back(new Connection(s_output,s_loopbackOut,Connection::Audio));
                } else {
                    s_connections.push_back(new Connection(prev,s_loopbackOut,Connection::Audio));
                }

                Q_ASSERT(ok);

                return 1;
            }
        }

        if(s_input.valid()&&s_output.valid()&&s_input->isMidiObject()&&s_output->isMidiObject())
        {
            qDebug()<<"Midi.";
            bool ok=1;
            for(int i=0;i<s_chain.size();i++) if(!s_chain[i]->isMidiObject()) ok=0;
            if(ok) {
                live::ObjectPtr mprev=s_input;
                for(int i=0;i<s_chain.size();i++)
                {
                    s_connections.push_back(new Connection(mprev,s_chain[i],Connection::Hybrid));
                    mprev=s_chain[i];
                }
                if(!s_loopbackOut.valid()) {
                    s_connections.push_back(new Connection(mprev,s_output,Connection::Hybrid));
                } else {
                    s_connections.push_back(new Connection(mprev,s_output,Connection::Midi));
                    s_connections.push_back(new Connection(s_output,s_loopbackOut,Connection::Audio));
                }
                return 1;
            }
        }

        if(s_input.valid()&&s_output.valid()&&(s_input->isAudioObject()||(s_loopbackOut.valid()&&s_loopbackOut->isAudioObject()))&&s_output->isAudioObject())
        {
            qDebug()<<"Audio.";
            bool ok=1;
            for(int i=0;i<s_chain.size();i++) if(!s_chain[i]->isAudioObject()) ok=0;
            if(ok) {
                live::ObjectPtr aprev=s_input;
                if(s_loopbackOut.valid()&&s_loopbackOut->isAudioObject()) {
                    s_connections.push_back(new Connection(aprev,s_output,Connection::Audio));
                    aprev=s_output;
                }
                for(int i=0;i<s_chain.size();i++)
                {
                    s_connections.push_back(new Connection(aprev,s_chain[i],Connection::Audio));
                    aprev=s_chain[i];
                }
                if(!s_loopbackOut.valid()) {
                    s_connections.push_back(new Connection(aprev,s_output,Connection::Hybrid));
                } else {
                    s_connections.push_back(new Connection(aprev,s_loopbackOut,Connection::Audio));
                }
                return 1;
            }

        }
        else
        {
            qCritical()<<"No solution to audio train...";
            return 0;
        }
        return 1;
    }

    void unset()
    {
        while(s_connections.size())
        {
            delete s_connections.takeFirst();
        }
    }

    void set()
    {
        setChain(s_chain);
    }

    inline int chainSize()
    {
        return s_chain.size();
    }

    void insert(int i,live::ObjectPtr b)
    {
        live::ObjectChain c2=s_chain;
        c2.insert(i,b); //should be able to check
        setChain(c2);
    }

    void removeFromChain(int i)
    {
        live::ObjectChain c2=s_chain;
        c2.removeAt(i);
        setChain(c2);
    }

    bool inputIsAudioObject()
    {
        if(!s_input)
        {
            return 0;
        }
        else
        {
            return s_input->isAudioObject();
        }
    }

    bool inputIsMidiObject()
    {
        if(!s_input)
        {
            return 0;
        }
        else
        {
            return s_input->isMidiObject();
        }
    }

    live::ObjectPtr at(int b)
    {
        return s_chain[b];
    }
};

class ambition : public QObject
{
    Q_OBJECT
    static ambition* s_self;
public:
    static ambition* self();
    static void notifyCreated(Ambition*);
    static void notifyDestroyed(Ambition*);
protected:
    void u_notifyCreated(Ambition*);
    void u_notifyDestroyed(Ambition*);
signals:
    void created(Ambition*);
    void destoryed(Ambition*);
};

} // namespace live

#endif // AMBITION_H
