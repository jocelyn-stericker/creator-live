/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( SF2 PLUGIN ) ) )

*******************************************************/

#ifndef VSTINTERFACE_H
#define VSTINTERFACE_H

#include <live/instrumentinterface>
#include "livesf2wizard.h"
//#include "vstsettingswidget.h"
#include <QObject>

class SF2Interface : public QObject, public live::InstrumentInterface
{
    Q_OBJECT
    Q_INTERFACES(live::InstrumentInterface)
    Q_PLUGIN_METADATA(IID "ca.nettek.live.sf2")
public:
    SF2Interface();
    QString name() { return "SoundFont"; }
    QString description() { return ":)"; }
    /** selectionWidget should define a signal:
     * 'void instrumentUpdated(ObjectPtr out, ObjectPtr loopback);'
     */
    QWidget* selectionWidget(live::ObjectPtr current, live::ObjectPtr loopback) {
//        return new LiveSF2Wizard(current,loopback,0);
        return new LiveSF2Wizard();
    }

    QString settingsWidgetName() { return "SoundFonts..."; }
    QObject* settingsWidget() { return 0; }

    InstrumentInterface* next() { return 0; }
};

#endif // VSTINTERFACE_H
