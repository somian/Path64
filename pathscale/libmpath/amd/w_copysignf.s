#============================================================
#Copyright (c) 2002 Advanced Micro Devices, Inc.
#
#All rights reserved.
#
#Redistribution and  use in source and binary  forms, with or
#without  modification,  are   permitted  provided  that  the
#following conditions are met:
#
#+ Redistributions  of source  code  must  retain  the  above
#  copyright  notice,   this  list  of   conditions  and  the
#  following disclaimer.
#
#+ Redistributions  in binary  form must reproduce  the above
#  copyright  notice,   this  list  of   conditions  and  the
#  following  disclaimer in  the  documentation and/or  other
#  materials provided with the distribution.
#
#+ Neither the  name of Advanced Micro Devices,  Inc. nor the
#  names  of  its contributors  may  be  used  to endorse  or
#  promote  products  derived   from  this  software  without
#  specific prior written permission.
#
#THIS  SOFTWARE  IS PROVIDED  BY  THE  COPYRIGHT HOLDERS  AND
#CONTRIBUTORS "AS IS" AND  ANY EXPRESS OR IMPLIED WARRANTIES,
#INCLUDING,  BUT NOT  LIMITED TO,  THE IMPLIED  WARRANTIES OF
#MERCHANTABILITY  AND FITNESS  FOR A  PARTICULAR  PURPOSE ARE
#DISCLAIMED.  IN  NO  EVENT  SHALL  ADVANCED  MICRO  DEVICES,
#INC.  OR CONTRIBUTORS  BE LIABLE  FOR ANY  DIRECT, INDIRECT,
#INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES
#(INCLUDING,  BUT NOT LIMITED  TO, PROCUREMENT  OF SUBSTITUTE
#GOODS  OR  SERVICES;  LOSS  OF  USE, DATA,  OR  PROFITS;  OR
#BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON  ANY THEORY OF
#LIABILITY,  WHETHER IN CONTRACT,  STRICT LIABILITY,  OR TORT
#(INCLUDING NEGLIGENCE  OR OTHERWISE) ARISING IN  ANY WAY OUT
#OF  THE  USE  OF  THIS  SOFTWARE, EVEN  IF  ADVISED  OF  THE
#POSSIBILITY OF SUCH DAMAGE.
#
#It is  licensee's responsibility  to comply with  any export
#regulations applicable in licensee's jurisdiction.
#============================================================
# copysignf.gas
#
# An implementation of the copysignf libm function.
#
# Prototype:
#
#     float copysignf(float x, float y);
#
#   Returns the absolute value of x with the sign of y.
#   NaNs are not considered special; their sign bits are handled
#   the same as for any other number. */
#
#

	.weak copysignf
	.set copysignf,__copysignf

.section .text
#	.align 16
	.global __copysignf
	.type __copysignf,@function
__copysignf:

	lea	mask(%rip),%rdx	# point to the sign bit mask
	movaps	(%rdx),%xmm2	# and place in an xmm register
	
	andps	%xmm2,%xmm0	# strip x's sign bit
	andnps	%xmm1,%xmm2	# get y's sign bit
	orps	%xmm2,%xmm0	# copy the sign bit

	ret
	.align 16
mask:
	.long 0x7fffffff,0,0,0  # use the inverse of the mask

