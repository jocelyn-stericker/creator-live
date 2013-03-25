/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/ambition>

namespace live {

Ambition::Ambition(ObjectPtr cinput, ObjectChain chain, ObjectPtr coutput
                 , ObjectPtr cloop)
  : m_input(cinput)
  , m_chain(chain)
  , m_output(coutput)
  , m_loopbackOut(cloop)
  , m_connections()
  , x_ambition(QMutex::Recursive)
  , mef(new MidiEventFilter)
  , b_output(m_output->name())
  { b_output = m_output->name();
    setChain(m_chain);
    ambition::self()->notifyCreated(this);
}

Ambition::~Ambition() {
    while (m_connections.size()) {
        delete m_connections.takeFirst();
    }
    ambition::self()->notifyDestroyed(this);
}

ambition* ambition::m_self = 0;

ambition* ambition::self() {
    return m_self ? m_self : (m_self = new ambition);
}

void ambition::notifyCreated(Ambition* a) {
    self()->u_notifyCreated(a);
}

void ambition::notifyDestroyed(Ambition* a) {
    self()->u_notifyDestroyed(a);
}

void ambition::u_notifyCreated(Ambition* a) {
    emit created(a);
}

void ambition::u_notifyDestroyed(Ambition* a) {
    emit destoryed(a);
}
}  // namespace live
