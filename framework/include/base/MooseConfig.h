#ifndef _FRAMEWORK_INCLUDE_BASE_MOOSECONFIG_H
#define _FRAMEWORK_INCLUDE_BASE_MOOSECONFIG_H 1

/* framework/include/base/MooseConfig.h. Generated automatically at end of configure. */
/* framework/include/base/MooseConfig.h.tmp.  Generated from MooseConfig.h.in by configure.  */
/* framework/include/base/MooseConfig.h.in.  Generated from configure.ac by autoheader.  */

/* The size of the derivative backing array */
#ifndef MOOSE_AD_MAX_DOFS_PER_ELEM
#define MOOSE_AD_MAX_DOFS_PER_ELEM 64
#endif

/* Whether or not libpng was detected on the system */
#ifndef MOOSE_HAVE_LIBPNG
#define MOOSE_HAVE_LIBPNG 1
#endif

/* The directory where libtorch is installed */
#ifndef MOOSE_LIBTORCH_DIR
#define MOOSE_LIBTORCH_DIR /home/64a/miniforge/envs/moose2/lib/python3.10/site-packages/torch
#endif

/* Whether to use libtorch-related code or not */
#ifndef MOOSE_LIBTORCH_ENABLED
#define MOOSE_LIBTORCH_ENABLED 1
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef MOOSE_PACKAGE_BUGREPORT
#define MOOSE_PACKAGE_BUGREPORT "https://github.com/idaholab/moose/discussions"
#endif

/* Define to the full name of this package. */
#ifndef MOOSE_PACKAGE_NAME
#define MOOSE_PACKAGE_NAME "moose"
#endif

/* Define to the full name and version of this package. */
#ifndef MOOSE_PACKAGE_STRING
#define MOOSE_PACKAGE_STRING "moose 0.9.0"
#endif

/* Define to the one symbol short name of this package. */
#ifndef MOOSE_PACKAGE_TARNAME
#define MOOSE_PACKAGE_TARNAME "moose"
#endif

/* Define to the home page for this package. */
#ifndef MOOSE_PACKAGE_URL
#define MOOSE_PACKAGE_URL "https://mooseframework.org"
#endif

/* Define to the version of this package. */
#ifndef MOOSE_PACKAGE_VERSION
#define MOOSE_PACKAGE_VERSION "0.9.0"
#endif

/* once: _FRAMEWORK_INCLUDE_BASE_MOOSECONFIG_H */
#endif
