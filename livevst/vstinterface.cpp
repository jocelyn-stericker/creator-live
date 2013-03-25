/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#include "vstinterface.h"
#include "vst.h"

#include <QSettings>
#include <QtPlugin>

VstInterface::VstInterface()
{
    QSettings settings("Nettek", "VSTi plugin for Creator Live");
    Vst::m_vstpaths_linux=settings.value("vstis",QStringList()).toStringList();
}
