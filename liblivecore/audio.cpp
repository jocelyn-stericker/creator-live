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

#if defined(_WIN32)
#include <QDir>
#include <QProcess>
#include <QMessageBox>
#endif

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
            jack_disconnect( client, readPort.toLatin1(), writePort.toLatin1() );
        } else {
            jack_connect( client, readPort.toLatin1(), writePort.toLatin1() );
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
            m_port_.push_back(SecretAudio::singleton->getInputPort());
            jackConnectPorts( m_realnames[i],SecretAudio::singleton->getInputPortId(), 0 );
        }
    }
}

void AudioIn::proc()
{
    if(!m_suspend)
    {
        for(int i=0; i<chans; i++)
        {
            float* _buffer=(float*)jack_port_get_buffer(m_port_[i],live::audio::nFrames());
            aOut(_buffer, i, this);
        }
    }
}

void AudioOut::init() {
    live_mutex(SecretAudio::x_sa) {
        for (int i=0; i<chans || i < m_realnames.size(); i++) {
            m_port_[i].push_back(SecretAudio::singleton->getOutputPort());

            QString id = SecretAudio::singleton->getOutputPortId();
            jackConnectPorts(id , m_realnames[i], 0 );
        }
    }
}

void AudioOut::aIn(const float*data, int chan, live::Object*p) {
    if (!chan) {
        Q_ASSERT (!m_processed);
        m_processed = 1;
    }
    Q_ASSERT(chan<chans);

//    live_mutex(SecretAudio::x_sa) {
    if (m_port_[chan].size()) {
        jack_default_audio_sample_t* out0 = (float*) jack_port_get_buffer( m_port_[chan][0], SecretAudio::singleton->nframes );

        if ( out0 ) {
            memcpy (out0, data, sizeof (jack_default_audio_sample_t) * SecretAudio::singleton->nframes );
        }
    }

//    }
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
        std::cout << "Connecting to the JACK Audio Connection Kit... ";
        client = jack_client_open(live::audio::getKey().toLatin1(), (jack_options_t) (JackNoStartServer | JackUseExactName), 0);
        if (!client) {
#ifdef _WIN32
            QMessageBox::critical(0,"Error","Please ensure that Live is installed correctly.");
            exit(0);
            return 0;
#else
            std::cout << "FAILED! Please make sure that JACK is running (try qjackctl)\n";
            //        QMessageBox::critical(0,"Error","Couldn't connect to JACK. Is it running?");
            //Unimplemented functionality
            m_error="Couldn't connect to Jack server!";
            live::Object::endProc(true);
            return 0;
#endif
        }
        std::cout << "Done\n";

        qDebug() << "Connected to jackd... Yay!";
        jack_set_process_callback( client, jackCallback, 0 );
        jack_set_xrun_callback(client, xrunCallback, 0);
        jack_on_info_shutdown(client, QUITCALLBACK, 0);
    }

    live::Object::endProc(true);

    for (int i = 0; i < 32; ++i) {
        m_availInPorts.push_back( jack_port_register(getJackClient(),QString("clIn"+QString::number(i)).toLatin1(),JACK_DEFAULT_AUDIO_TYPE,JackPortIsInput,0) );
        m_availInPortIds.push_back(QString(live::audio::getKey()+":clIn"+QString::number(i)));
        m_availOutPorts.push_back( jack_port_register(getJackClient(),QString("clOut"+QString::number(i)).toLatin1(),JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0));
        m_availOutPortIds.push_back(QString(live::audio::getKey()+":clOut"+QString::number(i)));
    }

    refresh();
    jack_activate( client );    // before refresh
    kill_kitten;

    return 1;
}

SecretAudio::SecretAudio()
  : m_error()
  , m_availInPorts()
  , m_availInPortIds()
  , m_availOutPorts()
  , m_availOutPortIds()
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
    if (!client) return 0;
    jack_client_close(client);
    client = 0;
    return 1;
}

