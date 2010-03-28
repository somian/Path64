/* ============================================================
Copyright (c) 2006 Advanced Micro Devices, Inc.

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

In  the  redistribution and use of this software, each party
shall  at  all times comply with all applicable governmental
laws,  statutes, ordinances, rules, regulations, orders, and
other   requirements,   including  without  limitation  such
governmental   requirements   applicable   to  environmental
protection,  health,  safety, wages, hours, equal employment
opportunity,  nondiscrimination,  working conditions, import
or export control, and transportation.  Without limiting the
foregoing,  each  party  shall  adhere  to  the  U.S. Export
Administration   Regulations  (EAR),  currently   found   at
15 C.F.R. Sections 730  through  744,  and,  unless properly
authorized  by  the  U.S. Government,  shall not (1) export,
re-export  or  release  restricted  technology, software, or
source  code  to  a  national of a country in Country Groups
D:1 or E:1,  or  (2) export to Country Groups D:1 or E:1 the
direct  product  of  such  technology  or  software, if such
foreign  produced  direct  product  is  subject  to national
security controls as identified on the Commerce Control List
(currently  found  in  Supplement 1  to Section 774 of EAR).
These  export  requirements  shall survive any expiration or
termination of this agreement.
============================================================ */

#ifndef LIBM_INLINES_AMD_H_INCLUDED
#define LIBM_INLINES_AMD_H_INCLUDED 1

#include "libm_util_amd.h"

/*Derived from static inline void __remainder_piby2f_inline(unsigned long ux, double *r, int *region)
in libm_inlines_amd.h. libm_inlines.h has the pure Windows one while libm_inlines_amd.h has the mixed one.
*/
/* Given positive argument x, reduce it to the range [-pi/4,pi/4] using
   extra precision, and return the result in r.
   Return value "region" tells how many lots of pi/2 were subtracted
   from x to put it in the range [-pi/4,pi/4], mod 4. */
