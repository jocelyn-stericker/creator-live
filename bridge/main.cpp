#include <QApplication>
#include "bridgesettings.h"
#include <QFontDatabase>
#include <QFile>
#include <QDebug>
#include <QTcpSocket>
#include "live/../midibridge.h"
#include "live/../midisystem_p.h"
#include "../context-dj/djcontext.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(argc==2&&argv[1][0]=='c') {
        live::audio::registerInterface(new live_private::SecretAudio);
        live_private::SecretMidi::me=new live_private::SecretMidi;
        qDebug()<<"Going to try to be client.";

        // FIXME:: should be null.

//        QTcpSocket* hi = new QTcpSocket();
//        hi->connectToHost("192.168.0.14",1143);
//        hi->waitForConnected();
//        qDebug()<<"CONNECTED!";
//        while(1) {
//            hi->waitForReadyRead();
//            QByteArray x=hi->readAll();
//            x.replace('\0',"_0_");
//            qDebug()<<x;
//        }

        live_private::BridgeClient* b=new live_private::BridgeClient();
        b->tryIp("C0A8000E");
        return a.exec();
    }

    QFontDatabase::addApplicationFont(":/icons/HandelGotDLig.ttf");
    QFile stylefile(":/style/default");
    stylefile.open(QIODevice::ReadOnly);
    a.setStyleSheet(QString(stylefile.readAll()));

    BridgeSettings w;

    DJContext d;
    d.show();

//    live::object::get(live::MidiOnly|live::InputOnly)[1]->midiConnect(live::object::get(live::MidiOnly|live::OutputOnly)[1]);
    w.show();
    
    return a.exec();
}
