/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "ambition.h"
#include "livewindow.h"

Ambition::Ambition(live::ObjectPtr cinput,live::ObjectChain chain, live::ObjectPtr coutput, live::ObjectPtr cloop) : s_input(cinput),s_chain(chain),s_output(coutput),s_loopbackOut(cloop)
{b_output=s_output->name(); setChain(s_chain); LiveWindow::singleton->curPatch()->ambitions.push_back(this); }

Ambition::~Ambition()
{
    while(s_connections.size())
    {
        delete s_connections.takeFirst();
    }
    LiveWindow::singleton->curPatch()->ambitions.removeOne(this);
}
