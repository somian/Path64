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
#define USE_SCALEDOUBLE_2
#include "libm_inlines_amd.h"
#undef USE_SCALEDOUBLE_1
#undef USE_SCALEDOUBLE_2

double FN_PROTOTYPE(fma)(double a, double b, double sum)
{
  /* Returns a * b + sum with no intermediate loss of precision */

  double ha, ta, hb, tb, z, zz, r, s, az, asum;
  int ua, ub, usum;
  int scaled, expover, expunder, scaleexp;
  unsigned long u;

  GET_BITS_DP64(a, u);
  ua = (int)((u & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
  GET_BITS_DP64(b, u);
  ub = (int)((u & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
  GET_BITS_DP64(sum, u);
  usum = (int)((u & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;

  if (ua == EMAX_DP64 + 1 || ub == EMAX_DP64 + 1 || usum == EMAX_DP64 + 1)
    {
      /* One or more of the arguments is NaN or infinity. The
         result will also be NaN or infinity. */
      return a * b + sum;
    }
  else if (ua + ub > usum + 2 * MANTLENGTH_DP64)
    {
      /* sum is negligible compared with the extra-length product a*b */
      return a*b;
    }
  else if (usum > ua + ub + MANTLENGTH_DP64)
    {
      /* The product a*b is negligible compared with sum */
      return sum;
    }

  expover = EMAX_DP64 - 2;
  expunder = EMIN_DP64 + MANTLENGTH_DP64;
  scaleexp = 0;


  if (ua + ub > expover || usum > expover)
    {
      /* The result is likely to overflow. Scale down in an attempt
         to avoid unnecessary overflow. The true result may still overflow. */
      scaled = 1;
      scaleexp = expover / 2;
      a = scaleDouble_1(a, -scaleexp);
      b = scaleDouble_1(b, -scaleexp);
      sum = scaleDouble_2(sum, -2*scaleexp);
    }
  else if (ua + ub < expunder)
    {
      /* The product a*b is near underflow; scale up */
      scaled = 1;
      scaleexp = expunder / 2;
      a = scaleDouble_1(a, -scaleexp);
      b = scaleDouble_1(b, -scaleexp);
      sum = scaleDouble_2(sum, -2*scaleexp);
    }
  else
    scaled = 0;

  /* Split a into ha (head) and ta (tail). Do the same for b. */
  ha = a;
  GET_BITS_DP64(ha, u);
  u &= 0xfffffffff8000000;
  PUT_BITS_DP64(u, ha);
  ta = a - ha;
  hb = b;
  GET_BITS_DP64(hb, u);
  u &= 0xfffffffff8000000;
  PUT_BITS_DP64(u, hb);
  tb = b - hb;

  /* Carefully multiply the parts together. z is the most significant
     part of the result, and zz the least significant part */
  z = a * b;
  zz = (((ha * hb - z) + ha * tb) + ta * hb) + ta * tb;

  /* Set az = abs(z), asum = abs(sum) */
  GET_BITS_DP64(z, u);
  u &= ~SIGNBIT_DP64;
  PUT_BITS_DP64(u, az);
  GET_BITS_DP64(sum, u);
  u &= ~SIGNBIT_DP64;
  PUT_BITS_DP64(u, asum);

  /* Carefully add (z,zz) to sum */
  r = z + sum;

  if (az > asum)
    s = ((z - r) + sum) + zz;
  else
    s = ((sum - r) + z) + zz;

  if (scaled)
    return scaleDouble_1(r + s, 2*scaleexp);
  else
    return r + s;
}

weak_alias (__fma, fma)
