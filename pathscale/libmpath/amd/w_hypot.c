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

#define USE_SCALEDOUBLE_1
#define USE_INFINITY_WITH_FLAGS
#define USE_HANDLE_ERROR
#include "libm_inlines_amd.h"
#undef USE_SCALEDOUBLE_1
#undef USE_INFINITY_WITH_FLAGS
#undef USE_HANDLE_ERROR

#include "libm_errno_amd.h"

/* Deal with errno for out-of-range result */
static inline double retval_errno_erange_overflow(double x, double y)
{
  struct exception exc;
  exc.arg1 = x;
  exc.arg2 = y;
  exc.type = OVERFLOW;
  exc.name = (char *)"hypot";
  if (_LIB_VERSION == _SVID_)
    exc.retval = HUGE;
  else
    exc.retval = infinity_with_flags(AMD_F_OVERFLOW | AMD_F_INEXACT);
  if (_LIB_VERSION == _POSIX_)
    __set_errno(ERANGE);
  else if (!matherr(&exc))
    __set_errno(ERANGE);
  return exc.retval;
}

double FN_PROTOTYPE(hypot)(double x, double y)
{
  /* Returns sqrt(x*x + y*y) with no overflow or underflow unless
     the result warrants it */

  const double large = 1.79769313486231570815e+308; /* 0x7fefffffffffffff */

  double u, r, retval, hx, tx, x2, hy, ty, y2, hs, ts;
  unsigned long xexp, yexp, ux, uy, ut;
  int dexp, expadjust;

  GET_BITS_DP64(x, ux);
  ux &= ~SIGNBIT_DP64;
  GET_BITS_DP64(y, uy);
  uy &= ~SIGNBIT_DP64;
  xexp = (ux >> EXPSHIFTBITS_DP64);
  yexp = (uy >> EXPSHIFTBITS_DP64);

  if (xexp == BIASEDEMAX_DP64 + 1 || yexp == BIASEDEMAX_DP64 + 1)
    {
      /* One or both of the arguments are NaN or infinity. The
         result will also be NaN or infinity. */
      retval = x*x + y*y;
      if (((xexp == BIASEDEMAX_DP64 + 1) && !(ux & MANTBITS_DP64)) ||
          ((yexp == BIASEDEMAX_DP64 + 1) && !(uy & MANTBITS_DP64)))
        /* x or y is infinity. ISO C99 defines that we must
           return +infinity, even if the other argument is NaN.
           Note that the computation of x*x + y*y above will already 
           have raised invalid if either x or y is a signalling NaN. */
        return infinity_with_flags(0);
      else
        /* One or both of x or y is NaN, and neither is infinity.
           Raise invalid if it's a signalling NaN */
        return retval;
    }

  /* Set x = abs(x) and y = abs(y) */
  PUT_BITS_DP64(ux, x);
  PUT_BITS_DP64(uy, y);

  /* The difference in exponents between x and y */
  dexp = (int)(xexp - yexp);
  expadjust = 0;

  if (ux == 0)
    /* x is zero */
    return y;
  else if (uy == 0)
    /* y is zero */
    return x;
  else if (dexp > MANTLENGTH_DP64 + 1 || dexp < -MANTLENGTH_DP64 - 1)
    /* One of x and y is insignificant compared to the other */
    return x + y; /* Raise inexact */
  else if (xexp > EXPBIAS_DP64 + 500 || yexp > EXPBIAS_DP64 + 500)
    {
      /* Danger of overflow; scale down by 2**600. */
      expadjust = 600;
      ux -= 0x2580000000000000;
      PUT_BITS_DP64(ux, x);
      uy -= 0x2580000000000000;
      PUT_BITS_DP64(uy, y);
    }
  else if (xexp < EXPBIAS_DP64 - 500 || yexp < EXPBIAS_DP64 - 500)
    {
      /* Danger of underflow; scale up by 2**600. */
      expadjust = -600;
      if (xexp == 0)
        {
          /* x is denormal - handle by adding 601 to the exponent
           and then subtracting a correction for the implicit bit */
          PUT_BITS_DP64(ux + 0x2590000000000000, x);
          x -= 9.23297861778573578076e-128; /* 0x2590000000000000 */
          GET_BITS_DP64(x, ux);
        }
      else
        {
          /* x is normal - just increase the exponent by 600 */
          ux += 0x2580000000000000;
          PUT_BITS_DP64(ux, x);
        }
      if (yexp == 0)
        {
          PUT_BITS_DP64(uy + 0x2590000000000000, y);
          y -= 9.23297861778573578076e-128; /* 0x2590000000000000 */
          GET_BITS_DP64(y, uy);
        }
      else
        {
          uy += 0x2580000000000000;
          PUT_BITS_DP64(uy, y);
        }
    }


#ifdef FAST_BUT_GREATER_THAN_ONE_ULP
  /* Not awful, but results in accuracy loss larger than 1 ulp */
  r = x*x + y*y
#else
  /* Slower but more accurate */

  /* Sort so that x is greater than y */
  if (x < y)
    {
      u = y;
      y = x;
      x = u;
      ut = ux;
      ux = uy;
      uy = ut;
    }

  /* Split x into hx and tx, head and tail */
  PUT_BITS_DP64(ux & 0xfffffffff8000000, hx);
  tx = x - hx;

  PUT_BITS_DP64(uy & 0xfffffffff8000000, hy);
  ty = y - hy;

  /* Compute r = x*x + y*y with extra precision */
  x2 = x*x;
  y2 = y*y;
  hs = x2 + y2;

  if (dexp == 0)
    /* We take most care when x and y have equal exponents,
       i.e. are almost the same size */
    ts = (((x2 - hs) + y2) +
          ((hx * hx - x2) + 2 * hx * tx) + tx * tx) +
      ((hy * hy - y2) + 2 * hy * ty) + ty * ty;
  else
    ts = (((x2 - hs) + y2) +
          ((hx * hx - x2) + 2 * hx * tx) + tx * tx);

  r = hs + ts;
#endif

  /* The sqrt can introduce another half ulp error. */
#ifdef WINDOWS
  /* VC++ intrinsic call */
  _mm_store_sd(&retval, _mm_sqrt_sd(_mm_setzero_pd(), _mm_load_sd(&r)));
#else
  /* Hammer sqrt instruction */
  asm volatile ("sqrtsd %1, %0" : "=x" (retval) : "x" (r));
#endif

  /* If necessary scale the result back. This may lead to
     overflow but if so that's the correct result. */
  retval = scaleDouble_1(retval, expadjust);

  if (retval > large)
    /* The result overflowed. Deal with errno. */
    return retval_errno_erange_overflow(x, y);

  return retval;
}

weak_alias (__hypot, hypot)
/* See comment in w_exp.c */
#if defined(KEY) && defined(TARG_X8664) /* Bug 14156 */
weak_alias (__hypot, __ieee754_hypot)
#endif /* defined(TARG_X8664) */
