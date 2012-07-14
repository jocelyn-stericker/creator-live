#ifndef LIVESF2_GLOBAL_H
#define LIVESF2_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIVESF2_LIBRARY)
#  define LIVESF2SHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIVESF2SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIVESF2_GLOBAL_H
