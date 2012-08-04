/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "live/audio"
#include "audiosystem_qnx_p.h"
#include "live/songsystem"

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

#include <sys/asoundlib.h>

using namespace live_private;

static inline void lesi2f_array (short *src, const int& count, float *dest) {
    float normfact=1.0 / ((float) 0x8000);

    short v;
    int i=count;
    const int h=count/2;
    while (--i>= 0) {
        v = src [i];
        dest [ i/2 + (i%2) * h ] = ((float) v)*normfact;
    }
}

static inline void fi2les_array_i (const float *src, short *dest, const int& count) {
    float normfact=1.0 * 0x7FFF;
    unsigned char* ucptr = ((unsigned char*) dest) + 2 * count ;

    int at=count;
    const int half=count/2;

    int value ;
    while (--at >= 0) {
        ucptr -= 2;
        value = lrintf (src [ at/2 + (at%2) * half] * normfact);
        ucptr [0] = value;
        ucptr [1] = value >> 8;
    }
}

/**
 * @brief fi2les_array_i_half fills in one channel of an interleaved little endian short array on little endian platforms.
 * @param src one channel of the non-interleaved float array
 * @param dest the stereo interleaved array
 * @param count the count of items in the FULL INTERLEAVED array (i.e., 2*length of src)
 * @param chan 0 for left, 1 for right.
 */
static inline void fi2les_array_half (const float *src, short *dest, const int& count, int chan) {
    unsigned char* ucptr;
    int value;
    float normfact=1.0 * 0x7FFF;

    ucptr = ((unsigned char*) dest) + 2 * count + 2 * chan;

    int at=count/2;

    while (--at >= 0) {
        ucptr -= 4;
        value = lrintf (src [ at ] * normfact);
        ucptr [0] = value;
        ucptr [1] = value >> 8;
    }
}

