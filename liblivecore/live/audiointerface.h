/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef AUDIOINTERFACE_H
#define AUDIOINTERFACE_H

#include "live/object.h"

namespace live {

class LIBLIVECORESHARED_EXPORT AudioInterface
{
public:
    virtual ~AudioInterface()
    {
//      Make sure you clean up. For example:
//        while(inputs.size())
//        {
//            delete inputs.takeFirst();
//        }
//        while(outputs.size())
//        {
//            delete outputs.takeFirst();
//        }
//        delClient();
    }

    virtual QString name()=0;
    virtual QString description()=0;

    /** A settings widget is opened when shouldDisplaySettingsWidget() is true
     *  or when the user requests it.
     *
     *  You don't need to define a settings widget, you can return 0.
     *  For the Qt/Desktop platform, settingsWidget() should be a QWidget
     *  which can be any size from 480x200 to 640x480. */
    virtual QObject* settingsWidget()=0;

    virtual bool shouldDisplaySettingsWidget()=0;

    virtual QString errorString()=0;

    virtual bool makeClient()=0;
    virtual bool delClient()=0;
    virtual bool refresh()=0;
    virtual ObjectPtr getNull(int chans)=0;
    virtual qint32 sampleRate()=0;
    virtual const int& nFrames()=0;

    virtual bool resetMappings()=0;
    virtual bool addMapping(QStringList mapping, bool input,QString name)=0;
    virtual int mappingCount(bool input)=0;

    virtual QStringList getInputChanStringList()=0;
    virtual QStringList getOutputChanStringList()=0;
    virtual QObject* qobject()=0;
};

}

Q_DECLARE_INTERFACE(live::AudioInterface, "ca.nettek.live.audioInterface/0.01")

#endif // AUDIOINTERFACE_H
