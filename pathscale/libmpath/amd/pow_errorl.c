/* ============================================================
Copyright (c) 2004 Advanced Micro Devices, Inc.

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

#define USE_INFINITY_WITH_FLAGS
#define USE_ZERO_WITH_FLAGS
#define USE_NAN_WITH_FLAGS
#include "libm_inlines_amd.h"
#undef USE_INFINITY_WITH_FLAGS
#undef USE_ZERO_WITH_FLAGS
#undef USE_NAN_WITH_FLAGS


/* Deal with errno for out-of-range result */
#include "libm_errno_amd.h"
static inline double retval_errno_erange_overflow(double x, double y, int sign)
{
  struct exception exc;
  exc.arg1 = x;
  exc.arg2 = y;
  exc.type = OVERFLOW;
  exc.name = (char *)"pow";
  if (_LIB_VERSION == _SVID_)
    {
      if (sign == 1)
        exc.retval = HUGE;
      else /* sign = -1 */
        exc.retval = -HUGE;
    }
  else
    {
      if (sign == 1)
        exc.retval = infinity_with_flags(AMD_F_OVERFLOW);
      else /* sign == -1 */
        exc.retval = -infinity_with_flags(AMD_F_OVERFLOW);
    }
  if (_LIB_VERSION == _POSIX_)
    __set_errno(ERANGE);
  else if (!matherr(&exc))
    __set_errno(ERANGE);
  return exc.retval;
}

static inline double retval_errno_erange_underflow(double x, double y, int sign)
{
  struct exception exc;
  exc.arg1 = x;
  exc.arg2 = y;
  exc.type = UNDERFLOW;
  exc.name = (char *)"pow";
  if (sign == 1)
    exc.retval = zero_with_flags(AMD_F_UNDERFLOW | AMD_F_INEXACT);
  else /* sign == -1 */
    exc.retval = -zero_with_flags(AMD_F_UNDERFLOW | AMD_F_INEXACT);
  if (_LIB_VERSION == _POSIX_)
    __set_errno(ERANGE);
  else if (!matherr(&exc))
    __set_errno(ERANGE);
  return exc.retval;
}

/* Deal with errno for out-of-range arguments */
static inline double retval_errno_edom(double x, double y, int type)
{
  struct exception exc;
  exc.arg1 = x;
  exc.arg2 = y;
  exc.type = DOMAIN;
  exc.name = (char *)"pow";
  if (_LIB_VERSION == _SVID_)
    exc.retval = 0.0;
  else if (type == 1)
    exc.retval = infinity_with_flags(AMD_F_DIVBYZERO);
  else if (type == 2)
    exc.retval = -infinity_with_flags(AMD_F_DIVBYZERO);
  else /* type == 3 */
    exc.retval = nan_with_flags(AMD_F_INVALID);
  if (_LIB_VERSION == _POSIX_)
    __set_errno (EDOM);
  if (!matherr(&exc))
    {
      if (_LIB_VERSION == _SVID_)
        (void)fputs("pow: DOMAIN error\n", stderr);
      __set_errno(EDOM);
    }
  return exc.retval;
}


double __pow_error(double argx,double argy,int index)
{

switch(index)
{
case 0:	/* x = 1.0, y = INF */
	return retval_errno_erange_underflow(argx, argy, 1);
	break;
case 1:
        /* x = +1.0 and y is NaN. Raise invalid and return NaN. */
        return retval_errno_erange_overflow(argx, argy, 1);

	break;
case 8:
        /* abs(x) < 1.0 */
	return retval_errno_erange_underflow(argx, argy, 1);
	break;
case 10:
        /* y is large, abs(x) > 1.0 */
        return retval_errno_erange_overflow(argx, argy, 1);

      /* y is -ve */
	break;
case 11:
       /* abs(x) = 0.0. Return +infinity. */
        return retval_errno_edom(argx, argy, 1);

	break;
case 12:
       /* abs(x) < 1.0; return +infinity. */
        return retval_errno_erange_overflow(argx, argy, 1);
	break;
case 14:
          /* abs(x) > 1.0 y is negative, but not -INF*/
	return retval_errno_erange_underflow(argx, argy, 1);
	break;
case 15:
       /* y is negative; return +infinity with div-by-zero
               for all cases */
        return retval_errno_edom(argx, argy, 1);
	break;
case 16:
       /* -0.0 raised to a negative odd integer returns -infinity
             with div-by-zero */
        return retval_errno_edom(argx, argy, 2);
	break;
case 17:
       /* Return +infinity with div-by-zero */
        return retval_errno_edom(argx, argy, 1);
	break;
case 18:
       /* x is negative and y is not an integer. Return a NaN. */
        return retval_errno_edom(argx, argy, 3);
	break;

  /* Test for overflow and underflow due to y*log2(x)
     being too large or small. */
case 23:
  /* If r overflowed or underflowed we need to deal with errno */
      /* Result has overflowed. */
//      if (negateres)
        return retval_errno_erange_overflow(argx, argy, -1);
	break;
case 24:
//      else
        return retval_errno_erange_overflow(argx, argy, 1);
	break;
case 25:
      /* Result has underflowed. */
//      if (negateres)
	return retval_errno_erange_underflow(argx, argy, -1);
	break;
case 26:
//      else
	return retval_errno_erange_underflow(argx, argy, 1);

	break;
default:
	break;
}
}
