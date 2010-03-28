/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
#include <math.h>
#include "complex.h"
#include "vector.h"

#ifdef __x86_64__
#include "w_sincos.c"
#endif

#if defined(BUILD_OS_DARWIN)
dcomplex __libm_dcis(double x)
{
  dcomplex result;
  result.dimag = sin(x);
  result.dreal = cos(x);
  return result;
}
#pragma weak __dcis
dcomplex __dcis(double x) { return __libm_dcis(x); }
#else /* defined(BUILD_OS_DARWIN) */
dcomplex __libm_dcis(double x)
{
  dcomplex result;
  FN_NAME(sincos)(x, &result.dimag, &result.dreal);
  return result;
}

dcomplex __dcis(double x) __attribute__ ((weak, alias("__libm_dcis")));
#endif /* defined(BUILD_OS_DARWIN) */
