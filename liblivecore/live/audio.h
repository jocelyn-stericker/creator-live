/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef AUDIOSYS_H
#define AUDIOSYS_H

#include "live/object.h"

namespace live {

class AudioInterface;

class LIBLIVECORESHARED_EXPORT audio
{
    static AudioInterface* s_audioInterface;
public:
    static void registerInterface(AudioInterface*);
    static QObject* getCurrentInterface();

    static const int& nFrames();
    // Assumes: N/A
    // State changes: none, fast
    // Returns: number of frames per period for jack callback

    static qint32 sampleRate();
    // Assumes: N/A
    // State changes: Jack call
    // Returns: sample rate

    static void resetMappings();
    // Assumes: no connections have been made to mapped I/O's (!!)
    // State changes: removes all mapped I/O's
    // Returns: N/A

    static void addMapping(QStringList ios,bool input,QString name);
    // Assumes: no ios are currently mapped, blocking will not have undesired consequences, connection will not be revoked
    // State changes: adds an audio mapping
    // Returns: N/A

    static int mappingCount(bool input);

    static void refresh();
    // Assumes: N/A
    // State changes: may query jack, slow
    // Returns: N/A

    static QStringList getInputChanStringList();

    static QStringList getOutputChanStringList();

    static ObjectPtr null(int chans);
    // Assumes: N/A
    // State changes: N/A
    // Returns: a null port called every period with chans channels

    static void stop();

    QByteArray save();
    static void loadVerify(const QByteArray&str);
};  // Note: to get inputs&outputs, use the store!

}

#endif // AUDIOSYS_H
