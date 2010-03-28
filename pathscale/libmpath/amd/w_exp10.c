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
#define USE_HANDLE_ERROR
#include "libm_inlines_amd.h"
#undef USE_SPLITEXP
#undef USE_SCALEDOUBLE_1
#undef USE_SCALEDOUBLE_2
#undef USE_ZERO_WITH_FLAGS
#undef USE_INFINITY_WITH_FLAGS
#undef USE_HANDLE_ERROR

#include "libm_errno_amd.h"

/* Deal with errno for out-of-range result */
static inline double retval_errno_erange_overflow(double x)
{
  struct exception exc;
  exc.arg1 = x;
  exc.arg2 = x;
  exc.type = OVERFLOW;
  exc.name = (char *)"exp10";
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
  exc.name = (char *)"exp10";
  exc.retval = zero_with_flags(AMD_F_UNDERFLOW | AMD_F_INEXACT);
  if (_LIB_VERSION == _POSIX_)
    __set_errno(ERANGE);
  else if (!matherr(&exc))
    __set_errno(ERANGE);
  return exc.retval;
}

double FN_PROTOTYPE(exp10)(double x)
{
  static const double
         max_exp10_arg =  3.0825471555991674677e+02, /* 0x40734413509f79ff */
         min_exp10_arg = -3.2330621534311580944e+02, /* 0xc07434e6420f4374 */
                 log10 = 2.30258509299404568401e+00, /* 0x40026bb1bbb55516 */
 thirtytwo_by_log10of2 = 1.06301699036395595131e+02, /* 0x405a934f0979a371 */
   log10of2_by_32_lead = 9.40718688070774078369e-03, /* 0x3F83441340000000 */
  log10of2_by_32_trail = 4.83791671566737916758e-10; /* 0x3E009F79FEF311F1 */

  double y, z1, z2, z;
  int m;
  unsigned long ux, ax;

  /*
    Computation of exp10(x).

    We compute the values m, z1, and z2 such that
    exp10(x) = 2**m * (z1 + z2),  where exp10(x) is 10**x.

    Computations needed in order to obtain m, z1, and z2
    involve three steps.

    First, we reduce the argument x to the form
    x = n * log10of2/32 + remainder,
    where n has the value of an integer and |remainder| <= log10of2/64.
    The value of n = x * 32/log10of2 rounded to the nearest integer and
    the remainder = x - n*log10of2/32.

    Second, we approximate exp10(r1 + r2) - 1 where r1 is the leading
    part of the remainder and r2 is the trailing part of the remainder.

    Third, we reconstruct exp10(x) so that
    exp10(x) = 2**m * (z1 + z2).
  */


  GET_BITS_DP64(x, ux);
  ax = ux & (~SIGNBIT_DP64);

  if (ax >= 0x40734413509f79ff) /* abs(x) >= 308.25... */
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
      if (x > max_exp10_arg)
        /* Return +infinity with overflow flag */
        return retval_errno_erange_overflow(x);
      else if (x < min_exp10_arg)
        /* x is negative. Return +zero with underflow and inexact flags */
        return retval_errno_erange_underflow(x);
    }


  /* Handle small arguments separately */
  if (ax < 0x3f9bcb7b131bbb9d)   /* abs(x) < 1/(16*log10) */
    {
      if (ax < 0x3c00000000000000)   /* abs(x) < 2^(-63) */
        return 1.0 + x; /* Raises inexact if x is non-zero */
      else
        y = log10*x;
        z = ((((((((((
		      1.0/3628800)*y+
		     1.0/362880)*y+
		    1.0/40320)*y+
		   1.0/5040)*y+
		  1.0/720)*y+
		 1.0/120)*y+
		1.0/24)*y+
	       1.0/6)*y+
	      1.0/2)*y+
	     1.0)*y + 1.0;
    }
  else
    {
      /* Find m, z1 and z2 such that exp10(x) = 2**m * (z1 + z2) */

      splitexp(x, log10, thirtytwo_by_log10of2, log10of2_by_32_lead,
               log10of2_by_32_trail, &m, &z1, &z2);

      /* Scale (z1 + z2) by 2.0**m */
      if (m > EMIN_DP64 && m < EMAX_DP64)
	return scaleDouble_1((z1+z2),m);
      else
	return scaleDouble_2((z1+z2),m);
    }
  return z;
}

weak_alias (__exp10, exp10)
weak_alias (__exp10, pow10)
