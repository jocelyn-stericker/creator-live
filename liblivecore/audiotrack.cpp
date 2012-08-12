/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/audiotrack>

void live::AudioTrack::aThru(float*proc,int chan) {
    // TODO: Evaluate threadsafety.

    bool ok=1;

    float* RAW;
    bool warning = false;
    int count=s_container[chan]->getRawPointer(s_curPos,RAW,s_playback&&(s_record||s_overdub), &warning);
    if (warning) {
        QtConcurrent::run(this,&live::AudioTrack::async);
    }
    RAW-=RAW?1:0;
    int i;
    for (i=0; i<nframes; i++) {
        if ((RAW=RAW?RAW+1:0),--count==-1) {
            count=s_container[chan]->getRawPointer(s_curPos+i,RAW,s_playback&&(s_record||s_overdub));
        }

        float* b = &(proc[i]);

        float swap = RAW?*RAW:0;

        if (RAW&&s_playback&&(s_record||s_overdub)) {
            if (*RAW!=*b + ((float)s_overdub)* (*RAW)) ok=0;
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

    if ((ok)||s_updateCounter==4) {
        if (s_updateCounter) emit dataUpdated((int)(s_curPos-nframes*s_updateCounter),(int)(s_curPos));
        else if (++s_boringCounter==4) {
            s_boringCounter=0;
            emit locationChanged(s_curPos);
        }
        s_updateCounter=0;
    } else {
        s_boringCounter=0;
        ++s_updateCounter;
    }

    s_curPos += s_playback&&(chan==s_chans-1) ? nframes : 0;
}

live::AudioTrack::AudioTrack(int cchans) :
    live::Object("Audio Track",false,false),
    nframes(live::audio::nFrames()),
    s_container( new AudioContainer*[cchans] ),
    s_chans(cchans),
    s_curPos(0),
    s_vol(100),
    s_pan(50),
    s_record(0),
    s_overdub(0),
    s_playback(0),
    s_mute(0),
    s_simpleCount(0),
    s_updateCounter(0),
    s_boringCounter(0) {
    setTemporary(0);
    for (int i=0; i<cchans; i++) {
        s_container[i]=new AudioContainer;
    }
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
    return qMax(s_container[0]->length(), s_container[1]->length());
}

void live::AudioTrack::setVol(int vol) {
    ;
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

void live::AudioTrack::setPos(long pos) {
    // TODO: evaluate threadsafety
    pos=(long)(((float)live::audio::sampleRate())*((float)pos)/1000.0f);
    s_curPos=pos;
}

void live::AudioTrack::aIn(const float *in, int chan, ObjectChain*p) {
    float* proc=new float[nframes];
    memcpy(proc,in,sizeof(float)*nframes);

    aThru(proc,chan);

    p->push_back(this);
    aOut(proc,chan,p);
    p->pop_back();
    delete[] proc;
}

void live::AudioTrack::clearData() {
    int dataSize=qMax(s_container[0]->s_data.size(),s_container[1]->s_data.size())*live::audio::sampleRate();
    for (int h=0;h<s_chans;h++) {
        s_container[h]->clear();
    }
    emit dataUpdated(0,dataSize);
}

void live::AudioTrack::clearData(const float &a, const float &b) {
    //cannot delete container.
    for (int h=0;h<s_chans;h++) {
        float*dataPtr;
        int counter=s_container[h]->getRawPointer(a,dataPtr);
        dataPtr-=dataPtr?1:0;

        for (int i=a;i<b;i++) {
            if ((dataPtr+=dataPtr?1:0),--counter==-1) {
                counter=s_container[h]->getRawPointer(i,dataPtr)-1;
            }
            if (dataPtr) {
                *dataPtr=0.0f;
            }
        }
    }
}

int live::AudioTrack::formatForString(QString s,bool verifyAvailable) {
#ifndef __QNX__
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
#ifndef __QNX__
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
                2,
                live::audio::sampleRate() );

    if (!file) {
        qDebug() << "sndfile refuses to open "<<filename<<"...";
        return 0;
    }

    int frames=qMax(s_container[0]->s_data.size(),s_container[1]->s_data.size())*live::audio::sampleRate();
    float*data=new float[frames*2];  //2==chans, no of items, data is interlaced
    for (int i=0;i<2;i++) {
        float*dataPtr;
        int counter=s_container[i]->getRawPointer(0,dataPtr);
        for (int j=0;j<s_container[i]->s_data.size()*live::audio::sampleRate();j++) {
            data[j*2+i]=dataPtr?*dataPtr:0.0f;
            if (j!=frames/2) {
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
    lthread::ui();
#if !defined(_WIN32) && !defined(__QNX__)
    s_container[0]->clear();
    s_container[1]->clear();

    SndfileHandle file( filename.toStdString() );

    if (!file) {
        qDebug() << "sndfile refuses to open "<<filename<<"...";
        return 0;
    }

    if (file.samplerate()!=live::audio::sampleRate()) {
        qDebug() << "Expected sample rate"<<live::audio::sampleRate() << "but got incompatible"<<file.samplerate();
        return 0;
    }

    sf_count_t frames=file.frames();
//        Q_ASSERT(frames%file.channels()==0);
    float*data=new float[frames*2];  //2==chans, no of items, data is interlaced
    file.readf(data,frames);
    for (int i=0;i<2;i++) {
        float*dataPtr;
        int counter=frames?(s_container[i]->getRawPointer(0,dataPtr,1)):0;
        dataPtr--;
        for (int j = 0; j < frames/2; ++j) live_async {
            if (j!=frames/2) {
                if ((dataPtr?++dataPtr:0),--counter==-1) {
                    counter=s_container[i]->getRawPointer(j,dataPtr,1)-1;
                }
                (*dataPtr)=data[j*2+i];
            }
        }
    }

    int dataSize=qMax(s_container[0]->s_data.size(),s_container[1]->s_data.size())*live::audio::sampleRate();
    emit dataUpdated(0,dataSize);
#endif
    return 1;
}

void live::AudioTrack::async() {
    //this function prevents data allocation in the real-time thread.
    float*a;
    s_container[0]->getRawPointer((unsigned)s_curPos + live::audio::sampleRate(),a,1);
    s_container[1]->getRawPointer((unsigned)s_curPos + live::audio::sampleRate(),a,1);
}
