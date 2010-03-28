/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
#include <math.h>
#include "complex.h"
#include "vector.h"

#ifdef __x86_64__
#include "w_sincosf.c"
#endif

#if defined(BUILD_OS_DARWIN)
complex __libm_rcis(float x)
{
  complex result;
  result.imag = sinf(x);
  result.real = cosf(x);
  return result;
}
#pragma weak __rcis
complex __rcis(float x) { return __libm_rcis(x); }
#else /* defined(BUILD_OS_DARWIN) */
complex __libm_rcis(float x)
{
  complex result;
  FN_NAME(sincosf)(x, &result.imag, &result.real);
  return result;
}

complex __rcis(float x) __attribute__ ((weak, alias("__libm_rcis")));
#endif /* defined(BUILD_OS_DARWIN) */
