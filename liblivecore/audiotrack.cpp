/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/audiotrack>
#include <live/object>

live::AudioSecondBank* live::AudioSecondBank::singleton = 0;

void live::AudioTrack::aThru(float*proc,int chan) {
    live::lthread::assertAudio();

    float* RAW;
    int size;
    int count;
    count=size=s_container[chan]->getRawPointer(s_curPos,RAW,s_playback&&(s_record||s_overdub));
    RAW-=RAW?1:0;
    unsigned i;
    qint64 cont = nframes;

    for (i=0; i<nframes; i++) {
        if ((RAW=RAW?RAW+1:0),--count==-1) {
            cont -= size;
            if (s_playback) s_container[chan]->appendGraph(size);
            count=size=s_container[chan]->getRawPointer(s_curPos+i,RAW,s_playback&&(s_record||s_overdub));
        }

        float* b = &(proc[i]);

        float swap = RAW?*RAW:0;

        if (RAW&&s_playback&&(s_record||s_overdub)) {
            *RAW = *b + ((float)s_overdub)* (*RAW);
            *RAW = (*RAW)> 1.0f ?  1.0f : (*RAW);
            *RAW = (*RAW)<-1.0f ? -1.0f : (*RAW);
        }

        *b += s_playback ? swap*(((float)s_vol)/100.0f) : 0.0f;
        *b = s_mute ? 0.0f : *b;

        if (s_chans==2) {
            *b *= (!chan&&s_pan>50) ? (100.0f-(float)s_pan)/50.0f : 1.0f;
            *b *= (chan&&s_pan<50) ? (float)s_pan / 50.0f : 1.0f;
            *b = ((*b)>1.0f) ? 1.0f : *b;
            *b = ((*b)<-1.0f) ? -1.0f : *b;
        }
    }

    if (s_playback) {
        s_container[chan]->appendGraph(cont);
        s_curPos += (chan==s_chans-1) ? nframes : 0;
    }

    if (s_updateCounter>=80.0/float(live::audio::nFrames())*128.0) {
        if (s_updateCounter) emit dataUpdated((qint64)(s_curPos-nframes*s_updateCounter),(qint64)(s_curPos));
        s_updateCounter=0;
    } else {
        ++s_updateCounter;
    }
}

live::AudioTrack::AudioTrack(int cchans)
  : live::Object("Audio Track",false,false,cchans)
  , nframes(live::audio::nFrames())
  , s_container( new AudioContainer*[cchans] )
  , s_chans(cchans)
  , s_curPos(0)
  , s_vol(100)
  , s_pan(50)
  , s_record(0)
  , s_overdub(0)
  , s_playback(0)
  , s_mute(0)
  , s_simpleCount(0)
  , s_updateCounter(0)
  , m_proc(new float[live::audio::nFrames()])
  { setTemporary(0);
    for (int i=0; i<cchans; i++) {
        s_container[i]=new AudioContainer;
    }
    setPos(0.0f);
}

live::AudioTrack::~AudioTrack() {
    for (int i=0;i<s_chans;i++) {
        delete s_container[i];
    }
    delete[]s_container;
}


const int& live::AudioTrack::volume() const {
    return s_vol;
}

const int& live::AudioTrack::pan() const {
    return s_pan;
}

const bool& live::AudioTrack::isRecord() const {
    return s_record;
}

const bool& live::AudioTrack::isOverdub() const {
    return s_overdub;
}

const bool& live::AudioTrack::isPlay() const {
    return s_playback;
}

const bool& live::AudioTrack::isMute() const {
    return s_mute;
}

float live::AudioTrack::pos() const {
    return (float)s_curPos*1000.0f/(float)live::audio::sampleRate();
}

float live::AudioTrack::length() const {
    return qMax(s_container[0]->length(), s_container[0]->length());
}

void live::AudioTrack::setVol(int vol) {
    Q_ASSERT((vol>=0)&&(vol<=100));
    s_vol=vol;
}

void live::AudioTrack::setPan(int pan) {
    Q_ASSERT((pan>=0)&&(pan<=100));
    Q_ASSERT(s_chans==2);
    s_pan=pan;
}

void live::AudioTrack::startRecord() {
    Q_ASSERT(!s_record);
    Q_ASSERT(!s_overdub);
    s_record=1;
    float* a;
    for (int i = 0; i < s_chans; ++i) {
        s_container[i]->getRawPointer((unsigned)s_curPos,a,1,0);
        s_container[i]->pointGraph(s_curPos);
    }
}

void live::AudioTrack::stopRecord() {
    Q_ASSERT(s_record);
    Q_ASSERT(!s_overdub);
    s_record=0;
}

void live::AudioTrack::startOverdub() {
    Q_ASSERT(!s_overdub);
    Q_ASSERT(!s_record);
    s_overdub=1;
    float* a;
    for (int i = 0; i < s_chans; ++i) {
        s_container[i]->getRawPointer((unsigned)s_curPos,a,1,0);
        s_container[i]->pointGraph(s_curPos);
    }
}

void live::AudioTrack::stopOverdub() {
    Q_ASSERT(s_overdub);
    Q_ASSERT(!s_record);
    s_overdub=0;
}

void live::AudioTrack::startPlayback() {
    Q_ASSERT(!s_playback);
    s_playback=1;
}

