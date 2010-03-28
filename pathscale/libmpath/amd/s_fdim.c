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

#define USE_NAN_WITH_FLAGS
#include "libm_inlines_amd.h"
#undef USE_NAN_WITH_FLAGS

double FN_PROTOTYPE(fdim)(double x, double y)
{
  /* Returns the positive difference between x and y.
     If x <= y, returns +0.0.
     If x > y, returns x - y.
     If one argument is NaN, that argument is returned.
     If both arguments are NaN, the first argument is returned. */

  /* This works under 64-bit Linux and Windows */
  if (x != x || y != y)
    /* x or y is NaN */
    return x + y; /* Raise signal if it's a signalling NaN */
  else if (x >= y)
    return x - y;
  else
    return 0.0;
}

weak_alias (__fdim, fdim)
