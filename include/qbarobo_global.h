#ifndef QBAROBO_GLOBAL_H
#define QBAROBO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QBAROBO_LIBRARY)
#  define QBAROBOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QBAROBOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QBAROBO_GLOBAL_H
