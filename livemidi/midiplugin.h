/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                ( ( ( MIDI PLUGIN ) ) )

*******************************************************/

#ifndef MIDIPLUGIN_H
#define MIDIPLUGIN_H

#include "midiwizard.h"

#include <live/object>
#include <live/instrumentinterface>

class MidiPluginObject : public live::Object
{
    live::ObjectPtr s_in;
    live::ObjectPtr s_out;
    live::Connection s_connection;
public:
    LIVE_HYBRID
    LIVE_EFFECT

    MidiPluginObject(live::ObjectPtr in, live::ObjectPtr out)
      : live::Object(in.valid() ? "" : in->name(), false, false, 2)
      , s_in(in)
      , s_out(out)
      , s_connection(out, this, live::AudioConnection)
      {
    }

    void aIn(const float* data, int chan, live::ObjectChain* p) {
        if (p->back() != s_out)
            return;

        aOut(data, chan, this);
    }
    void mIn(const live::Event* data, live::ObjectChain* p) {
        p->push_back(this);
        s_in->mIn(data,p);
        p->pop_back();
    }
};


class MidiInterface : public QObject, public live::InstrumentInterface
{
    Q_OBJECT
    Q_INTERFACES(live::InstrumentInterface)
    Q_PLUGIN_METADATA(IID "ca.nettek.live.midi")
public:
    MidiInterface();
    QString name() { return "Physical MIDI Device"; }
    QString description() { return "Do they still exist?"; }
    /** selectionWidget should define a signal:
     * 'void instrumentUpdated(ObjectPtr out, ObjectPtr loopback);'
     */
    QWidget* selectionWidget(live::ObjectPtr, live::ObjectPtr) {
        return new MidiWizard();
    }

    QString settingsWidgetName() { return "MIDI Bridge Settings..."; }
    QObject* settingsWidget() { return 0; }

    InstrumentInterface* next() { return 0; }
};
#endif // MIDIPLUGIN_H
