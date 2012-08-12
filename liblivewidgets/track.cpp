/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "live_widgets/track.h"
#include "live/app"
#include "live_widgets/trackhint.h"

#include <QCoreApplication>

using namespace live;
using namespace live_widgets;

AbstractTrack::AbstractTrack(QWidget* parent)
    : QWidget(parent) {
}
