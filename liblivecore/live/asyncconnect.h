/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef ASYNCCONNECT_H
#define ASYNCCONNECT_H

#include <QObject>
#include "liblivecore_global.h"

namespace live {

LIBLIVECORESHARED_EXPORT bool async_connect(QObject* sender, const char* signal, QObject* receiver, const char* method);

}

#endif // ASYNCCONNECT_H