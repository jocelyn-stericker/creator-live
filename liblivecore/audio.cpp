/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/audio>

#include <live/audiointerface>
#include <live/songsystem>
#include <live/keysignature>

#include "audiosystem_p.h"

#include <QCryptographicHash>

using namespace live_private;

#ifdef __QNX__
#error This should not be compiled in QNX builds. Use audio_qnx.cpp
#endif

///////////////////////////////////////////////////////////////////////////////////////
// Interface with jack connect
///////////////////////////////////////////////////////////////////////////////////////

QStringList jackGetPorts( bool getReadPorts, bool getWritePorts ) {
    //return QStringList();
    jack_client_t* client;
    if ( SecretAudio::singleton && !SecretAudio::singleton->client ) {
        SecretAudio::singleton->makeClient();
    }
    if ( !SecretAudio::singleton || !SecretAudio::singleton->client ) client = jack_client_open("CLListen", JackNullOption, NULL);
    else client = SecretAudio::singleton->client;

    QStringList a;
    if (getReadPorts) {
        do {
            const char **readPorts;
            readPorts = jack_get_ports(client, NULL, "audio", JackPortIsOutput );

            if ( !readPorts ) {
                break;
            }

            for (int i = 0; readPorts[i]; i++ ) {
                a << QString( readPorts[i] );
            }

            jack_free(readPorts);
        }
        while ( 0 );
    }

    if (getWritePorts) {
        do {
            const char **writePorts;
            writePorts = jack_get_ports(client, NULL, "audio", JackPortIsInput);
            if ( !writePorts ) {
                break;
            }

            for (int i = 0; writePorts[i]; i++ ) {
                a << QString( writePorts[i] );
            }
            jack_free(writePorts);
        }
        while ( 0 );
    }

    if ( !SecretAudio::singleton ) {
        jack_client_close( client );
    }

    return a;
}

QStringList jackGetInputPorts() {
    return jackGetPorts( 0, true );
}

QStringList jackGetOutputPorts() {
    return jackGetPorts( true, 0 );
}

void jackConnectPorts( QString readPort, QString writePort, bool disconnect ) {
    // Josh est cheap.
    live_mutex(SecretAudio::x_sa) {
        jack_client_t* client=0;
        if ( !SecretAudio::singleton ) client = jack_client_open("HathorListen", JackNullOption, NULL);
        else client = SecretAudio::singleton->client;
        if (disconnect) {
            jack_disconnect( client, readPort.toAscii(), writePort.toAscii() );
//            system( QString( QString("jack_disconnect \"") + readPort + QString("\" \"") + writePort + QString("\" >/dev/null 2>/dev/null") ).toAscii() );
        } else {
            jack_connect( client, readPort.toAscii(), writePort.toAscii() );
//            system( QString( QString("jack_connect \"") + readPort + QString("\" \"") + writePort + QString("\" >/dev/null 2>/dev/null") ).toAscii() );
        }
        if ( !SecretAudio::singleton ) {
            jack_client_close( client );
        }
    }
}

void jackDisconnectPorts( QString readPort, QString writePort ) {
    jackConnectPorts( readPort, writePort, true );
}

///////////////////////////////////////////////////////////////////////////////////////
// Physical Ports
///////////////////////////////////////////////////////////////////////////////////////

LIBLIVECORESHARED_EXPORT int AudioIn::lastDeviceInternalID=-1;

void AudioIn::init() {
    live_mutex(SecretAudio::x_sa) {
        for (int i=0; i<chans; i++) {
            QString name=this->name();
            name.replace(' ','_');
            ++lastDeviceInternalID;
            qDebug() << s_name << "is connected to" << s_realnames[i];
            s_port_.push_back(SecretAudio::singleton->getInputPort());
            jackConnectPorts( s_realnames[i],SecretAudio::singleton->getInputPortId(), 0 );
        }
    }
}

void AudioIn::proc()
{
    if(!s_suspend)
    {
        for(int i=0; i<chans; i++)
        {
            float* _buffer=(float*)jack_port_get_buffer(s_port_[i],live::audio::nFrames());
            aOut(_buffer, i, this);
        }
    }
}

