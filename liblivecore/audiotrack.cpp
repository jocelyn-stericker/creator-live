/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/audiotrack>
#include <live/object>

#include <QtConcurrentRun>

live::AudioSecondBank* live::AudioSecondBank::singleton = 0;

void live::AudioTrack::aThru(float*proc,int chan) {
    live::lthread::assertAudio();

    float* RAW;
    int size;
    int count;
    count=size=m_container[chan]->getRawPointer(m_curPos,RAW,m_playback&&(m_record||m_overdub));
    RAW-=RAW?1:0;
    unsigned i;
    qint64 cont = nframes;

    for (i=0; i<nframes; i++) {
        if ((RAW=RAW?RAW+1:0),--count==-1) {
            cont -= size;
            if (m_playback) m_container[chan]->appendGraph(size);
            count=size=m_container[chan]->getRawPointer(m_curPos+i,RAW,m_playback&&(m_record||m_overdub));
        }

        float* b = &(proc[i]);

        float swap = RAW?*RAW:0;

        if (RAW&&m_playback&&(m_record||m_overdub)) {
            *RAW = *b + ((float)m_overdub)* (*RAW);
            *RAW = (*RAW)> 1.0f ?  1.0f : (*RAW);
            *RAW = (*RAW)<-1.0f ? -1.0f : (*RAW);
        }

        *b += m_playback ? swap*(((float)m_vol)/100.0f) : 0.0f;
        *b = m_mute ? 0.0f : *b;

        if (m_chans==2) {
            *b *= (!chan&&m_pan>50) ? (100.0f-(float)m_pan)/50.0f : 1.0f;
            *b *= (chan&&m_pan<50) ? (float)m_pan / 50.0f : 1.0f;
            *b = ((*b)>1.0f) ? 1.0f : *b;
            *b = ((*b)<-1.0f) ? -1.0f : *b;
        }
    }

    if (m_playback) {
        m_container[chan]->appendGraph(cont);
        m_curPos += (chan==m_chans-1) ? nframes : 0;
    }

    if (m_updateCounter>=80.0/float(live::audio::nFrames())*128.0) {
        if (m_updateCounter) emit dataUpdated((qint64)(m_curPos-nframes*m_updateCounter),(qint64)(m_curPos));
        m_updateCounter=0;
    } else {
        ++m_updateCounter;
    }
}

live::AudioTrack::AudioTrack(int cchans)
  : live::Object("Audio Track",false,false,cchans)
  , nframes(live::audio::nFrames())
  , m_container( new AudioContainer*[cchans] )
  , m_chans(cchans)
  , m_curPos(0)
  , m_vol(100)
  , m_pan(50)
  , m_record(0)
  , m_overdub(0)
  , m_playback(0)
  , m_mute(0)
  , m_simpleCount(0)
  , m_updateCounter(0)
  , m_proc(new float[live::audio::nFrames()])
  { setTemporary(0);
    for (int i=0; i<cchans; i++) {
        m_container[i]=new AudioContainer;
    }
    setPos(0.0f);
}

live::AudioTrack::~AudioTrack() {
    for (int i=0;i<m_chans;i++) {
        delete m_container[i];
    }
    delete[]m_container;
}


const int& live::AudioTrack::volume() const {
    return m_vol;
}

const int& live::AudioTrack::pan() const {
    return m_pan;
}

const bool& live::AudioTrack::isRecord() const {
    return m_record;
}

const bool& live::AudioTrack::isOverdub() const {
    return m_overdub;
}

const bool& live::AudioTrack::isPlay() const {
    return m_playback;
}

const bool& live::AudioTrack::isMute() const {
    return m_mute;
}

float live::AudioTrack::pos() const {
    return (float)m_curPos*1000.0f/(float)live::audio::sampleRate();
}

float live::AudioTrack::length() const {
    return qMax(m_container[0]->length(), m_container[0]->length());
}

void live::AudioTrack::setVol(int vol) {
    Q_ASSERT((vol>=0)&&(vol<=100));
    m_vol=vol;
}

void live::AudioTrack::setPan(int pan) {
    Q_ASSERT((pan>=0)&&(pan<=100));
    Q_ASSERT(m_chans==2);
    m_pan=pan;
}

void live::AudioTrack::startRecord() {
    Q_ASSERT(!m_record);
    Q_ASSERT(!m_overdub);
    m_record=1;
    float* a;
    for (int i = 0; i < m_chans; ++i) {
        m_container[i]->getRawPointer((unsigned)m_curPos,a,1,0);
        m_container[i]->pointGraph(m_curPos);
    }
}