void AudioOut::run() {
    setPriority(QThread::TimeCriticalPriority);

    int     card = -1;
    int     dev = 0;
    snd_pcm_t *pcm_handle;
    int     mSampleRate;
    int     mSampleChannels;
    int     mSampleBits;

    int     rtn;
    snd_pcm_channel_info_t pi;
    snd_mixer_t *mixer_handle;
    snd_mixer_group_t group;
    snd_pcm_channel_params_t pp;
    snd_pcm_channel_setup_t setup;
    int     bsize, n;
    fd_set  rfds, wfds;
    if ((rtn = snd_pcm_open_preferred (&pcm_handle, &card, &dev, SND_PCM_OPEN_PLAYBACK)) < 0) {
        perror("device open");
        return;
    }

    mSampleRate = 48000;
    mSampleChannels = 2;
    mSampleBits = 16;

    memset (&pi, 0, sizeof (pi));
    pi.channel = SND_PCM_CHANNEL_PLAYBACK;
    if ((rtn = snd_pcm_channel_info (pcm_handle, &pi)) < 0) {
        fprintf (stderr, "snd_pcm_channel_info failed: %s\n", snd_strerror (rtn));
        return;
    }
    memset (&pp, 0, sizeof (pp));

    pp.mode = SND_PCM_MODE_BLOCK;
    pp.channel = SND_PCM_CHANNEL_PLAYBACK;
    pp.start_mode = SND_PCM_START_FULL;
    pp.stop_mode = SND_PCM_STOP_STOP;

    std::cerr<<"PLAYBACK INFO:"<<pi.min_fragment_size<<":"<<pi.max_fragment_size<<":"<<pi.formats<<":"<<pi.flags<<":"<<pi.rates<<std::endl;
    /**********************************************************************************************************************************
     * The PlayBook supports:
     *  SND_PCM_CHNINFO_BLOCK, SND_PCM_CHNINFO_MMAP, SND_PCM_CHNINFO_INTERLEAVE, SND_PCM_CHNINFO_BLOCK_TRANSFER, SND_PCM_CHNINFO_PAUSE,
     *  SND_PCM_CHNINFO_MMAP_VALID, SND_PCM_CHNINFO_SECURE
     *
     * The PlayBook DOES NOT support:
     *  SND_PCM_CHNINFO_STREAM, SND_PCM_CHNINFO_NONINTERLEAVE, SND_PCM_CHNINFO_PROTECTED
     *
     * The native format is SND_PCM_SFMT_S16_LE at a sample rate of 48000.
     */


    pp.buf.block.frag_size = pi.min_fragment_size;
    pp.buf.block.frags_max = 2;
    pp.buf.block.frags_min = 1;

    pp.format.interleave = 1;
    pp.format.rate = mSampleRate;
    pp.format.voices = mSampleChannels;

    pp.format.format=SND_PCM_SFMT_S16_LE;

    strcpy (pp.sw_mixer_subchn_name, "Creator Live Speakers");
    if ((rtn = snd_pcm_channel_params (pcm_handle, &pp)) < 0) {
        fprintf (stderr, "snd_pcm_channel_params failed: %s\n", snd_strerror (rtn));
        return;
    }

    if ((rtn = snd_pcm_channel_prepare (pcm_handle, SND_PCM_CHANNEL_PLAYBACK)) < 0)
        fprintf (stderr, "snd_pcm_channel_prepare failed: %s\n", snd_strerror (rtn));

    memset (&setup, 0, sizeof (setup));
    memset (&group, 0, sizeof (group));
    setup.channel = SND_PCM_CHANNEL_PLAYBACK;
    setup.mixer_gid = &group.gid;
    if ((rtn = snd_pcm_channel_setup (pcm_handle, &setup)) < 0) {
        fprintf (stderr, "snd_pcm_channel_setup failed: %s\n", snd_strerror (rtn));
        return;
    }

    printf ("Format %s \n", snd_pcm_get_format_name (setup.format.format));
    printf ("Frag Size %d \n", setup.buf.block.frag_size);
    printf ("Total Frags %d \n", setup.buf.block.frags);
    printf ("Rate %d \n", setup.format.rate);
    printf ("Voices %d \n", setup.format.voices);
    bsize = setup.buf.block.frag_size;

    if (group.gid.name[0] == 0) {
        printf ("Mixer Pcm Group [%s] Not Set \n", group.gid.name);
        exit (-1);
    }
    printf ("Mixer Pcm Group [%s]\n", group.gid.name);
    if ((rtn = snd_mixer_open (&mixer_handle, card, setup.mixer_device)) < 0) {
        fprintf (stderr, "snd_mixer_open failed: %s\n", snd_strerror (rtn));
        return;
    }

    FD_ZERO (&rfds);
    FD_ZERO (&wfds);
    n = 1;

    FD_SET (snd_mixer_file_descriptor (mixer_handle), &rfds);
    FD_SET (snd_pcm_file_descriptor (pcm_handle, SND_PCM_CHANNEL_PLAYBACK), &wfds);
    rtn = qMax (snd_mixer_file_descriptor (mixer_handle),
                snd_pcm_file_descriptor (pcm_handle, SND_PCM_CHANNEL_PLAYBACK));



    for(int i=0;i<bsize/sizeof(qint16);++i) {
        mSampleBfr2[i]=0;
    }

    while (1) {
        if (tcgetpgrp (0) == getpid ()) FD_SET (STDIN_FILENO, &rfds);

        //         PLAYBACK

        if (select (rtn + 1, &rfds, &wfds, NULL, NULL) == -1) {
            perror("select");
            return;
        }

        if (FD_ISSET (snd_pcm_file_descriptor (pcm_handle, SND_PCM_CHANNEL_PLAYBACK), &wfds)) {
            int     written = 0;

            written = snd_pcm_write (pcm_handle, mSampleBfr2, bsize);
            if (written < bsize) {
                std::cerr<<"No good! (A+)\n";
                snd_pcm_channel_status_t status;
                memset (&status, 0, sizeof (status));
                status.channel = SND_PCM_CHANNEL_PLAYBACK;

                if (snd_pcm_channel_status (pcm_handle, &status) < 0) {
                    printf ("overrun: capture channel status error (play)\n");
                    exit (1);
                }

                if (status.status == SND_PCM_STATUS_READY ||
                        status.status == SND_PCM_STATUS_OVERRUN) {
                    if (snd_pcm_channel_prepare (pcm_handle, SND_PCM_CHANNEL_PLAYBACK) < 0) {
                        printf ("overrun: capture channel prepare error (play)\n");
                        exit (1);
                    }
                }

                if (written < 0) written = 0;

                written += snd_pcm_write(pcm_handle, mSampleBfr2 + written, bsize - written);
                std::cerr<<"TOTAL WRITTEN:"<<written<<"\n";

                FD_SET (snd_mixer_file_descriptor (mixer_handle), &rfds);
                FD_SET (snd_pcm_file_descriptor (pcm_handle, SND_PCM_CHANNEL_PLAYBACK), &wfds);
                rtn = qMax (snd_mixer_file_descriptor (mixer_handle),
                            snd_pcm_file_descriptor (pcm_handle, SND_PCM_CHANNEL_PLAYBACK));
            }
            //            lock->unlock();
            //            qDebug() << "WAKEONE";
            //            printf ("bytes written = %d \n", written);
        }

        for(int i=0;i<bsize/sizeof(qint16);++i) {
            mSampleBfr2[i]=0;
        }

        dataRead->wakeAll();
        master->process();
    }

    n = snd_pcm_channel_flush (pcm_handle, SND_PCM_CHANNEL_PLAYBACK);

    rtn = snd_mixer_close (mixer_handle);
    rtn = snd_pcm_close (pcm_handle);
}

