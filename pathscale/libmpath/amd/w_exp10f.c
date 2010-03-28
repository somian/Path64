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

#define USE_SPLITEXPF
#define USE_SCALEFLOAT_1
#define USE_SCALEFLOAT_2
#define USE_ZEROF_WITH_FLAGS
#define USE_INFINITYF_WITH_FLAGS
#define USE_HANDLE_ERRORF
#include "libm_inlines_amd.h"
#undef USE_SPLITEXPF
#undef USE_SCALEFLOAT_1
#undef USE_SCALEFLOAT_2
#undef USE_ZEROF_WITH_FLAGS
#undef USE_INFINITYF_WITH_FLAGS
#undef USE_HANDLE_ERRORF

#include "libm_errno_amd.h"

/* Deal with errno for out-of-range result */
static inline float retval_errno_erange_overflow(float x)
{
  struct exception exc;
  exc.arg1 = (double)x;
  exc.arg2 = (double)x;
  exc.type = OVERFLOW;
  exc.name = (char *)"exp10f";
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

static inline float retval_errno_erange_underflow(float x)
{
  struct exception exc;
  exc.arg1 = (double)x;
  exc.arg2 = (double)x;
  exc.type = UNDERFLOW;
  exc.name = (char *)"exp10f";
  exc.retval = zerof_with_flags(AMD_F_UNDERFLOW | AMD_F_INEXACT);
  if (_LIB_VERSION == _POSIX_)
    __set_errno(ERANGE);
  else if (!matherr(&exc))
    __set_errno(ERANGE);
  return exc.retval;
}

float FN_PROTOTYPE(exp10f)(float x)
{
  static const float
         max_exp10_arg = 3.8531841278E+01F, /* 0x421A209B */
         min_exp10_arg =-4.4853469848E+01F, /* 0xC23369F4 */
                 log10 = 2.3025850929E+00F, /* 0x40135D8E */
 thirtytwo_by_log10of2 = 1.0630169677E+02F, /* 0x42D49A78 */
   log10of2_by_32_lead = 9.4070434570E-03F, /* 0x3C1A2000 */
   log10of2_by_32_tail = 1.4390730030E-07F; /* 0x341A84F0 */

  float y, z1, z2, z;
  int m;
  unsigned int ux, ax;

  /*
    Computation of exp10f (x).

    We compute the values m, z1, and z2 such that
    exp10f(x) = 2**m * (z1 + z2),  where exp10f(x) is 10**x.

    Computations needed in order to obtain m, z1, and z2
    involve three steps.

    First, we reduce the argument x to the form
    x = n * log10of2/32 + remainder,
    where n has the value of an integer and |remainder| <= log10of2/64.
    The value of n = x * 32/log10of2 rounded to the nearest integer and
    the remainder = x - n*log10of2/32.

    Second, we approximate exp10f(r1 + r2) - 1 where r1 is the leading
    part of the remainder and r2 is the trailing part of the remainder.

    Third, we reconstruct exp10f(x) so that
    exp10f(x) = 2**m * (z1 + z2).
  */

  GET_BITS_SP32(x, ux);
  ax = ux & (~SIGNBIT_SP32);

  if (ax >= 0x421A209B) /* abs(x) >= 38.5... */
    {
      if(ax >= 0x7f800000)
        {
          /* x is either NaN or infinity */
          if (ux & MANTBITS_SP32)
            /* x is NaN */
            return x + x; /* Raise invalid if it is a signalling NaN */
          else if (ux & SIGNBIT_SP32)
            /* x is negative infinity; return 0.0 with no flags. */
            return 0.0F;
          else
            /* x is positive infinity */
            return x;
        }
      if (x > max_exp10_arg)
        /* Return +infinity with overflow flag */
        return retval_errno_erange_overflow(x);
      else if (x < min_exp10_arg)
        /* x is negative. Return +zero with underflow and inexact flags */
        return retval_errno_erange_underflow(x);
    }

  /* Handle small arguments separately */
  if (ax < 0x3bde5bd9)   /* abs(x) < 1/(64*log10) */
    {
      if (ax < 0x32800000)   /* abs(x) < 2^(-26) */
        return 1.0F + x; /* Raises inexact if x is non-zero */
      else
        {
        y = log10*x;
      z = ((((((((
		    1.0F/40320)*x+
		   1.0F/5040)*y+
		  1.0F/720)*y+
		 1.0F/120)*y+
		1.0F/24)*y+
	       1.0F/6)*y+
	      1.0F/2)*y+
               1.0F)*y + 1.0F;
        }
    }
  else
    {
      /* Find m, z1 and z2 such that exp10f(x) = 2**m * (z1 + z2) */

      splitexpf(x, log10, thirtytwo_by_log10of2, log10of2_by_32_lead,
                log10of2_by_32_tail, &m, &z1, &z2);

      /* Scale (z1 + z2) by 2.0**m */

      if (m >= EMIN_SP32 && m <= EMAX_SP32)
        z = scaleFloat_1((float)(z1+z2),m);
      else
        z = scaleFloat_2((float)(z1+z2),m);
    }
  return z;
}

weak_alias (__exp10f, exp10f)
weak_alias (__exp10f, pow10f)
