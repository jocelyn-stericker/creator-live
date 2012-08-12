/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "live/ambition"

namespace live {

Ambition::Ambition(ObjectPtr cinput, ObjectChain chain, ObjectPtr coutput, ObjectPtr cloop)
  : s_input(cinput)
  , s_chain(chain)
  , s_output(coutput)
  , s_loopbackOut(cloop)
  , s_ec(new MidiEventCounter)
  , b_output(s_output->name())
  { setChain(s_chain);
    ambition::self()->notifyCreated(this);
}

Ambition::~Ambition() {
    while (s_connections.size()) {
        delete s_connections.takeFirst();
    }
    ambition::self()->notifyDestroyed(this);
}

ambition* ambition::s_self = 0;

ambition* ambition::self() {
    return s_self ? s_self : (s_self = new ambition);
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