void live::AudioTrack::stopRecord() {
    Q_ASSERT(m_record);
    Q_ASSERT(!m_overdub);
    m_record=0;
}

void live::AudioTrack::startOverdub() {
    Q_ASSERT(!m_overdub);
    Q_ASSERT(!m_record);
    m_overdub=1;
    float* a;
    for (int i = 0; i < m_chans; ++i) {
        m_container[i]->getRawPointer((unsigned)m_curPos,a,1,0);
        m_container[i]->pointGraph(m_curPos);
    }
}

void live::AudioTrack::stopOverdub() {
    Q_ASSERT(m_overdub);
    Q_ASSERT(!m_record);
    m_overdub=0;
}

void live::AudioTrack::startPlayback() {
    Q_ASSERT(!m_playback);
    m_playback=1;
}

void live::AudioTrack::stopPlayback() {
    Q_ASSERT(m_playback);
    m_playback=0;
}

void live::AudioTrack::startMute() {
    Q_ASSERT(!m_mute);
    m_mute=1;
}

void live::AudioTrack::stopMute() {
    Q_ASSERT(m_mute);
    m_mute=0;
}

void live::AudioTrack::setPos(float pos) {
    // TODO: evaluate threadsafety
    m_curPos = long((float(live::audio::sampleRate()))*(float(pos)/1000.0f));
    float* a;
    for (int i = 0; i < m_chans; ++i) {
        m_container[i]->getRawPointer((unsigned)m_curPos,a,1,0);
        m_container[i]->pointGraph(m_curPos);
    }
}

void live::AudioTrack::aIn(const float *in, int chan, Object*) {
    if (!m_playback && !(m_record||m_overdub)) {
        aOut(in, chan, this);
        return;
    }
    memcpy(m_proc,in,sizeof(float)*nframes);
    aThru(m_proc,chan);
    aOut(m_proc,chan,this);
}

void live::AudioTrack::clearData() {
    qint64 dataSize=qMax(m_container[0]->m_data.size(),m_container[0]->m_data.size())*live::audio::sampleRate();
    for (int h=0;h<m_chans;h++) {
        m_container[h]->clear();
        m_container[h]->pointGraph(0);
        m_container[h]->appendGraph(dataSize);
        m_container[h]->pointGraph(m_curPos);
    }
    emit dataUpdated(0,dataSize);
}

void live::AudioTrack::clearData(const qint64 &a, const qint64 &b) {
    //FIXME: delete data in another thread (i.e., with Container::clear)
    for (int h=0;h<m_chans;h++) {
        float*dataPtr;
        int size;
        int counter=size=qint64(m_container[h]->getRawPointer(qint64(a),dataPtr));
        dataPtr-=dataPtr?1:0;

        for (qint64 i=a;i<b;++i) {
            if ((dataPtr+=dataPtr?1:0),--counter==-1) {
                counter=size=m_container[h]->getRawPointer(i,dataPtr)-1;
                if (i + counter < b) {
                    qint64 id=(qint64)(i/live::audio::sampleRate());
                    kill_kitten {
                        live::AudioSecond* toDelete = m_container[h]->m_data[id];
                        m_container[h]->m_data[id] = live::AudioSecondBank::singleton->buySecond(i);

                        live::AudioContainer::AudioDataDestroyer* add =
                                new live::AudioContainer::AudioDataDestroyer(toDelete,0);
                        QtConcurrent::run(add,&live::AudioContainer::AudioDataDestroyer::run);
                        i += counter;
                        counter = 0;
                    }
                    m_container[h]->regSecond(m_container[h]->m_data[id]);
                    continue;
                }
            }
            if (dataPtr) {
                *dataPtr=0.0f;
            }
        }

        for(qint64 i = a; i < b + 22100; i += 22100) kill_kitten {
            m_container[h]->pointGraph(i);
            m_container[h]->appendGraph(22100);
            m_container[h]->pointGraph(m_curPos);
        }
    }
    emit dataUpdated(a,b);
}

