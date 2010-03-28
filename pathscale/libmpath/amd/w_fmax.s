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
#
# Prototype:
#     double fmax(double x, double y);
#
# fmax(x, y) returns the larger (more positive) of x and y.
# NaNs are treated as missing values: if one argument is NaN,
# the other argument is returned. If both arguments are NaN,
# the first argument is returned.
#
# N.B. We use the ucomisd instruction in preference to the
# maxsd instruction because maxsd raises "invalid" exception
# if either argument is a quiet NaN. ucomisd only raises
# "invalid" for signalling NaNs.
#
	.weak	fmax
	.set	fmax,__fmax
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC0:
	.long	0
	.long	0
	.text
	.align 2
	.p2align 4,,15
.globl __fmax
	.type	__fmax,@function
__fmax:
	ucomisd	%xmm1, %xmm1		# Jump to L1 if y is NaN
	jne	.L1
	jp	.L1
	ucomisd	%xmm0, %xmm0		# Jump to L2 if x is NaN
	jne	.L2
	jp	.L2
	ucomisd	%xmm0, %xmm1		# Jump to L2 if y > x
	ja	.L2
	ucomisd	%xmm1, %xmm0		# Jump to L1 if x > y
	ja	.L1
	leaq	.LC0(%rip), %rdx	# Now x and y must be equal.
	ucomisd	(%rdx), %xmm0		# Jump to L1 if x and y are non-zero
	jne	.L1
	jp	.L1
	addsd	%xmm1, %xmm0		# Add the zeros to get correct sign
	.p2align 4,,7
.L1:
	ret
.L2:
	movapd	%xmm1, %xmm0
	ret
