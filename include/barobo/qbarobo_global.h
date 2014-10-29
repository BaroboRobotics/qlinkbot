#ifndef QBAROBO_GLOBAL_H
#define QBAROBO_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef QLINKBOT_DYN_LINK
# if defined(QLINKBOT_LIBRARY)
   // qlinkbot is being built as a shared library
#  define QLINKBOT_API Q_DECL_EXPORT
# else
   // qlinkbot is being linked against as a shared library
#  define QLINKBOT_API Q_DECL_IMPORT
# endif
#else
  // qlinkbot is a static library, no need for anything fancy
# define QLINKBOT_API
#endif

#endif // QBAROBO_GLOBAL_H