void live::AudioTrack::stopPlayback() {
    Q_ASSERT(s_playback);
    s_playback=0;
}

void live::AudioTrack::startMute() {
    Q_ASSERT(!s_mute);
    s_mute=1;
}

void live::AudioTrack::stopMute() {
    Q_ASSERT(s_mute);
    s_mute=0;
}

void live::AudioTrack::setPos(float pos) {
    // TODO: evaluate threadsafety
    s_curPos = long((float(live::audio::sampleRate()))*(float(pos)/1000.0f));
    float* a;
    for (int i = 0; i < s_chans; ++i) {
        s_container[i]->getRawPointer((unsigned)s_curPos,a,1,0);
        s_container[i]->pointGraph(s_curPos);
    }
}

void live::AudioTrack::aIn(const float *in, int chan, Object*) {
    if (!s_playback && !(s_record||s_overdub)) {
        aOut(in, chan, this);
        return;
    }
    memcpy(m_proc,in,sizeof(float)*nframes);
    aThru(m_proc,chan);
    aOut(m_proc,chan,this);
}

void live::AudioTrack::clearData() {
    qint64 dataSize=qMax(s_container[0]->s_data.size(),s_container[0]->s_data.size())*live::audio::sampleRate();
    for (int h=0;h<s_chans;h++) {
        s_container[h]->clear();
        s_container[h]->pointGraph(0);
        s_container[h]->appendGraph(dataSize);
        s_container[h]->pointGraph(s_curPos);
    }
    emit dataUpdated(0,dataSize);
}

void live::AudioTrack::clearData(const qint64 &a, const qint64 &b) {
    //FIXME: delete data in another thread (i.e., with Container::clear)
    for (int h=0;h<s_chans;h++) {
        float*dataPtr;
        int size;
        int counter=size=qint64(s_container[h]->getRawPointer(qint64(a),dataPtr));
        dataPtr-=dataPtr?1:0;

        for (qint64 i=a;i<b;++i) {
            if ((dataPtr+=dataPtr?1:0),--counter==-1) {
                counter=size=s_container[h]->getRawPointer(i,dataPtr)-1;
                if (i + counter < b) {
                    qint64 id=(qint64)(i/live::audio::sampleRate());
                    kill_kitten {
                        live::AudioSecond* toDelete = s_container[h]->s_data[id];
                        s_container[h]->s_data[id] = live::AudioSecondBank::singleton->buySecond(i);

                        live::AudioContainer::AudioDataDestroyer* add =
                                new live::AudioContainer::AudioDataDestroyer(toDelete,0);
                        QtConcurrent::run(add,&live::AudioContainer::AudioDataDestroyer::run);
                        i += counter;
                        counter = 0;
                    }
                    s_container[h]->regSecond(s_container[h]->s_data[id]);
                    continue;
                }
            }
            if (dataPtr) {
                *dataPtr=0.0f;
            }
        }

        for(qint64 i = a; i < b + 22100; i += 22100) kill_kitten {
            s_container[h]->pointGraph(i);
            s_container[h]->appendGraph(22100);
            s_container[h]->pointGraph(s_curPos);
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
                s_chans,
                live::audio::sampleRate() );

    if (!file) {
        qDebug() << "sndfile refuses to open "<<filename<<"...";
        return 0;
    }

    qint64 frames=qMax(s_container[0]->s_data.size(),s_container[0]->s_data.size())*qint64(live::audio::sampleRate());
    float*data=new float[frames*s_chans];  //s_chans==chans, no of items, data is interlaced
    for (int i=0;i<s_chans;i++) {
        float*dataPtr;
        qint64 counter=s_container[i]->getRawPointer(0,dataPtr);
        for (qint64 j=0;j<s_container[i]->s_data.size()*live::audio::sampleRate();j++) {
            data[j*s_chans+i]=dataPtr?*dataPtr:0.0f;
            if (j!=frames/s_chans) {
                if ((dataPtr?++dataPtr:0),--counter==-1) {
                    counter=s_container[i]->getRawPointer(j,dataPtr);
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
    for (int i = 0; i < s_chans; ++i)
        s_container[i]->clear();

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
    float*data=new float[frames*s_chans];  //chans==s_chans, no of items, data is interlaced
    file.readf(data,frames);
    for (int i=0;i<s_chans;i++) {
        float*dataPtr;
        qint64 size;
        qint64 counter=size=frames?(s_container[i]->getRawPointer(0,dataPtr,1)):0;
        dataPtr--;
        for (qint64 j = 0; j < frames/s_chans; ++j) {
            if (j!=frames/s_chans) {
                if ((dataPtr?++dataPtr:0),--counter==-1) {
                    counter=size=s_container[i]->getRawPointer(j,dataPtr,1)-1;
                }
                (*dataPtr)=data[j*s_chans+i];
            }
        }
    }

    qint64 dataSize=qMax(s_container[0]->s_data.size(),s_container[0]->s_data.size())*live::audio::sampleRate();
    for (int c = 0; c < s_chans; ++c)
        for (qint64 i = 0; i < dataSize + 22100; i += 22100) kill_kitten {
            s_container[c]->pointGraph(i);
            s_container[c]->appendGraph(22100);
            s_container[c]->pointGraph(s_curPos);
            emit dataUpdated(i,22100);
        }
#endif
    return 1;
}