int live::AudioTrack::formatForString(QString s,bool verifyAvailable) {
#if !defined(__QNX__) && !defined(_WIN32)
    Q_UNUSED(verifyAvailable);
    s=s.toLower();
    if (s.endsWith("wav")) return SF_FORMAT_WAV;
    if (s.endsWith("aiff")) return SF_FORMAT_AIFF;
    if (s.endsWith("au")) return SF_FORMAT_AU;
    if (s.endsWith("flac")) return SF_FORMAT_FLAC;
    if (s.endsWith("ogg")) return SF_FORMAT_OGG;
    Q_ASSERT(verifyAvailable);
#endif
    return -1;
}

int live::AudioTrack::rateForInt(int i) {
#if !defined(__QNX__) && !defined(_WIN32)
    switch (i) {
    case 16:
        return SF_FORMAT_PCM_16;
        break;
    case 24:
        return SF_FORMAT_PCM_24;
        break;
    case 32:
        return SF_FORMAT_PCM_32;
        break;
    default:
        return -1;
        Q_ASSERT(0);
        break;
    }
#endif
    return -1;
}

bool live::AudioTrack::exportFile(QString filename,QString format,int depth) {
#if !defined(_WIN32) && !defined(__QNX__)  // for now
    format=(format=="guess")?filename:format;
    if (depth!=24&&depth!=32&&depth!=16) {
        qDebug() << "Invalid PCM debth";
        return 0;
    }

    if (formatForString(format,1)==-1) {
        qDebug() << "Invalid format!";
        return 0;
    }

    SndfileHandle file(
                filename.toStdString(),
                SFM_WRITE,
                formatForString(format) | (format.toLower().endsWith("ogg")?SF_FORMAT_VORBIS:rateForInt(depth)),
                m_chans,
                live::audio::sampleRate() );

    if (!file) {
        qDebug() << "sndfile refuses to open "<<filename<<"...";
        return 0;
    }

    qint64 frames=qMax(m_container[0]->m_data.size(),m_container[0]->m_data.size())*qint64(live::audio::sampleRate());
    float*data=new float[frames*m_chans];  //m_chans==chans, no of items, data is interlaced
    for (int i=0;i<m_chans;i++) {
        float*dataPtr;
        qint64 counter=m_container[i]->getRawPointer(0,dataPtr);
        for (qint64 j=0;j<m_container[i]->m_data.size()*live::audio::sampleRate();j++) {
            data[j*m_chans+i]=dataPtr?*dataPtr:0.0f;
            if (j!=frames/m_chans) {
                if ((dataPtr?++dataPtr:0),--counter==-1) {
                    counter=m_container[i]->getRawPointer(j,dataPtr);
                }
            }
        }
    }
    file.writef(data,frames);
    delete[]data;
#endif
    return 1;

}

bool live::AudioTrack::importFile(QString filename) {
    lthread::assertUi();
#if !defined(_WIN32) && !defined(__QNX__)
    for (int i = 0; i < m_chans; ++i)
        m_container[i]->clear();

    SndfileHandle file( filename.toStdString(), SFM_READ, 0, 0, live::audio::sampleRate() );

    if (!file) {
        qDebug() << "sndfile refuses to open "<<filename<<"...";
        return 0;
    }

    if (file.samplerate()!=live::audio::sampleRate()) {
        qDebug() << "Expected sample rate"<<live::audio::sampleRate() << "but got incompatible"<<file.samplerate();
        return 0;
    }

    sf_count_t frames=file.frames();
    float*data=new float[frames*m_chans];  //chans==m_chans, no of items, data is interlaced
    file.readf(data,frames);
    for (int i=0;i<m_chans;i++) {
        float*dataPtr;
        qint64 size;
        qint64 counter=size=frames?(m_container[i]->getRawPointer(0,dataPtr,1)):0;
        dataPtr--;
        for (qint64 j = 0; j < frames/m_chans; ++j) {
            if (j!=frames/m_chans) {
                if ((dataPtr?++dataPtr:0),--counter==-1) {
                    counter=size=m_container[i]->getRawPointer(j,dataPtr,1)-1;
                }
                (*dataPtr)=data[j*m_chans+i];
            }
        }
    }

    qint64 dataSize=qMax(m_container[0]->m_data.size(),m_container[0]->m_data.size())*live::audio::sampleRate();
    for (int c = 0; c < m_chans; ++c)
        for (qint64 i = 0; i < dataSize + 22100; i += 22100) kill_kitten {
            m_container[c]->pointGraph(i);
            m_container[c]->appendGraph(22100);
            m_container[c]->pointGraph(m_curPos);
            emit dataUpdated(i,22100);
        }
#endif
    return 1;
}