void SecretAudio::process() {

    live::Object::beginProc();
    float* buffer = new float[ nframes ]; //{

    foreach( AudioOut* i, outputs ) {
        if ( i ) /*live_mutex(SecretAudio::x_sa)*/ {
            i->m_processed = false;
        }
    }
    foreach( AudioIn* i, inputs ) {
        if ( i ) /*live_mutex(SecretAudio::x_sa)*/ {
            kill_kitten i->proc();
        }
    }
//#ifdef _WIN32 FIXME : -> null
//    live::Object::beginProc();
//    foreach(Vst* v, Vst::m_vst) {
//        v->PROC_VST();
//    }
//    live::Object::endProc();
//#endif

    foreach( AudioNull* i, nulls ) {
        for ( int j = 0; j < i->chans; j++ ) {
            for (unsigned k=0; k<nframes; k++) {
                buffer[k]=0.0;
            }
            kill_kitten i->aIn(buffer, j, 0);
        }

    }

    foreach( AudioOut* i, outputs ) {
        if ( i && !i->m_processed) /*live_mutex(SecretAudio::x_sa)*/ {
            for (int j = 0; j < i->chans; ++j) {
                for (unsigned k=0; k<nframes; k++) {
                    buffer[k]=0.0;
                }
                kill_kitten i->aIn(buffer, j, 0);
            }
        }
    }
    live::Object::endProc();
    delete[] buffer;                      //}

    if (live::song::current()&&live::song::current()->metronome) {
        live::song::current()->metronome->clock();
    }
}

QMutex SecretAudio::x_sa(QMutex::Recursive);

bool SecretAudio::refresh() {
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
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////
// AudioSys
///////////////////////////////////////////////////////////////////////////////////////

live::AudioInterface* live::audio::m_audioInterface=0;
bool live::audio::strictInnocentXruns=false;

void live::audio::registerInterface(live::AudioInterface*ai) {
    if (m_audioInterface) m_audioInterface->delClient();
    m_audioInterface=ai;
    m_audioInterface->makeClient();
}

bool live::audio::valid() {
    return m_audioInterface ? m_audioInterface->valid() : false;
}

void live::audio::shutdown() {
    if (m_audioInterface) m_audioInterface->delClient();
    m_audioInterface = 0;
}

QObject* live::audio::getCurrentInterface() {
    return m_audioInterface ? m_audioInterface->qobject() : 0;
}

void live::audio::refresh() { if (m_audioInterface) m_audioInterface->refresh(); }

LIBLIVECORESHARED_EXPORT quint32 nframesv = 1;

LIBLIVECORESHARED_EXPORT const quint32& live::audio::nFrames() {
    if (!m_audioInterface) return nframesv;
    return nframesv=m_audioInterface->nFrames();
}

LIBLIVECORESHARED_EXPORT qint32 live::audio::sampleRate() {
    return m_audioInterface->sampleRate();
}

qint32 live_private::SecretAudio::sampleRate() {
    return client ? jack_get_sample_rate(client) : 0;
}

LIBLIVECORESHARED_EXPORT void live::audio::resetMappings() {
    m_audioInterface->resetMappings();
}

void live_private::SecretAudio::jack_disconnect(QString readPort,QString writePort) {
    ::jack_disconnect( client, readPort.toLatin1(), writePort.toLatin1() );
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
    m_audioInterface->addMapping(mapping,input,name);
}

bool live_private::SecretAudio::addMapping(QStringList mapping, bool input, QString name) {
    (input ? inputMappings : outputMappings ).push_back(mapping);

    (input ? inputMappingsName : outputMappingsName ).push_back(name);
    refresh();
    return 1;
}

LIBLIVECORESHARED_EXPORT int live::audio::mappingCount(bool input) {
    return m_audioInterface->mappingCount(input);
}

int live_private::SecretAudio::mappingCount(bool input) {
    return (input ? inputMappings : outputMappings ).size();
}

LIBLIVECORESHARED_EXPORT QStringList live::audio::getInputChanStringList() {
    QStringList ret = m_audioInterface->getInputChanStringList();
    for (int i = 0; i < ret.size(); ++i) {
        if (ret[i].contains(live::audio::getKey()))
            ret.removeAt(i--);
    }
    return ret;
}

QStringList live_private::SecretAudio::getInputChanStringList() {
    return jackGetOutputPorts();
}

LIBLIVECORESHARED_EXPORT QStringList live::audio::getOutputChanStringList() {
    QStringList ret = m_audioInterface->getOutputChanStringList();
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
    return m_audioInterface->getNull(chan);
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

LIBLIVECORESHARED_EXPORT void live::audio::stop() {
    delete m_audioInterface;
    m_audioInterface=0;
}

LIBLIVECORESHARED_EXPORT QString live::audio::m_serverKey = "";
LIBLIVECORESHARED_EXPORT QString live::audio::getKey() {
    if (!m_serverKey.size()) resetKey();
    return m_serverKey;
}

LIBLIVECORESHARED_EXPORT void live::audio::resetKey() {
    m_serverKey = "live" + QString(QCryptographicHash::hash(QByteArray::number(QDateTime::currentDateTime().toMSecsSinceEpoch()), QCryptographicHash::Md5).toHex());
    m_serverKey.truncate(10);
}
