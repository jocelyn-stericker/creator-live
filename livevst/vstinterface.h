/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#ifndef VSTINTERFACE_H
#define VSTINTERFACE_H

#include <live/instrumentinterface>
#include "vstwizard.h"
#include "vstsettingswidget.h"
#include <QObject>

class VstInterface : public QObject, public live::InstrumentInterface
{
    Q_OBJECT
    Q_INTERFACES(live::InstrumentInterface)
public:
    VstInterface();
    QString name() { return "VST Instrument"; }
    QString description() { return "Steinberg VST Instrument plugin"; }
    /** selectionWidget should define a signal:
     * 'void instrumentUpdated(ObjectPtr out, ObjectPtr loopback);'
     */
    QObject* selectionWidget(live::ObjectPtr current, live::ObjectPtr loopback) {
        return new VstWizard(current,loopback,0);
    }

    QString settingsWidgetName() { return "VST Instruments..."; }
    QObject* settingsWidget() { return new VstSettingsWidget; }

    InstrumentInterface* next() { return 0; }
};

#endif // VSTINTERFACE_H
