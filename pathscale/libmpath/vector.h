#ifndef __vector_function_h
#define __vector_function_h

#include <math.h>

#ifdef __x86_64__
# define FN_NAME(fname) concat(__vec_,fname)
#else
# define FN_NAME(fname) fname
#endif // __x86_64__

#define __proto__(x...) (x, long count, long dx, long dy)

#define vector_declare(name) \
  extern inline typeof(name) FN_NAME(name) __attribute__((always_inline))

#define vector_function(name, proto...) \
  void __v ## name __proto__(proto) __attribute__((pure)); \
  void __v ## name __proto__(proto) { \
    for (long i = 0; i < count; i++, x += dx, y += dy) { \
      *y = FN_NAME(name)(*x); \
    } \
  } \
  void v ## name __proto__(proto) __attribute__ ((weak, alias ("__v" # name)))

#ifdef _libm_weak
# undef _libm_weak
#endif

#define _libm_weak 0

#ifdef __x86_64__
#include "libm_amd.h"
#endif

#undef FN_PROTOTYPE
#define FN_PROTOTYPE(fname) static inline FN_NAME(fname)

#endif // __vector_function_h