void AudioIn::run() {
    setPriority(QThread::TimeCriticalPriority);

    int     cardR = -1;
    int     devR = 0;
    snd_pcm_t *pcm_handleR;
    int     mSampleRate;
    int     mSampleChannels;
    int     mSampleBits;

    int     rtn, rtnB;
    snd_pcm_channel_info_t piR;
    //    snd_mixer_t *mixer_handle;
    snd_mixer_group_t groupR;
    snd_pcm_channel_params_t ppR;
    snd_pcm_channel_setup_t setupR;
    int     bsize, n;
    fd_set  setR;

    if ((rtn = snd_pcm_open_preferred (&pcm_handleR, &cardR, &devR, SND_PCM_OPEN_CAPTURE)) < 0) {
        perror("device open");
        return;
    }

    mSampleRate = 48000;
    mSampleChannels = 2;
    mSampleBits = 16;

    memset (&piR, 0, sizeof (piR));
    piR.channel = SND_PCM_CHANNEL_CAPTURE;
    if ((rtn = snd_pcm_channel_info (pcm_handleR, &piR)) < 0) {
        fprintf (stderr, "snd_pcm_channel_info failed: %s\n", snd_strerror (rtn));
        return;
    }
    memset (&ppR, 0, sizeof (ppR));

    ppR.mode = SND_PCM_MODE_BLOCK;
    ppR.channel = SND_PCM_CHANNEL_CAPTURE;
    ppR.start_mode = SND_PCM_START_DATA;
    ppR.stop_mode = SND_PCM_STOP_STOP;

    ppR.buf.block.frag_size = 1536;
    ppR.buf.block.frags_max = 2;
    ppR.buf.block.frags_min = 1;

    ppR.format.interleave = 1;
    ppR.format.rate = mSampleRate;
    ppR.format.voices = mSampleChannels;

    ppR.format.format=SND_PCM_SFMT_S16_LE;

    strcpy (ppR.sw_mixer_subchn_name, "Creator Live Mic");
    if ((rtn = snd_pcm_channel_params (pcm_handleR, &ppR)) < 0) {
        fprintf (stderr, "snd_pcm_channel_params failed: %s\n", snd_strerror (rtn));
        return;
    }

    if ((rtn = snd_pcm_channel_prepare (pcm_handleR, SND_PCM_CHANNEL_CAPTURE)) < 0)
        fprintf (stderr, "snd_pcm_channel_prepare failed: %s\n", snd_strerror (rtn));

    memset (&setupR, 0, sizeof (setupR));
    memset (&groupR, 0, sizeof (groupR));
    setupR.channel = SND_PCM_CHANNEL_CAPTURE;
    setupR.mixer_gid = &groupR.gid;
    if ((rtn = snd_pcm_channel_setup (pcm_handleR, &setupR)) < 0) {
        fprintf (stderr, "snd_pcm_channel_setup failed: %s\n", snd_strerror (rtn));
        return;
    }


    printf ("Rec Format %s \n", snd_pcm_get_format_name (setupR.format.format));
    printf ("Rec Frag Size %d \n", setupR.buf.block.frag_size);
    printf ("Rec Total Frags %d \n", setupR.buf.block.frags);
    printf ("Rec Rate %d \n", setupR.format.rate);
    printf ("Rec Voices %d \n", setupR.format.voices);
    bsize = setupR.buf.block.frag_size;

    qint16 *mSampleBfr = new qint16[(3072+bsize)/sizeof(qint16)];
    //    short *mSampleBfr2 = new short[bsize/sizeof(short)];
    FD_ZERO (&setR);
    n = 1;
    FD_SET (snd_pcm_file_descriptor (pcm_handleR, SND_PCM_CHANNEL_CAPTURE), &setR);
    rtnB = snd_pcm_file_descriptor (pcm_handleR, SND_PCM_CHANNEL_CAPTURE);

    int asyncCount=0;

    float* bfrX2 = new float[3072/sizeof(qint16)];

    while (1) {
        //         CAPTURE

        if (select (rtnB + 1, &setR, NULL, NULL, NULL) == -1) {
            std::cerr<<"can't select\n";
            perror("select");
            return;
        }

        if (FD_ISSET (snd_pcm_file_descriptor (pcm_handleR, SND_PCM_CHANNEL_CAPTURE), &setR)) {
            int     read = 0;
            int amnt=bsize;
            read = snd_pcm_read (pcm_handleR, mSampleBfr+asyncCount/sizeof(qint16), amnt);

            if (read < amnt) {   // FIXME:: add snd_pcm_read!! We can actually be off by half a qint16 as it stands!
                std::cerr<<"No good! (B)\n";
                snd_pcm_channel_status_t status;
                memset (&status, 0, sizeof (status));
                status.channel = SND_PCM_CHANNEL_CAPTURE;
                if (snd_pcm_channel_status (pcm_handleR, &status) < 0) {
                    std::cerr<<"CCSE\n";
                    exit (1);
                }

                if (status.status == SND_PCM_STATUS_READY || status.status == SND_PCM_STATUS_OVERRUN) {
                    if (snd_pcm_channel_prepare (pcm_handleR, SND_PCM_CHANNEL_CAPTURE) < 0) {
                        std::cerr<<"CCPE\n";
                        exit (1);
                    }
                }
                FD_SET (snd_pcm_file_descriptor (pcm_handleR, SND_PCM_CHANNEL_CAPTURE), &setR);
                rtnB = snd_pcm_file_descriptor (pcm_handleR, SND_PCM_CHANNEL_CAPTURE);
            }
            asyncCount+=read;
            if(asyncCount>=3072) {
                lesi2f_array(mSampleBfr,3072/sizeof(qint16),bfrX2);
                master->lock->lock();

                live::ObjectChain p;
                p->push_back(this);
                master->dataRead->wait(master->lock);  // wait until bfrX2 is recorded.
                live::Object::beginProc();
                aOut(bfrX2, 0, p);
                aOut(bfrX2 + 3072/sizeof(qint16)/2, 1, p);
                live::Object::endProc();
                p->pop_back();

                master->lock->unlock();
                asyncCount-=3072;
                memcpy(mSampleBfr,mSampleBfr+3072/sizeof(qint16),asyncCount);
            }
        }
    }
}

