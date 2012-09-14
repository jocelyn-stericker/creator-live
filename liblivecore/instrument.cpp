/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/instrument>
#include <live/instrumentinterface>

using namespace live;

instrument* instrument::s_singleton=0;

void instrument::registerInterface(InstrumentInterface* c) {
    lthread::assertUi();
    s_singleton = s_singleton?s_singleton:new instrument;
    if (s_singleton->s_instrumentNames.contains(c->name())) {
        return;
    }
    s_singleton->s_instrumentNames.push_back(c->name());
    s_singleton->s_instruments.push_back(c);
}

instrument::~instrument() {
    lthread::assertUi();
    while (s_instruments.size()) {
        delete s_instruments.takeLast();
    }
    s_singleton=0;
}

QStringList instrument::names() {
    lthread::assertUi();
    s_singleton = s_singleton?s_singleton:new instrument;
    return s_singleton->s_instrumentNames;
}

QList<InstrumentInterface*> instrument::interfaces() {
    lthread::assertUi();
    s_singleton = s_singleton?s_singleton:new instrument;
    return s_singleton->s_instruments;
}
