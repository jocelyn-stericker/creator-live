/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/
#include <live/asyncconnect.h>
#include "asyncconnect_p.h"

LIBLIVECORESHARED_EXPORT live_private::ASyncConnectSys* live_private::ASyncConnectSys::s_singleton=0;

bool live::async_connect(QObject* sender, const char* signal, QObject* receiver, const char* method) {
    return live_private::ASyncConnectSys::me()->newConnection(sender,signal,receiver,method);
}
