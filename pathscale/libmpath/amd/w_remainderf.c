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

#define USE_NANF_WITH_FLAGS
#define USE_SCALEDOUBLE_1
#define USE_GET_FPSW_INLINE
#define USE_SET_FPSW_INLINE
#define USE_HANDLE_ERRORF
#include "libm_inlines_amd.h"
#undef USE_NANF_WITH_FLAGS
#undef USE_SCALEDOUBLE_1
#undef USE_GET_FPSW_INLINE
#undef USE_SET_FPSW_INLINE
#undef USE_HANDLE_ERRORF


#undef _FUNCNAME
#if defined(COMPILING_FMOD)
float FN_PROTOTYPE(fmodf)(float x, float y)
#define _FUNCNAME "fmodf"
#else
float FN_PROTOTYPE(remainderf)(float x, float y)
#define _FUNCNAME "remainderf"
#endif
{
  double dx, dy, scale, w, t;
  int i, ntimes, xexp, yexp;
  unsigned long ux, uy, ax, ay;

  unsigned int sw;

  dx = x;
  dy = y;

  GET_BITS_DP64(dx, ux);
  GET_BITS_DP64(dy, uy);
  ax = ux & ~SIGNBIT_DP64;
  ay = uy & ~SIGNBIT_DP64;
  xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
  yexp = (int)((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);

  if (xexp < 1 || xexp > BIASEDEMAX_DP64 ||
      yexp < 1 || yexp > BIASEDEMAX_DP64)
    {
      /* x or y is zero, NaN or infinity (neither x nor y can be
         denormalized because we promoted from float to double) */
      if (xexp > BIASEDEMAX_DP64)
        {
          /* x is NaN or infinity */
          if (ux & MANTBITS_DP64)
            {
            /* x is NaN */
              return x + x; /* Raise invalid if it is a signalling NaN */
            }
          else
            {
            /* x is infinity; result is NaN */
              return nanf_with_flags(AMD_F_INVALID);
            }
        }
      else if (yexp > BIASEDEMAX_DP64)
        {
          /* y is NaN or infinity */
          if (uy & MANTBITS_DP64)
            {
            /* y is NaN */
              return y + y; /* Raise invalid if it is a signalling NaN */
            }
          else
            {
            /* y is infinity; result is x */
              return x;
            }
        }
      else if (xexp < 1)
        {
          /* x must be zero (cannot be denormalized) */
          if (yexp < 1)
            {
            /* y must be zero (cannot be denormalized) */
              return nanf_with_flags(AMD_F_INVALID);
            }
          else
            return x;
        }
      else
        {
        /* y must be zero */
          return nanf_with_flags(AMD_F_INVALID);
        }
    }
  else if (ax == ay)
    {
      /* abs(x) == abs(y); return zero with the sign of x */
      PUT_BITS_DP64(ux & SIGNBIT_DP64, dx);
      return (float)dx;
    }

  /* Set dx = abs(x), dy = abs(y) */
  PUT_BITS_DP64(ax, dx);
  PUT_BITS_DP64(ay, dy);

  if (ax < ay)
    {
      /* abs(x) < abs(y) */
#if !defined(COMPILING_FMOD)
      if (dx > 0.5*dy)
        dx -= dy;
#endif
      return (float)(x < 0.0? -dx : dx);
    }

  /* Save the current floating-point status word. We need
     to do this because the remainder function is always
     exact for finite arguments, but our algorithm causes
     the inexact flag to be raised. We therefore need to
     restore the entry status before exiting. */
  sw = get_fpsw_inline();

  /* Set ntimes to the number of times we need to do a
     partial remainder. If the exponent of x is an exact multiple
     of 24 larger than the exponent of y, and the mantissa of x is
     less than the mantissa of y, ntimes will be one too large
     but it doesn't matter - it just means that we'll go round
     the loop below one extra time. */
  if (xexp <= yexp)
    {
      ntimes = 0;
      w = dy;
      scale = 1.0;
    }
  else
    {
      ntimes = (xexp - yexp) / 24;

      /* Set w = y * 2^(24*ntimes) */
      PUT_BITS_DP64((unsigned long)(ntimes * 24 + EXPBIAS_DP64) << EXPSHIFTBITS_DP64,
                    scale);
      w = scale * dy;
      /* Set scale = 2^(-24) */
      PUT_BITS_DP64((unsigned long)(-24 + EXPBIAS_DP64) << EXPSHIFTBITS_DP64,
                    scale);
    }

  /* Each time round the loop we compute a partial remainder.
     This is done by subtracting a large multiple of w
     from x each time, where w is a scaled up version of y.
     The subtraction can be performed exactly when performed
     in double precision, and the result at each stage can
     fit exactly in a single precision number. */
  for (i = 0; i < ntimes; i++)
    {
      /* t is the integer multiple of w that we will subtract.
         We use a truncated value for t. */
      t = (double)((int)(dx / w));
      dx -= w * t;
      /* Scale w down by 2^(-24) for the next iteration */
      w *= scale;
    }

  /* One more time */
#if defined(COMPILING_FMOD)
  t = (double)((int)(dx / w));
  dx -= w * t;
#else
 {
  unsigned int todd;
  /* Variable todd says whether the integer t is odd or not */
  t = (double)((int)(dx / w));
  todd = ((int)(dx / w)) & 1;
  dx -= w * t;

  /* At this point, dx lies in the range [0,dy) */
  /* For the remainder function, we need to adjust dx
     so that it lies in the range (-y/2, y/2] by carefully
     subtracting w (== dy == y) if necessary. */
  if (dx > 0.5 * w || ((dx == 0.5 * w) && todd))
    dx -= w;
 }
#endif

  /* **** N.B. for some reason this breaks the 32 bit version
     of remainder when compiling with optimization. */
  /* Restore the entry status flags */
  set_fpsw_inline(sw);

  /* Set the result sign according to input argument x */
  return (float)(x < 0.0? -dx : dx);

}

#if defined(COMPILING_FMOD)
weak_alias (__fmodf, fmodf)
#else
weak_alias (__remainderf, remainderf)
#endif
