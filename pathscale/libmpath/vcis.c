/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
#include "complex.h"
#include "vector.h"

/* This needs to be defined for linux as well as maybe other platforms */
/*#if ! defined(BUILD_OS_DARWIN)*/
/*vector_declare(sincos);*/
/* #endif /* defined(BUILD_OS_DARWIN) */

#ifdef __x86_64__
#include "w_sincos.c"
#endif

dcomplex FN_PROTOTYPE(cis)(double x) __attribute__((always_inline));

dcomplex FN_PROTOTYPE(cis)(double x)
{
  dcomplex result;
#if defined(BUILD_OS_DARWIN)
  result.dimag = sin(x);
  result.dreal = cos(x);
#else /* defined(BUILD_OS_DARWIN) */
  FN_NAME(sincos)(x, &result.dimag, &result.dreal);
#endif /* defined(BUILD_OS_DARWIN) */
  return result;
}

vector_function(cis, const double *x, dcomplex *y);
