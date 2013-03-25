#ifndef MIDIBRIDGE_H
#define MIDIBRIDGE_H

#include <live/object>
#include <live/midi>
#include <live/midievent>
#include <QTcpSocket>

namespace live_private {

// FIXME: needs to be own thread.
class LIBLIVECORESHARED_EXPORT BridgeMidiIn : public QObject, public live::Object {
    Q_OBJECT
    LIVE_MIDI
    LIVE_INPUT
    QString m_name;
public:
    BridgeMidiIn(QString name) : live::Object(name,1,0,2), m_name(name) { }

public slots:
    void readData(QByteArray data) {
        bool ok=0;

        qDebug() << "Trying to read data...";
        QList<QByteArray> commands = data.split('\n');
        if (!commands.size()) return;
        if (commands.first()!="BEGIN EVENT") return;
        commands.pop_front();

        qDebug() << "Event OK";

        if (!commands.size()) return;
        qDebug()<<commands.first() << "VS"<<"FROM "+m_name;
        if (commands.first()!="FROM "+m_name) return;
        commands.pop_front();

        qDebug() << "From OK";

        live::Event ev;
        live::ObjectChain p;
        p.push_back(this);

        ///////////////////////////////////////////////////////////
        if (!commands.size()) return;
        if (!commands.first().startsWith("MSG ")) return;
        commands.first().remove(0,4);
        ev.message=commands.first().toShort(&ok);
        if (!ok) return;
        commands.pop_front();

        qDebug() << "C1 OK"<<ev.message;

        ///////////////////////////////////////////////////////////
        if (!commands.size()) return;
        if (!commands.first().startsWith("DATA1 ")) return;
        commands.first().remove(0,6);
        ev.data1=commands.first().toShort(&ok);
        if (!ok) return;
        commands.pop_front();

        ///////////////////////////////////////////////////////////
        if (!commands.size()) return;
        if (!commands.first().startsWith("DATA2 ")) return;
        commands.first().remove(0,6);
        ev.data2=commands.first().toShort(&ok);
        if (!ok) return;
        commands.pop_front();

        ///////////////////////////////////////////////////////////
        if (!commands.size()) return;
        if (!commands.first().startsWith("DATA3 ")) return;
        commands.first().remove(0,6);
        ev.time=live::Time(commands.first().toInt(&ok))+live::midi::getTime();
        qDebug() << "TIME:"<<ev.time.toTime_ms()<<live::midi::getTime_msec();
        if (!ok) return;
        commands.pop_front();

        ///////////////////////////////////////////////////////////
        qDebug() << "Succesfully read data...";
        live::Event* ev2=new live::Event(ev.message,ev.data1,ev.data2);
        ev2->time=ev.time;
        ev2->time.sec=ev2->time.nsec=-1;
        mOut(ev2,&p);
    }
};

class LIBLIVECORESHARED_EXPORT BridgeMidiOut : public live::Object {
    LIVE_MIDI
    LIVE_OUTPUT
    QTcpSocket* m_sock;
    QString m_name;
public:
    BridgeMidiOut(QTcpSocket* sock, QString name) : live::Object(name,1,0,2), m_sock(sock), m_name(name) {
    }
    void mIn(const live::Event *ev, live::ObjectChain* )
    {
        QByteArray data;
        qDebug() << "Hey, I'm"<<m_name;
        data+="BEGIN EVENT\n";
        data+="TO "+m_name+"\n";
        data+="MSG "+QString::number(ev->message)+"\n";
        data+="DATA1 "+QString::number(ev->data1)+"\n";
        data+="DATA2 "+QString::number(ev->data2)+"\n";
        qDebug() << "###"<<live::midi::getTime_msec() << "VS"<<ev->time.toTime_ms();
        data+="DATA3 "+QString::number(live::midi::getTime_msec()-ev->time.toTime_ms())+"\n";
        data+="END EVENT\n";
        qDebug() << "I'm sending "<<data;
        m_sock->write(data);
        qDebug() << "Written!"<<live::midi::getTime_msec()-ev->time.toTime_ms();
    }
private:
    Q_DISABLE_COPY(BridgeMidiOut)
};

class LIBLIVECORESHARED_EXPORT BridgeClient : public QObject {
    Q_OBJECT
    QTcpSocket* m_sock;
    QList<BridgeMidiIn*> m_ins;
    QList<BridgeMidiOut*> m_outs;
    QList<live::Connection> m_connections;
public:
    BridgeClient()
      : m_sock()
      , m_ins()
      , m_outs()
      {
    }
public slots:
    void tryIp(QString code) {
        if (code.size()!=8) {
            qDebug() << "Invalid IP code...";
        }
        if (m_sock) {
            while (m_ins.size()) { delete m_ins.back(); m_ins.pop_front(); }
            while (m_outs.size()) { delete m_outs.back(); m_outs.pop_front(); }
            delete m_sock;
            m_sock=0;
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
        m_sock = new QTcpSocket();
        qDebug() << "Trying ipX "<<ip;
        m_sock->connectToHost(ip,1143);
        connect(m_sock,SIGNAL(readyRead()),this,SLOT(pair()));
    }

    void pair() {
        Q_ASSERT(sender()==m_sock);
        disconnect(m_sock,SIGNAL(readyRead()),this,SLOT(pair()));
        QList<QByteArray> data=m_sock->readAll().split('\n');

        qDebug() << "Pairing...";

        if (!data.size()) return;
        if (data[0] != "VERSION 1000") return;
        data.pop_front();
        if (!data.size()) return;
        if (data[0] != "BEGIN HELLO_WORLD") return;
        data.pop_front();
        if (!data.size()) return;
        if (data[0] != "BEGIN MIDI_INPUT") return;
        data.pop_front();

        qDebug() << "Agreed on basics.";

        if (!data.size()) return;
        while (data[0]!="END MIDI_INPUT") {
            m_ins.push_back(new BridgeMidiIn(data[0]));
            live::object::set(m_ins.back());

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
            m_outs.push_back(new BridgeMidiOut(m_sock,data[0]));
            live::object::set(m_outs.back());

            data.pop_front();
            if (!data.size()) return;
        }

        if (!data.size()) return;
        data.pop_front();

        for (int i=0;i<m_ins.size();i++)
        {
            for (int j=0;j<m_outs.size();j++)
            {
                if (i!=1||j!=1) continue;
                m_connections.push_back(live::Connection(m_ins[i], m_outs[i], live::MidiConnection));
            }
        }

        connect(m_sock,SIGNAL(readyRead()),this,SLOT(readData()));
    }

    void readData() {
        QByteArray data=m_sock->readAll();
        for (int i=0;i<m_ins.size();i++) m_ins[i]->readData(data);
    }

private:
    Q_DISABLE_COPY(BridgeClient)
};

}

#endif // MIDIBRIDGE_H
