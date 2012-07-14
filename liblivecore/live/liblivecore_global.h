/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LIBLIVECORE_GLOBAL_H
#define LIBLIVECORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBLIVECORE_LIBRARY)
#  define LIBLIVECORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBLIVECORESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBLIVECORE_GLOBAL_H