void AudioOut::init() {
    live_mutex(SecretAudio::x_sa) {
        for (int i=0; i<chans; i++) {
            s_port_[i].push_back(SecretAudio::singleton->getOutputPort());

            QString id = SecretAudio::singleton->getOutputPortId();
            qDebug() << "I am" << s_name;
            qDebug() << id << "is" << s_realnames[i];
            jackConnectPorts(id , s_realnames[i], 0 );
        }
    }
//    jack_cycle_wait(getJackClient()); (FIXME)
}

void AudioOut::aIn(const float*data, int chan, live::Object*) {
    s_processed = 1;
    Q_ASSERT(chan<chans);

    // TODO: DEPRICATE out. Replaced with mixer.

    live_mutex(SecretAudio::x_sa) {
        jack_default_audio_sample_t* out0 = (float*) jack_port_get_buffer( s_port_[chan][0], SecretAudio::singleton->nframes );

        if ( out0 ) {
            memcpy (out0, data, sizeof (jack_default_audio_sample_t) * SecretAudio::singleton->nframes );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////
// LOSE 32
///////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
QString exec(QString cmd) { // port to QProcess?
    QStringList sl=cmd.split(' ');
    for (int i=0;i<sl.size();i++) {
        if (sl[i].startsWith("\"")) {
            while (sl[i].startsWith("\"")&&(i+1<sl.size())&&!sl[i].endsWith("\"")) {
                sl[i]+=" "+sl.takeAt(i+1);
            }
            sl[i].remove(0,1);
            sl[i].chop(1);
        }
    }
    if (!sl.size()) {
        qDebug() << "Invalid call to exec(...)";
        return "ERROR";
    }
    if (sl[0].startsWith("jack")) {
        sl[0]="\""+QDir::homePath().replace("/","\\")+"\\Hathor\\"+sl[0]+"\"";
    }
    QProcess* proc=new QProcess;
    proc->setWorkingDirectory(QDir::homePath().replace("/","\\")+"\\Hathor");
    proc->start(sl.takeFirst(),sl,QIODevice::ReadOnly);
    if (!proc->waitForStarted(3000)) {
        QMessageBox::critical(0,"Can't talk to otherApp","Please ensure that Creator Live is installed correctly",QMessageBox::Close);
        return "ERROR";
    }
    QString result = "";
    if (!cmd.contains("-p")) {
        while (!(proc->atEnd()&&(proc->state()==QProcess::NotRunning))) {
            bool ok=1;
            while (proc->atEnd()) {
                if (proc->state()==QProcess::NotRunning) {
                    ok=0;
                    break;
                }
                proc->waitForReadyRead(300);
            }
            if (!ok) {
                break;
            }
            result+=proc->readLine();
            if (result.indexOf("End of list")!=-1) {
                break;
            }
        }
        delete proc;
    }
//    if (!cmd.contains("-p"))  // hack, but works
//    {
//        while ( !feof(pipe) )
//        {
//
//    qApp->processlive::Events();
//            if (fgets(buffer, 128, pipe) != NULL) result += buffer;
//            if ( result.indexOf("End of list") != -1 ) break;
//        }
//    }
//    pclose(pipe);
    return result;
}

QStringList parse( QString a ) {
    QStringList device;

    QTextStream ss(&a);
    QString l;
    while ( l = ss.readLine(), l != "" ) {
        QStringList ll = l.split(QRegExp("\\s+"));
        if ( ll.size() > 2 && ll[0] == "Name" ) {
            QString ds;
            for ( int i = 2; i < ll.size(); i++ ) {
                if ( i != 2 ) ds += ' ';
                ds += ll[i];
            }
            device.append( ds );
        }
    }

    return device;
}

QList<int> parseAutoType( QString a ) {
    QList<int> device;

    QTextStream ss(&a);
    QString l;
    bool good=0;
    while ( l = ss.readLine(), l != "" ) {
        QStringList ll = l.split(QRegExp("\\s+"));
        if ( ll.size() > 2 && ll[0] == "Name" ) {
            if ((good=l.contains("ASIO"))) {
                device.push_back(0);
            }
        }

        if (!good) {
            continue;
        }

        if ( ll.size() > 2 && ll[0] == "Max" && ll[1]=="inputs" ) {
            device.back()+=(ll[3].toInt()==0)?0:1;
        } else if ( ll.size() > 2 && ll[0] == "Max" && ll[1]=="outputs" ) {
            device.back()+=(ll[3].toInt()==0)?0:2;
        }
    }

    return device;
}

#endif

///////////////////////////////////////////////////////////////////////////////////////
// SecretAudio
///////////////////////////////////////////////////////////////////////////////////////
LIBLIVECORESHARED_EXPORT SecretAudio* SecretAudio::singleton = 0;
LIBLIVECORESHARED_EXPORT int SecretAudio::XRUNS = 0;

jack_client_t* live_private::getJackClient() {
    if (SecretAudio::singleton) return SecretAudio::singleton->client;
    else return (new SecretAudio)->client;
}

bool SecretAudio::makeClient() {
    if (client) return 1;
    Q_ASSERT(qApp);
    live::audio::resetKey();
    live::Object::beginProc();
    {
        qDebug() << "Trying to connect to jackd... (you can start the JACK server with qjackctl, for example...)";
        QObject::connect(qApp,SIGNAL(aboutToQuit()),SecretAudioShutdownHandler::singleton,SLOT(byeBye()));
        SecretAudioShutdownHandler::singleton->byeBye();
        client = jack_client_open(live::audio::getKey().toAscii(), (jack_options_t) (JackNoStartServer | JackUseExactName), 0);
        if (!client) {
#ifdef _WIN32
            QMessageBox::critical(0,"Error","Please ensure that Live is installed correctly.");
            exit(0); return;
#else
            //        QMessageBox::critical(0,"Error","Couldn't connect to JACK. Is it running?");
            //Unimplemented functionality
            qDebug("Couldn't connect to JACK!");
            s_error="Couldn't connect to Jack server!";
            live::Object::endProc(true);
            return 0;
#endif
        }

        qDebug() << "Connected to jackd... Yay!";
        jack_set_process_callback( client, jackCallback, 0 );
        jack_set_xrun_callback(client, xrunCallback, 0);
        jack_on_info_shutdown(client, QUITCALLBACK, 0);
    }

    live::Object::endProc(true);

    for (int i = 0; i < 64; ++i) {
        s_availInPorts.push_back( jack_port_register(getJackClient(),QString("clIn"+QString::number(i)).toAscii(),JACK_DEFAULT_AUDIO_TYPE,JackPortIsInput,0) );
        s_availInPortIds.push_back(QString(live::audio::getKey()+":clIn"+QString::number(i)));
        s_availOutPorts.push_back( jack_port_register(getJackClient(),QString("clOut"+QString::number(i)).toAscii(),JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0));
        s_availOutPortIds.push_back(QString(live::audio::getKey()+":clOut"+QString::number(i)));
    }

    refresh();
    jack_activate( client );    // before refresh
    kill_kitten;

    return 1;
}

SecretAudio::SecretAudio()
  : s_error()
  , s_availInPorts()
  , s_availInPortIds()
  , s_availOutPorts()
  , s_availOutPortIds()
  , nframes(1)
  , inputs()
  , outputs()
  , nulls()
  , inputMappings()
  , inputMappingsName()
  , outputMappings()
  , outputMappingsName()
  , client(NULL)
  , asioDeviceTypes()
  { connect(qApp,SIGNAL(destroyed()),this,SLOT(delClient()));
    singleton=this;
    makeClient();
//        live::audio::registerInterface(this);
}

bool SecretAudio::delClient() {
    jack_client_close(client);
    return 1;
}

void SecretAudio::process() {

    live::Object::beginProc();
    foreach( AudioOut* i, outputs ) {
        if ( i ) /*live_mutex(SecretAudio::x_sa)*/ {
            i->s_processed = false;
        }
    }
    foreach( AudioIn* i, inputs ) {
        if ( i ) /*live_mutex(SecretAudio::x_sa)*/ {
            i->proc();
        }
    }
    live::Object::endProc();
#ifdef _WIN32
    live::Object::beginProc();
    foreach(Vst* v, Vst::s_vst) {
        v->PROC_VST();
    }
    live::Object::endProc();

#endif

    live::Object::beginProc();
    float* buffer = new float[ nframes ]; //{
    foreach( AudioNull* i, nulls ) {
        for ( int j = 0; j < i->chans; j++ ) {
            for (unsigned k=0; k<nframes; k++) {
                buffer[k]=0.0;
            }
            i->aIn(buffer, j, 0);
        }
        foreach( AudioOut* i, outputs ) {
            if ( i && !i->s_processed) /*live_mutex(SecretAudio::x_sa)*/ {
                for (int j = 0; j < i->chans; ++j) {
                    for (unsigned k=0; k<nframes; k++) {
                        buffer[k]=0.0;
                    }
                    i->aIn(buffer, j, 0);
                }
            }
        }
    }
    delete[] buffer;                      //}
    live::Object::endProc();

    if (live::song::current()&&live::song::current()->metronome) {
//#ifdef __linux
//        QtConcurrent::run(SongSys::current->metronome,&Metronome::clock);
//#else
        live::song::current()->metronome->clock();
//#endif
    }
}

QMutex SecretAudio::x_sa(QMutex::Recursive);

bool SecretAudio::refresh() {
//    live::Object::beginProc();
    live_mutex(SecretAudio::x_sa) {
        for (int i=0;i<outputMappings;i++) {
            bool ok=1;
            for (int j=0;j<outputs;j++) {
                if (outputs[j]->name()==outputMappingsName[i]) {
                    ok=0;
                }
            }
            if (ok) {
                outputs.append( new AudioOut(outputMappings[i], outputMappingsName[i],1) );
            }
        }
        QStringList outputStrs = jackGetInputPorts();
        for (int i=0;i<outputStrs;i++) {
            if (outputStrs[i].contains(live::audio::getKey(),Qt::CaseInsensitive)) {
                continue;
            }
            bool ok=1;
            for (int j=0;j<outputs;j++) {
                if (outputs[j]->name()==outputStrs[i]) {
                    ok=0;
                    break;
                }
            }
            if (ok) {
                for (int j=0;j<outputMappings;j++) {
                    if (outputMappings[j].contains(outputStrs[i])) {
                        ok=0;
                        break;
                    }
                }
            }
            if (ok) {
                if (i+1<outputStrs.size()) {
                    //                QStringList adhocMapping;
                    //                adhocMapping.push_back(outputStrs[i]);
                    //                adhocMapping.push_back(outputStrs[i+1]);
                    //                outputMappings.push_back(adhocMapping);
                    //                outputMappingsName.push_back("Adhoc "+outputStrs[i]);
                    //                outputs.append( new AudioOut(adhocMapping, "Adhoc "+outputStrs[i],1) );
                }
            }
        }

        for (int i=0;i<inputMappings;i++) {
            bool ok=1;
            for (int j=0;j<inputs;j++) {
                if (inputs[j]->name()==inputMappingsName[i]) {
                    ok=0;
                }
            }
            if (ok) {
                inputs.append( new AudioIn(inputMappings[i], inputMappingsName[i],1) );
            }
        }
        QStringList inputStrs = jackGetOutputPorts();
        for (int i=0;i<inputStrs;i++) {
            if (inputStrs[i].contains(live::audio::getKey(),Qt::CaseInsensitive)) {
                continue;
            }
            bool ok=1;
            QString inputStrx=inputStrs[i];
            inputStrx.replace(':',"_");
            for (int j=0;j<inputs;j++) {
                if (inputs[j]->name()==inputStrx) {
                    ok=0;
                    break;
                }
            }
            if (ok) {
                for (int j=0;j<inputMappings;j++) {
                    if (inputMappings[j].contains(inputStrx)) {
                        ok=0;
                        break;
                    }
                }
            }
            if (ok) {
                if (i+1<inputStrs.size()) {
                    //                QStringList adhocMapping;
                    //                adhocMapping.push_back(inputStrx);
                    //                QString inputName=inputStrx;
                    //                inputStrx=inputStrs[i+1];
                    //                inputStrx.replace(':','_');
                    //                adhocMapping.push_back(inputStrx);
                    //                inputMappings.push_back(adhocMapping);
                    //                inputMappingsName.push_back("Adhoc "+inputName);
                    //                inputs.append( new AudioIn(adhocMapping, "Adhoc "+inputName,1) );
                }
            }
        }

        //store.
        live::object::clear(live::AudioOnly|live::InputOnly|live::NoRefresh);
        live::object::clear(live::AudioOnly|live::OutputOnly|live::NoRefresh);
        for (int i=0;i<inputs.size();i++) {
            live::object::set(inputs[i]);
        }
        for (int i=0;i<outputs.size();i++) {
            live::object::set(outputs[i]);
        }
    }
//    live::Object::endProc(true);
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////
// AudioSys
///////////////////////////////////////////////////////////////////////////////////////

live::AudioInterface* live::audio::s_audioInterface=0;
bool live::audio::strictInnocentXruns=false;

void live::audio::registerInterface(live::AudioInterface*ai) {
    if (s_audioInterface) s_audioInterface->delClient();
    s_audioInterface=ai;
    s_audioInterface->makeClient();
}

bool live::audio::valid() {
    return s_audioInterface ? s_audioInterface->valid() : false;
}

void live::audio::shutdown() {
    if (s_audioInterface) s_audioInterface->delClient();
    s_audioInterface = 0;
}

QObject* live::audio::getCurrentInterface() {
    return s_audioInterface ? s_audioInterface->qobject() : 0;
}

void live::audio::refresh() { if (s_audioInterface) s_audioInterface->refresh(); }

LIBLIVECORESHARED_EXPORT quint32 nframesv = 1;

LIBLIVECORESHARED_EXPORT const quint32& live::audio::nFrames() {
    if (!s_audioInterface) return nframesv;
    return nframesv=s_audioInterface->nFrames();
}

LIBLIVECORESHARED_EXPORT qint32 live::audio::sampleRate() {
    return s_audioInterface->sampleRate();
}

qint32 live_private::SecretAudio::sampleRate() {
    return client ? jack_get_sample_rate(client) : 0;
}

LIBLIVECORESHARED_EXPORT void live::audio::resetMappings() {
    s_audioInterface->resetMappings();
}

void live_private::SecretAudio::jack_disconnect(QString readPort,QString writePort) {
    ::jack_disconnect( client, readPort.toAscii(), writePort.toAscii() );
}


bool live_private::SecretAudio::resetMappings() {
    inputMappings.clear();
    inputMappingsName.clear();
    outputMappings.clear();
    outputMappingsName.clear();
    for (int i=0; i<inputs; i++) {
        Q_ASSERT(inputs[i]->aConnections.size()<1);
        delete inputs.takeAt(i);
    }
    for (int i=0; i<outputs; i++) {
        Q_ASSERT(outputs[i]->aConnections.size()<1);
        delete outputs.takeAt(i);
    }
    return refresh();
}

LIBLIVECORESHARED_EXPORT void live::audio::addMapping(QStringList mapping, bool input,QString name) {
    if (!valid()) return;
    s_audioInterface->addMapping(mapping,input,name);
}

bool live_private::SecretAudio::addMapping(QStringList mapping, bool input, QString name) {
    (input ? inputMappings : outputMappings ).push_back(mapping);

    (input ? inputMappingsName : outputMappingsName ).push_back(name);
    refresh();
    return 1;
}

LIBLIVECORESHARED_EXPORT int live::audio::mappingCount(bool input) {
    return s_audioInterface->mappingCount(input);
}

int live_private::SecretAudio::mappingCount(bool input) {
    return (input ? inputMappings : outputMappings ).size();
}

//QList<Object*> AudioSys::getInputPorts()
//{
//    if (!SecretAudio::singleton) new SecretAudio;

//    SecretAudio::singleton->refresh();

//    QList<Object*> list_;
//    for (int i=0; i<SecretAudio::singleton->inputs; i++)
//    {
//        list_.push_back(SecretAudio::singleton->inputs[i]);
//    }
//    return list_;
//}

LIBLIVECORESHARED_EXPORT QStringList live::audio::getInputChanStringList() {
    QStringList ret = s_audioInterface->getInputChanStringList();
    for (int i = 0; i < ret.size(); ++i) {
        if (ret[i].contains(live::audio::getKey()))
            ret.removeAt(i--);
    }
    return ret;
}

QStringList live_private::SecretAudio::getInputChanStringList() {
    return jackGetOutputPorts();
}

//QList<Object*> AudioSys::getOutputPorts()
//{
//    if (!SecretAudio::singleton) new SecretAudio;

//    SecretAudio::singleton->refresh();
//    QList<Object*> list_;
//    for (int i=0; i<SecretAudio::singleton->outputs; i++)
//    {
//        list_.push_back(SecretAudio::singleton->outputs[i]);
//    }
//    return list_;
//}

LIBLIVECORESHARED_EXPORT QStringList live::audio::getOutputChanStringList() {
    QStringList ret = s_audioInterface->getOutputChanStringList();
    for (int i = 0; i < ret.size(); ++i) {
        if (ret[i].contains(live::audio::getKey())) {
            ret.removeAt(i--);
        }
    }
    return ret;
}

QStringList live_private::SecretAudio::getOutputChanStringList() {
    return jackGetInputPorts();
}

LIBLIVECORESHARED_EXPORT live::ObjectPtr live::audio::null(int chan) {
    return s_audioInterface->getNull(chan);
}

live::ObjectPtr live_private::SecretAudio::getNull(int chans) {
    AudioNull* ret;

    live_mutex(SecretAudio::x_sa) kill_kitten {
        nulls.push_back( ret = new AudioNull(chans) );
        connect(nulls.back(), SIGNAL(destroyed(QObject*)), this, SLOT(removeNull(QObject*)), Qt::DirectConnection);
    }

    return ret;
}

void live_private::SecretAudio::removeNull(QObject* o) {
    live::Object::beginAsyncAction();
    nulls.removeAll(static_cast<AudioNull*>(o));
    live::Object::endAsyncAction(__FILE__,__LINE__);
}

//LIBLIVECORESHARED_EXPORT void live::audio::reset()
//{
//    Store::clear(live::AudioOnly|live::InputOnly|live::NoRefresh);
//    Store::clear(live::AudioOnly|live::OutputOnly|live::NoRefresh);
//    s_audioInterface->delClient();
//    delete s_audioInterface;
//    new SecretAudio;
//}

LIBLIVECORESHARED_EXPORT void live::audio::stop() {
    delete s_audioInterface;
    s_audioInterface=0;
}

LIBLIVECORESHARED_EXPORT QString live::audio::s_serverKey = "";
LIBLIVECORESHARED_EXPORT QString live::audio::getKey() {
    if (!s_serverKey.size()) resetKey();
    return s_serverKey;
}

LIBLIVECORESHARED_EXPORT void live::audio::resetKey() {
    s_serverKey = "live" + QString(QCryptographicHash::hash(QByteArray::number(QDateTime::currentDateTime().toMSecsSinceEpoch()), QCryptographicHash::Md5).toHex());
    s_serverKey.truncate(10);
}

///////////////////////////////////////////////////////////////////////////////////////
// Shutdown
///////////////////////////////////////////////////////////////////////////////////////

LIBLIVECORESHARED_EXPORT SecretAudioShutdownHandler* SecretAudioShutdownHandler::singleton = new SecretAudioShutdownHandler ;

void SecretAudioShutdownHandler::byeBye() {
    //uncomment this line:
    //exec( "start /MIN taskkill /F /IM jackd.exe" );
    //exec( "start /MIN taskkill /F /IM sooperlooper.exe" );
}
