/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
#include "complex.h"
#include "vector.h"

#if ! defined(BUILD_OS_DARWIN)
/*vector_declare(sincosf);*/
#endif /* defined(BUILD_OS_DARWIN) */

#ifdef __x86_64__
#include "w_sincosf.c"
#endif

complex FN_PROTOTYPE(cisf)(float x) __attribute__((always_inline));

complex FN_PROTOTYPE(cisf)(float x)
{
  complex result;
#if defined(BUILD_OS_DARWIN)
  result.imag = sinf(x);
  result.real = cosf(x);
#else /* defined(BUILD_OS_DARWIN) */
  FN_NAME(sincosf)(x, &result.imag, &result.real);
#endif /* defined(BUILD_OS_DARWIN) */
  return result;
}

vector_function(cisf, const float *x, complex *y);
