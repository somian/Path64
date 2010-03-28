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
# An implementation of the finite libm function.
#
# Prototype:
#     int finitef(float x);
#
# Returns 0 if x is infinite or NaN, otherwise returns 1
#
#
	.file	"finitef_amd.c"
	.weak	finitef
	.set	finitef,__finitef
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC0:
	.long	0x7f800000
	.text
	.align 2
	.p2align 4,,15
.globl __finitef
	.type	__finitef,@function
__finitef:
	leaq	.LC0(%rip), %rdx	# point to the exponent bit mask
	movss	(%rdx), %xmm2		# and place in an xmm register
	xor	%eax,%eax		# clear the return value
	andps	%xmm2, %xmm0;		# mask for the exponent
	comiss	%xmm2, %xmm0		# compare with all ones
	setnz	%al			# set return value to 1 if not inf or NaN
	ret
