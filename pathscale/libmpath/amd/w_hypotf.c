/*
 * Copyright (C) 2008. PathScale, LLC. All Rights Reserved.
 */
/* ============================================================
Copyright (c) 2002 Advanced Micro Devices, Inc.

All rights reserved.

Redistribution and  use in source and binary  forms, with or
without  modification,  are   permitted  provided  that  the
following conditions are met:

+ Redistributions  of source  code  must  retain  the  above
  copyright  notice,   this  list  of   conditions  and  the
  following disclaimer.

+ Redistributions  in binary  form must reproduce  the above
  copyright  notice,   this  list  of   conditions  and  the
  following  disclaimer in  the  documentation and/or  other
  materials provided with the distribution.

+ Neither the  name of Advanced Micro Devices,  Inc. nor the
  names  of  its contributors  may  be  used  to endorse  or
  promote  products  derived   from  this  software  without
  specific prior written permission.

THIS  SOFTWARE  IS PROVIDED  BY  THE  COPYRIGHT HOLDERS  AND
CONTRIBUTORS "AS IS" AND  ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING,  BUT NOT  LIMITED TO,  THE IMPLIED  WARRANTIES OF
MERCHANTABILITY  AND FITNESS  FOR A  PARTICULAR  PURPOSE ARE
DISCLAIMED.  IN  NO  EVENT  SHALL  ADVANCED  MICRO  DEVICES,
INC.  OR CONTRIBUTORS  BE LIABLE  FOR ANY  DIRECT, INDIRECT,
INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES
(INCLUDING,  BUT NOT LIMITED  TO, PROCUREMENT  OF SUBSTITUTE
GOODS  OR  SERVICES;  LOSS  OF  USE, DATA,  OR  PROFITS;  OR
BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON  ANY THEORY OF
LIABILITY,  WHETHER IN CONTRACT,  STRICT LIABILITY,  OR TORT
(INCLUDING NEGLIGENCE  OR OTHERWISE) ARISING IN  ANY WAY OUT
OF  THE  USE  OF  THIS  SOFTWARE, EVEN  IF  ADVISED  OF  THE
POSSIBILITY OF SUCH DAMAGE.

It is  licensee's responsibility  to comply with  any export
regulations applicable in licensee's jurisdiction.
============================================================ */

#include "libm_amd.h"
#include "libm_util_amd.h"

#ifdef USE_SOFTWARE_SQRT
#define USE_SQRTF_AMD_INLINE
#endif
#define USE_INFINITYF_WITH_FLAGS
#define USE_HANDLE_ERRORF
#include "libm_inlines_amd.h"
#ifdef USE_SOFTWARE_SQRT
#undef USE_SQRTF_AMD_INLINE
#endif
#undef USE_INFINITYF_WITH_FLAGS
#undef USE_HANDLE_ERRORF

#include "libm_errno_amd.h"

/* Deal with errno for out-of-range result */
static inline float retval_errno_erange_overflow(float x, float y)
{
  struct exception exc;
  exc.arg1 = (double)x;
  exc.arg2 = (double)y;
  exc.type = OVERFLOW;
  exc.name = (char *)"hypotf";
  if (_LIB_VERSION == _SVID_)
    exc.retval = HUGE;
  else
    exc.retval = infinityf_with_flags(AMD_F_OVERFLOW | AMD_F_INEXACT);
  if (_LIB_VERSION == _POSIX_)
    __set_errno(ERANGE);
  else if (!matherr(&exc))
    __set_errno(ERANGE);
  return exc.retval;
}

float FN_PROTOTYPE(hypotf)(float x, float y)
{
  /* Returns sqrt(x*x + y*y) with no overflow or underflow unless
     the result warrants it */

    /* Do intermediate computations in double precision
       and use sqrt instruction from chip if available. */
    double dx = x, dy = y, dr, retval;

    /* The largest finite float, stored as a double */
    const double large = 3.40282346638528859812e+38; /* 0x47efffffe0000000 */


  unsigned long ux, uy, avx, avy;

  GET_BITS_DP64(x, avx);
  avx &= ~SIGNBIT_DP64;
  GET_BITS_DP64(y, avy);
  avy &= ~SIGNBIT_DP64;
  ux = (avx >> EXPSHIFTBITS_DP64);
  uy = (avy >> EXPSHIFTBITS_DP64);

  if (ux == BIASEDEMAX_DP64 + 1 || uy == BIASEDEMAX_DP64 + 1)
    {
      retval = x*x + y*y;
      /* One or both of the arguments are NaN or infinity. The
         result will also be NaN or infinity. */
      if (((ux == BIASEDEMAX_DP64 + 1) && !(avx & MANTBITS_DP64)) ||
          ((uy == BIASEDEMAX_DP64 + 1) && !(avy & MANTBITS_DP64)))
        /* x or y is infinity. ISO C99 defines that we must
           return +infinity, even if the other argument is NaN.
           Note that the computation of x*x + y*y above will already 
           have raised invalid if either x or y is a signalling NaN. */
        return infinityf_with_flags(0);
      else
        /* One or both of x or y is NaN, and neither is infinity.
           Raise invalid if it's a signalling NaN */
        return (float)retval;
    }

    dr = (dx*dx + dy*dy);

#if USE_SOFTWARE_SQRT
    retval = sqrtf_amd_inline(r);
#else
#ifdef WINDOWS
  /* VC++ intrinsic call */
  _mm_store_sd(&retval, _mm_sqrt_sd(_mm_setzero_pd(), _mm_load_sd(&dr)));
#else
    /* Hammer sqrt instruction */
    asm volatile ("sqrtsd %1, %0" : "=x" (retval) : "x" (dr));
#endif
#endif

    if (retval > large)
      return retval_errno_erange_overflow(x, y);
    else
      return (float)retval;
  }

weak_alias (__hypotf, hypotf)
/* See comment in w_exp.c */
#if defined(KEY) && defined(TARG_X8664) /* Bug 14156 */
weak_alias (__hypotf, __ieee754_hypotf)
#endif /* defined(TARG_X8664) */
