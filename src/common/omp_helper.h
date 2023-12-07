#ifndef __omp_helper_h
#define __omp_helper_h

#ifdef _OPENMP

#include <omp.h>

#if   (_OPENMP >= 202011)
#  define _OPENMP_MAJOR 5
#  define _OPENMP_MINOR 1
#elif   (_OPENMP >= 201811)
#  define _OPENMP_MAJOR 5
#  define _OPENMP_MINOR 0
#elif   (_OPENMP >= 201511)
#  define _OPENMP_MAJOR 4
#  define _OPENMP_MINOR 5
#elif (_OPENMP >= 201307)
#  define _OPENMP_MAJOR 4
#  define _OPENMP_MINOR 0
#elif (_OPENMP >= 201107)
#  define _OPENMP_MAJOR 3
#  define _OPENMP_MINOR 1
#elif (_OPENMP >= 200805)
#  define _OPENMP_MAJOR 3
#  define _OPENMP_MINOR 0
#elif (_OPENMP >= 200505)
#  define _OPENMP_MAJOR 2
#  define _OPENMP_MINOR 5
#else
#  define _OPENMP_MAJOR 2
#  define _OPENMP_MINOR 0
#endif

#if (_OPENMP >= 200805)
#  define __has_openmp_atomics
#endif

#ifdef __cplusplus
constexpr int openmp_version       (void) { return _OPENMP; }
constexpr int openmp_version_major (void) { return _OPENMP_MAJOR; }
constexpr int openmp_version_minor (void) { return _OPENMP_MINOR; }
constexpr int openmp_has_atomics   (void) { return _OPENMP >= 200805; }
#endif

#endif /* _OPENMP */

#define _TOSTRING(_STR) # _STR
#define TOSTRING(_STR) _TOSTRING(_STR)

#endif /*__omp_helper_h*/
