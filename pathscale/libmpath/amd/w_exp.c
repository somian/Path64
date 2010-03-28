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

#define USE_SPLITEXP
#define USE_SCALEDOUBLE_1
#define USE_SCALEDOUBLE_2
#define USE_ZERO_WITH_FLAGS
#define USE_INFINITY_WITH_FLAGS
#define USE_NAN_WITH_FLAGS
#define USE_HANDLE_ERROR
#include "libm_inlines_amd.h"
#undef USE_ZERO_WITH_FLAGS
#undef USE_SPLITEXP
#undef USE_SCALEDOUBLE_1
#undef USE_SCALEDOUBLE_2
#undef USE_INFINITY_WITH_FLAGS
#undef USE_NAN_WITH_FLAGS
#undef USE_HANDLE_ERROR

#include "libm_errno_amd.h"

/* Deal with errno for out-of-range argument */
static inline double retval_errno_edom(double x)
{
  struct exception exc;
  exc.arg1 = x;
  exc.arg2 = x;
  exc.type = DOMAIN;
  exc.name = (char *)"exp";
  if (_LIB_VERSION == _SVID_)
    exc.retval = HUGE;
  else
    exc.retval = nan_with_flags(AMD_F_INVALID);
  if (_LIB_VERSION == _POSIX_)
    __set_errno(EDOM);
  else if (!matherr(&exc))
    {
      if(_LIB_VERSION == _SVID_)
        (void)fputs("exp: DOMAIN error\n", stderr);
    __set_errno(EDOM);
    }
  return exc.retval;
}

/* Deal with errno for out-of-range result */
static inline double retval_errno_erange_overflow(double x)
{
  struct exception exc;
  exc.arg1 = x;
  exc.arg2 = x;
  exc.type = OVERFLOW;
  exc.name = (char *)"exp";
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

static inline double retval_errno_erange_underflow(double x)
{
  struct exception exc;
  exc.arg1 = x;
  exc.arg2 = x;
  exc.type = UNDERFLOW;
  exc.name = (char *)"exp";
  exc.retval = zero_with_flags(AMD_F_UNDERFLOW | AMD_F_INEXACT);
  if (_LIB_VERSION == _POSIX_)
    __set_errno(ERANGE);
  else if (!matherr(&exc))
    __set_errno(ERANGE);
  return exc.retval;
}

double FN_PROTOTYPE(exp)(double x)
{
  static const double
       max_exp_arg =  7.09782712893383973096e+02, /* 0x40862e42fefa39ef */
       min_exp_arg = -7.45133219101941108420e+02, /* 0xc0874910d52d3051 */
 thirtytwo_by_log2 = 4.61662413084468283841e+01,  /* 0x40471547652b82fe */
   log2_by_32_lead = 2.16608493356034159660e-02,  /* 0x3f962e42fe000000 */
  log2_by_32_trail = 5.68948749532545630390e-11;  /* 0x3dcf473de6af278e */

  double z1, z2, z;
  int m;
  unsigned long ux, ax;

  /*
    Computation of exp(x).

    We compute the values m, z1, and z2 such that
    exp(x) = 2**m * (z1 + z2),  where
    exp(x) is the natural exponential of x.

    Computations needed in order to obtain m, z1, and z2
    involve three steps.

    First, we reduce the argument x to the form
    x = n * log2/32 + remainder,
    where n has the value of an integer and |remainder| <= log2/64.
    The value of n = x * 32/log2 rounded to the nearest integer and
    the remainder = x - n*log2/32.

    Second, we approximate exp(r1 + r2) - 1 where r1 is the leading
    part of the remainder and r2 is the trailing part of the remainder.

    Third, we reconstruct the exponential of x so that
    exp(x) = 2**m * (z1 + z2).
  */


  GET_BITS_DP64(x, ux);
  ax = ux & (~SIGNBIT_DP64);

  if (ax >= 0x40862e42fefa39ef) /* abs(x) >= 709.78... */
    {
      if(ax >= 0x7ff0000000000000)
        {
          /* x is either NaN or infinity */
          if (ux & MANTBITS_DP64)
            /* x is NaN */
            return x + x; /* Raise invalid if it is a signalling NaN */
          else if (ux & SIGNBIT_DP64)
            /* x is negative infinity; return 0.0 with no flags. */
            return 0.0;
          else
            /* x is positive infinity */
            return x;
        }
      if (x > max_exp_arg)
        /* Return +infinity with overflow flag */
        return retval_errno_erange_overflow(x);
      else if (x < min_exp_arg)
        /* x is negative. Return +zero with underflow and inexact flags */
        return retval_errno_erange_underflow(x);
    }

  /* Handle small arguments separately */
  if (ax < 0x3fb0000000000000)   /* abs(x) < 1/16 */
    {
      if (ax < 0x3c00000000000000)   /* abs(x) < 2^(-63) */
        z = 1.0 + x; /* Raises inexact if x is non-zero */
      else
        z = ((((((((((
		      1.0/3628800)*x+
		     1.0/362880)*x+
		    1.0/40320)*x+
		   1.0/5040)*x+
		  1.0/720)*x+
		 1.0/120)*x+
		1.0/24)*x+
	       1.0/6)*x+
	      1.0/2)*x+
	     1.0)*x + 1.0;
    }
  else
    {
      /* Find m, z1 and z2 such that exp(x) = 2**m * (z1 + z2) */

      splitexp(x, 1.0, thirtytwo_by_log2, log2_by_32_lead, log2_by_32_trail,
               &m, &z1, &z2);

      /* Scale (z1 + z2) by 2.0**m */

      if (m >= EMIN_DP64 && m <= EMAX_DP64)
	z = scaleDouble_1((z1+z2),m);
      else
	z = scaleDouble_2((z1+z2),m);
    }
  return z;
}


weak_alias (__exp, exp)
/* GNU libm.a puts "exp", "__exp" and "__ieee754_exp" into the same w_exp.o
 * file. At least one function ("cexp") defined elsewhere in libm.a (but not
 * in libmpath) imports "__ieee754_exp". Thus, a statically linked program
 * which uses both "exp" and "cexp" will link both w_exp.o from libmpath.a
 * and w_exp.o from libm.a, resulting in a multiple definition of __exp. The
 * cure is to provide "__ieee754_exp" ourself so the w_exp.o in libm.a is
 * not needed.
 *
 * Several other sets of symbols exhibit the same problem.
 *
 * libm.a fixed the problem between versions 2.2.5 (used with GCC 3) and
 * 2.3.6 (used with GCC 4) by moving the offending __ieee754* symbols to
 * different .o files, so the conflict no longer occurs. However, providing
 * this alias when using GCC 4 allows the use of the libmpath version instead
 * of the libm version in more circumstances (e.g. cexp as well as exp.)
 */
#if defined(KEY) && defined(TARG_X8664) /* Bug 14156 */
weak_alias (__exp, __ieee754_exp)
#endif /* defined(TARG_X8664) */
