/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                ( ( ( MIDI PLUGIN ) ) )

*******************************************************/

#include "midiplugin.h"

#include <QSettings>
#include <QtPlugin>

MidiInterface::MidiInterface()
{
//    QSettings settings("Nettek", "SF2 plugin for Creator Live");
//    Vst::s_vstpaths_linux=settings.value("vstis",QStringList()).toStringList();
}

//Q_EXPORT_PLUGIN2(livemidi, MidiPluginObject)