void AudioOut::aIn(const float *data, int chan, live::ObjectChain* ) {
    fi2les_array_half(data,mSampleBfr2,3072/sizeof(qint16),chan);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// The following is profoundly boring. It works, so don't change it.

QStringList jackGetPorts(bool, bool) { Q_ASSERT(0); return QStringList(); }
QStringList jackGetInputPorts() { Q_ASSERT(0); return QStringList(); }
QStringList jackGetOutputPorts() { Q_ASSERT(0); return QStringList(); }
void jackConnectPorts(QString, QString, bool) { Q_ASSERT(0); }
void jackDisconnectPorts(QString, QString) { Q_ASSERT(0); }

LIBLIVECORESHARED_EXPORT SecretAudio* SecretAudio::singleton = 0;

bool SecretAudio::makeClient() { return 1; } // depr.

SecretAudio::SecretAudio() : inputs(), outputs() {
    qDebug() << "NEWSECRETAUDIO...";
    connect(qApp,SIGNAL(destroyed()),this,SLOT(delClient()));
    singleton=this;
    outputs.push_back(new AudioOut(this));
    inputs.push_back(new AudioIn(outputs.back()));
    outputs.back()->start();
    inputs.back()->start();
    live::object::set(outputs.back());
    live::object::set(inputs.back());
    nframes = 3072/2/sizeof(qint16);    // mort de rire.
}

bool SecretAudio::delClient() {
    qDebug() << "Unregistering client. Or not. No need.";
    return 1;
}

void SecretAudio::process() {
    live::Object::beginProc();

    float* buffer = 0;
    foreach( AudioNull* i, nulls ) {
        if(!buffer) buffer = new float[ nframes ];

        for ( int j = 0; j < i->chans; j++ ) {
            for(int k=0; k<nframes; k++) {
                buffer[k]=0.0;
            }
            live::ObjectChain p;
            i->aIn( buffer, j, p);
        }
    }
    delete[] buffer;

    if(live::song::current&&live::song::current->metronome) {
        live::song::current->metronome->clock();
    }

    live::Object::endProc();
}

bool SecretAudio::refresh() {
    return 1;
}
/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
AudioSys
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////

LIBLIVECORESHARED_EXPORT live::AudioInterface* live::audio::s_audioInterface=0;

void live::audio::registerInterface(live::AudioInterface*ai) {
    if(s_audioInterface) s_audioInterface->delClient();
    s_audioInterface=ai;
    s_audioInterface->makeClient();
}

QObject* live::audio::getCurrentInterface() {
    return s_audioInterface->qobject();
}

void live::audio::refresh() { s_audioInterface->refresh(); }

LIBLIVECORESHARED_EXPORT const int& live::audio::nFrames() {
    return s_audioInterface->nFrames();
}

LIBLIVECORESHARED_EXPORT qint32 live::audio::sampleRate() {
    return s_audioInterface->sampleRate();
}

qint32 live_private::SecretAudio::sampleRate() {
    return nframes;
}

LIBLIVECORESHARED_EXPORT void live::audio::resetMappings() {
    s_audioInterface->resetMappings();
}

void live_private::SecretAudio::jack_disconnect(QString readPort,QString writePort) {
    Q_ASSERT(0);
}

bool live_private::SecretAudio::resetMappings() {
}

LIBLIVECORESHARED_EXPORT void live::audio::addMapping(QStringList mapping, bool input,QString name) {
    s_audioInterface->addMapping(mapping,input,name);
}

bool live_private::SecretAudio::addMapping(QStringList, bool, QString) {
    Q_ASSERT(0);
}

LIBLIVECORESHARED_EXPORT int live::audio::mappingCount(bool) {
    Q_ASSERT(0);
}

int live_private::SecretAudio::mappingCount(bool) {
    Q_ASSERT(0);
}

LIBLIVECORESHARED_EXPORT QStringList live::audio::getInputChanStringList() {
    return s_audioInterface->getInputChanStringList();
}

QStringList live_private::SecretAudio::getInputChanStringList() {
    return jackGetOutputPorts();
}

LIBLIVECORESHARED_EXPORT QStringList live::audio::getOutputChanStringList() {
    return s_audioInterface->getOutputChanStringList();
}

QStringList live_private::SecretAudio::getOutputChanStringList() {
    return jackGetInputPorts();
}

LIBLIVECORESHARED_EXPORT live::ObjectPtr live::audio::null(int chan) {
    return s_audioInterface->getNull(chan);
}

live::ObjectPtr live_private::SecretAudio::getNull(int chans) {
    nulls.push_back( new AudioNull(chans) );
    return nulls.back();
}

LIBLIVECORESHARED_EXPORT void live::audio::stop() {
    delete s_audioInterface;
    s_audioInterface=0;
}

/*/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
Shutdown
///////////////////////////////////////////////////////////////////////////////////////
/*/////////////////////////////////////////////////////////////////////////////////////

LIBLIVECORESHARED_EXPORT SecretAudioShutdownHandler* SecretAudioShutdownHandler::singleton = new SecretAudioShutdownHandler ;

void SecretAudioShutdownHandler::byeBye() {
    //uncomment this line:
    //exec( "start /MIN taskkill /F /IM jackd.exe" );
    //exec( "start /MIN taskkill /F /IM sooperlooper.exe" );
}
