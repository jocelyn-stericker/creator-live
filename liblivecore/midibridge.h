#ifndef MIDIBRIDGE_H
#define MIDIBRIDGE_H

#include <live/object.h>
#include <live/midi.h>
#include <live/midievent.h>
#include <QTcpSocket>

namespace live_private {

class BridgeMidiIn : public QObject, public live::Object {
    Q_OBJECT
    LIVE_MIDI
    LIVE_INPUT
    QString s_name;
public:
    BridgeMidiIn(QString name) : live::Object(name,1,0), s_name(name) { }

public slots:
    void readData(QByteArray data) {
        bool ok=0;

        qDebug()<<"Trying to read data...";
        QList<QByteArray> commands = data.split('\n');
        if (!commands.size()) return;
        if (commands.first()!="BEGIN EVENT") return;
        commands.pop_front();

        qDebug()<<"Event OK";

        if (!commands.size()) return;
        qDebug()<<commands.first()<<"VS"<<"FROM "+s_name;
        if (commands.first()!="FROM "+s_name) return;
        commands.pop_front();

        qDebug()<<"From OK";

        live::Event ev;
        live::ObjectChain p;
        p.push_back(this);

        ///////////////////////////////////////////////////////////
        if (!commands.size()) return;
        if (!commands.first().startsWith("MSG ")) return;
        commands.first().remove(0,4);
        ev.message=commands.first().toInt(&ok);
        if (!ok) return;
        commands.pop_front();

        qDebug()<<"C1 OK"<<ev.message;

        ///////////////////////////////////////////////////////////
        if (!commands.size()) return;
        if (!commands.first().startsWith("DATA1 ")) return;
        commands.first().remove(0,6);
        ev.data1=commands.first().toInt(&ok);
        if (!ok) return;
        commands.pop_front();

        ///////////////////////////////////////////////////////////
        if (!commands.size()) return;
        if (!commands.first().startsWith("DATA2 ")) return;
        commands.first().remove(0,6);
        ev.data2=commands.first().toInt(&ok);
        if (!ok) return;
        commands.pop_front();

        ///////////////////////////////////////////////////////////
        if (!commands.size()) return;
        if (!commands.first().startsWith("DATA3 ")) return;
        commands.first().remove(0,6);
        ev.time=live::Time(commands.first().toInt(&ok))+live::midi::getTime();
        qDebug()<<"TIME:"<<ev.time.toTime_ms()<<live::midi::getTime_msec();
        if (!ok) return;
        commands.pop_front();

        ///////////////////////////////////////////////////////////
        qDebug()<<"Succesfully read data...";
        live::Event* ev2=new live::Event(ev.message,ev.data1,ev.data2);
        ev2->time=ev.time;
        ev2->time.sec=ev2->time.nsec=-1;
        mOut(ev2,p);
    }
};

class BridgeMidiOut : public live::Object {
    LIVE_MIDI
    LIVE_OUTPUT
    QTcpSocket* s_sock;
    QString s_name;
public:
    BridgeMidiOut(QTcpSocket* sock, QString name) : live::Object(name,1,0), s_sock(sock), s_name(name) {
    }
    void mIn(const live::Event *ev, live::ObjectChain &)
    {
        QByteArray data;
        qDebug()<<"Hey, I'm"<<s_name;
        data+="BEGIN EVENT\n";
        data+="TO "+s_name+"\n";
        data+="MSG "+QString::number(ev->message)+"\n";
        data+="DATA1 "+QString::number(ev->data1)+"\n";
        data+="DATA2 "+QString::number(ev->data2)+"\n";
        qDebug()<<"###"<<live::midi::getTime_msec()<<"VS"<<ev->time.toTime_ms();
        data+="DATA3 "+QString::number(live::midi::getTime_msec()-ev->time.toTime_ms())+"\n";
        data+="END EVENT\n";
        qDebug()<<"I'm sending "<<data;
        s_sock->write(data);
        qDebug()<<"Written!"<<live::midi::getTime_msec()-ev->time.toTime_ms();
    }
};

class BridgeClient : public QObject {
    Q_OBJECT
    QTcpSocket* s_sock;
    QList<BridgeMidiIn*> s_ins;
    QList<BridgeMidiOut*> s_outs;
public:
    BridgeClient() : s_sock() {}
public slots:
    void tryIp(QString code) {
        if (code.size()!=8) {
            qDebug()<<"Invalid IP code...";
        }
        if (s_sock) {
            while (s_ins.size()) { delete s_ins.back(); s_ins.pop_front(); }
            while (s_outs.size()) { delete s_outs.back(); s_outs.pop_front(); }
            delete s_sock;
            s_sock=0;
            return;
        }
        QStringList parts;
        do {
            QString part=code;
            part.truncate(2);
            code.remove(0,2);
            parts.push_back(QString::number(part.toInt(0,16)));
        } while (code.size());
        QString ip=parts.join(".");
        s_sock = new QTcpSocket();
        qDebug()<<"Trying ipX "<<ip;
        s_sock->connectToHost(ip,1143);
        connect(s_sock,SIGNAL(readyRead()),this,SLOT(pair()));
    }

    void pair() {
        Q_ASSERT(sender()==s_sock);
        disconnect(s_sock,SIGNAL(readyRead()),this,SLOT(pair()));
        QList<QByteArray> data=s_sock->readAll().split('\n');

        qDebug()<<"Pairing...";

        if (!data.size()) return;
        if (data[0] != "VERSION 1000") return;
        data.pop_front();
        if (!data.size()) return;
        if (data[0] != "BEGIN HELLO_WORLD") return;
        data.pop_front();
        if (!data.size()) return;
        if (data[0] != "BEGIN MIDI_INPUT") return;
        data.pop_front();

        qDebug()<<"Agreed on basics.";

        if (!data.size()) return;
        while (data[0]!="END MIDI_INPUT") {
            s_ins.push_back(new BridgeMidiIn(data[0]));
            live::object::set(s_ins.back());

            data.pop_front();
            if (!data.size()) return;
        }

        if (!data.size()) return;
        data.pop_front();

        if (!data.size()) return;
        if (data[0] != "BEGIN MIDI_OUTPUT") return;
        data.pop_front();

        if (!data.size()) return;
        while (data[0]!="END MIDI_OUTPUT") {
            s_outs.push_back(new BridgeMidiOut(s_sock,data[0]));
            live::object::set(s_outs.back());

            data.pop_front();
            if (!data.size()) return;
        }

        if (!data.size()) return;
        data.pop_front();

        for (int i=0;i<s_ins.size();i++)
        {
            for (int j=0;j<s_outs.size();j++)
            {
                if (i!=1||j!=1) continue;
                qDebug()<<"Connect"<<s_ins[i]->name()<<"to"<<s_outs[j]->name();
                s_ins[i]->midiConnect(s_outs[j]);
            }
        }

        connect(s_sock,SIGNAL(readyRead()),this,SLOT(readData()));
    }

    void readData() {
        QByteArray data=s_sock->readAll();
        for (int i=0;i<s_ins.size();i++) s_ins[i]->readData(data);
    }
};

}

#endif // MIDIBRIDGE_H
