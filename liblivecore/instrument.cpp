/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/instrument>
#include <live/instrumentinterface>

using namespace live;

instrument* instrument::m_singleton=0;

void instrument::registerInterface(InstrumentInterface* c) {
    lthread::assertUi();
    m_singleton = m_singleton?m_singleton:new instrument;
    if (m_singleton->m_instrumentNames.contains(c->name())) {
        return;
    }
    m_singleton->m_instrumentNames.push_back(c->name());
    m_singleton->m_instruments.push_back(c);
}

instrument::~instrument() {
    lthread::assertUi();
    while (m_instruments.size()) {
        delete m_instruments.takeLast();
    }
    m_singleton=0;
}

QStringList instrument::names() {
    lthread::assertUi();
    m_singleton = m_singleton?m_singleton:new instrument;
    return m_singleton->m_instrumentNames;
}

QList<InstrumentInterface*> instrument::interfaces() {
    lthread::assertUi();
    m_singleton = m_singleton?m_singleton:new instrument;
    return m_singleton->m_instruments;
}