void __remainder_piby2d2f(unsigned long ux, double *r, int *region)
{


      /* This method simulates multi-precision floating-point
         arithmetic and is accurate for all 1 <= x < infinity */
#if 0
      const int bitsper = 36;
#else
#define bitsper 36
#endif
      unsigned long res[10];
      unsigned long u, carry, mask, mant, nextbits;
      int first, last, i, rexp, xexp, resexp, ltb, determ, bc;
      double dx;
      static const double
        piby2 = 1.57079632679489655800e+00; /* 0x3ff921fb54442d18 */
#ifdef WINDOWS
      static unsigned long pibits[] =
      {
        0LL,
        5215LL, 13000023176LL, 11362338026LL, 67174558139LL,
        34819822259LL, 10612056195LL, 67816420731LL, 57840157550LL,
        19558516809LL, 50025467026LL, 25186875954LL, 18152700886LL
      };
#else
      static unsigned long pibits[] =
      {
        0L,
        5215L, 13000023176L, 11362338026L, 67174558139L,
        34819822259L, 10612056195L, 67816420731L, 57840157550L,
        19558516809L, 50025467026L, 25186875954L, 18152700886L
      };
#endif

#ifdef DEBUGGING_PRINT
        printf("On entry, ux = %25.20e\n", ux);
#endif

      xexp = (int)(((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64);
      ux = ((ux & MANTBITS_DP64) | IMPBIT_DP64) >> 29;

#ifdef DEBUGGING_PRINT
#endif

      /* Now ux is the mantissa bit pattern of x as a long integer */
      mask = 1;
      mask = (mask << bitsper) - 1;

      /* Set first and last to the positions of the first
         and last chunks of 2/pi that we need */
      first = xexp / bitsper;
      resexp = xexp - first * bitsper;
      /* 120 is the theoretical maximum number of bits (actually
         115 for IEEE single precision) that we need to extract
         from the middle of 2/pi to compute the reduced argument
         accurately enough for our purposes */
      last = first + 120 / bitsper;

#ifdef DEBUGGING_PRINT
      printf("first = %d, last = %d\n", first, last);
#endif

      /* Do a long multiplication of the bits of 2/pi by the
         integer mantissa */
#if 0
      for (i = last; i >= first; i--)
        {
          u = pibits[i] * ux + carry;
          res[i - first] = u & mask;
          carry = u >> bitsper;
        }
      res[last - first + 1] = 0;
#else
      /* Unroll the loop. This is only correct because we know
         that bitsper is fixed as 36. */
      res[4] = 0;
      u = pibits[last] * ux;
      res[3] = u & mask;
      carry = u >> bitsper;
      u = pibits[last - 1] * ux + carry;
      res[2] = u & mask;
      carry = u >> bitsper;
      u = pibits[last - 2] * ux + carry;
      res[1] = u & mask;
      carry = u >> bitsper;
      u = pibits[first] * ux + carry;
      res[0] = u & mask;
#endif

#ifdef DEBUGGING_PRINT
      printf("resexp = %d\n", resexp);
      printf("Significant part of x * 2/pi with binary"
             " point in correct place:\n");
      for (i = 0; i <= last - first; i++)
        {
          if (i > 0 && i % 5 == 0)
            printf("\n ");
/*          if (i == 1)
              printf("%s ", d2b(res[i], bitsper, resexp));
            else
*/            printf("%s ", d2b(res[i], bitsper, -1));
        }
      printf("\n");
#endif

      /* Reconstruct the result */
      ltb = (int)((((res[0] << bitsper) | res[1])
                   >> (bitsper - 1 - resexp)) & 7);

      /* determ says whether the fractional part is >= 0.5 */
      determ = ltb & 1;

#ifdef DEBUGGING_PRINT
      printf("ltb = %d (last two bits before binary point"
             " and first bit after)\n", ltb);
      printf("determ = %d (1 means need to negate because the fractional\n"
             "            part of x * 2/pi is greater than 0.5)\n", determ);
#endif

      i = 1;
      if (determ)
        {
          /* The mantissa is >= 0.5. We want to subtract it
             from 1.0 by negating all the bits */
          *region = ((ltb >> 1) + 1) & 3;
          mant = 1;
          mant = ~(res[1]) & ((mant << (bitsper - resexp)) - 1);
          while (mant < 0x0000000000010000)
            {
              i++;
              mant = (mant << bitsper) | (~(res[i]) & mask);
            }
          nextbits = (~(res[i+1]) & mask);
        }
      else
        {
          *region = (ltb >> 1);
          mant = 1;
          mant = res[1] & ((mant << (bitsper - resexp)) - 1);
          while (mant < 0x0000000000010000)
            {
              i++;
              mant = (mant << bitsper) | res[i];
            }
          nextbits = res[i+1];
        }

#ifdef DEBUGGING_PRINT
/*      printf("First bits of mant = %s\n", d2b(mant, bitsper, -1)); */
#endif

      /* Normalize the mantissa. The shift value 6 here, determined by
         trial and error, seems to give optimal speed. */
      bc = 0;
      while (mant < 0x0000400000000000)
        {
          bc += 6;
          mant <<= 6;
        }
      while (mant < 0x0010000000000000)
        {
          bc++;
          mant <<= 1;
        }
      mant |= nextbits >> (bitsper - bc);

      rexp = 52 + resexp - bc - i * bitsper;

#ifdef DEBUGGING_PRINT
      printf("Normalised mantissa = 0x%016lx\n", mant);
      printf("Exponent to be inserted on mantissa = rexp = %d\n", rexp);
#endif

      /* Put the result exponent rexp onto the mantissa pattern */
      u = ((unsigned long)rexp + EXPBIAS_DP64) << EXPSHIFTBITS_DP64;
      ux = (mant & MANTBITS_DP64) | u;
      if (determ)
        /* If we negated the mantissa we negate x too */
        ux |= SIGNBIT_DP64;
      PUT_BITS_DP64(ux, dx);

#ifdef DEBUGGING_PRINT
        printf("(x*2/pi) = %25.20e\n", dx);
#endif

      /* x is a double precision version of the fractional part of
         x * 2 / pi. Multiply x by pi/2 in double precision
         to get the reduced argument r. */
      *r = dx * piby2;

#ifdef DEBUGGING_PRINT
      printf(" r = frac(x*2/pi) * pi/2:\n");
      printf(" r = %25.20e\n", *r);
      printf("region = (number of pi/2 subtracted from x) mod 4 = %d\n",
             *region);
#endif


}
#endif /* LIBM_INLINES_AMD_H_INCLUDED */
