#============================================================
#Copyright (c) 2004 Advanced Micro Devices, Inc.
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

# Copyright 2005 PathScale, Inc.  All Rights Reserved.

#============================================================
#
# fastexpf.s
#
# An implementation of the expf libm function.
#
# Prototype:
#
#     float fastexpf(float x);
#
#   Computes e raised to the x power.
# Does not perform error checking.   Denormal results are truncated to 0.
#
#


#       .weak expf
#       .set expf,fastexpf
       .weak fastexpf_
       .set fastexpf_,__fastexpf_

        .text
    .align 16
    .p2align 4,,15

#FORTRAN FASTEXPF calling interface
# C equivalent
# float fastexpf_(float * x)
# { return fastexpf(*x); }
#

#.globl __fastexpf_
    .type   __fastexpf_,@function
__fastexpf_:
    sub         $8,%rsp
    movss (%rdi),%xmm0
    call __fastexpf
    add         $8,%rsp
    ret

        .align 16
        .p2align 4,,15

# define local variable storage offsets
.equ	p_temp,0		# temporary for get/put bits operation
.equ	stack_size,0x18



#.globl fastexpf
#       .type   fastexpf,@function
#fastexpf:
	.weak	fastexpf
	.set	fastexpf,__fastexpf
	.type	__fastexpf,@function
__fastexpf:
	sub	$stack_size,%rsp

	movss	 %xmm0,p_temp(%rsp)
	cvtss2sd	%xmm0,%xmm0

#        /* Find m, z1 and z2 such that exp(x) = 2**m * (z1 + z2) */
#      Step 1. Reduce the argument.
	#    r = x * thirtytwo_by_logbaseof2;
	movlpd	.L__real_thirtytwo_by_log2(%rip),%xmm2	#
	mulsd	%xmm0,%xmm2 

# get x value to integer register.
	mov		p_temp(%rsp),%edx		# edx is ux
#    /* Set n = nearest integer to r */
	cvtpd2dq	%xmm2,%xmm3
	lea		.L__two_to_jby32_table(%rip),%rdi
	cvtdq2pd	%xmm3,%xmm1


#    r1 = x - n * logbaseof2_by_32_lead;
	mulsd	.L__real_log2_by_32(%rip),%xmm1		
	movd	%xmm3,%ecx
	subsd	%xmm1,%xmm0				# r1 in xmm0, 

#    j = n & 0x0000001f;
	mov		$0x01f,%rax
	and		%ecx,%eax
	movsd	%xmm0,%xmm1
#    f1 = two_to_jby32_lead_table[j];

#    *m = (n - j) / 32;
	sub		%eax,%ecx
	sar		$5,%ecx					#m


#      Step 2. Compute the polynomial.
#     q = r1 + 	
#              r*r*( 5.00000000000000008883e-01 +
#                      r*( 1.66666666665260878863e-01 +
#                      r*( 4.16666666662260795726e-02 +
#                      r*( 8.33336798434219616221e-03 +
#                      r*( 1.38889490863777199667e-03 )))));
#    q = r + r^2/2 + r^3/6 + r^4/24 + r^5/120 + r^6/720	
#    q = r + r^2/2 + r^3/6   good enough for single precision	
	movlpd	.L__real_3FC5555555548F7C(%rip),%xmm2	# 	1/6
	mulsd	%xmm0,%xmm2								# *x
	mulsd	%xmm0,%xmm0								# x*x
	addsd	.L__real_3fe0000000000000(%rip),%xmm2	# 	+ .5
	mulsd	%xmm2,%xmm0								# *x^2
	addsd	%xmm1,%xmm0								# 	+ x
#    *z2 = f2 + ((f1 + f2) * q);
	movlpd	(%rdi,%rax,8),%xmm4		# f1

# deal with infinite results
	mov		$128,%eax
	cmp		%eax,%ecx
	cmovg	%eax,%ecx			# if infinite, then set rcx to multiply
						# by infinity
	mulsd	%xmm4,%xmm0
	addsd	%xmm4,%xmm0			#z = z1 + z2   done with 1,2,3,4,5

	cvtsd2ss	%xmm0,%xmm0
# deal with denormal results
	xor		%eax,%eax
	add		$127,%ecx		# add bias
	cmovs	%eax,%ecx			# if denormal, then multiply by 0
	shl		$23,%ecx		# build 2^n

#  if NaN or inf
	mov		$0x07f800000,%eax
	mov		%eax,%r10d
	and		%edx,%r10d
	cmp		%eax,%r10d


# end of splitexp				
#        /* Scale (z1 + z2) by 2.0**m */
#      Step 3. Reconstitute.

	mov		 %ecx,p_temp(%rsp) # get 2^n to memory		
	mulss	p_temp(%rsp),%xmm0	# result *= 2^n

# we'd like to avoid a branch, and can use cmp's and and's to
# eliminate them.  But it adds cycles for normal cases 
# to handle events that are supposed to be exceptions.
#  Using this branch with the
# check above results in faster code for the normal cases.
# And branch mispredict penalties should only come into
# play for nans and infinities.
	jae		.L__exp_naninf

#
.L__final_check:
#	cvtsd2ss	%xmm0,%xmm0
	add		$stack_size,%rsp
	ret

#  deal with nans and infinities

.L__exp_naninf:
	xorps	%xmm0,%xmm0
	mov	$0x0007FFFFF,%eax
	test	%eax,%edx
	jnz		.L__enan					# jump if mantissa not zero, so it's a NaN
# inf
	mov		%edx,%eax
	rcl		$1,%eax
	jnc		.L__r				# exp(+inf) = inf
	xor		%edx,%edx				# exp(-inf) = 0
	jmp		.L__r

#NaN
.L__enan:	
	mov		$0x000400000,%eax	# convert to quiet
	or		%eax,%edx
.L__r:
	movd	%edx,%xmm0
	jmp		.L__final_check

	.data
        .align 64

.L__real_3fe0000000000000:	.quad 0x03fe0000000000000	# 1/2
						.quad 0
.L__real_thirtytwo_by_log2: .quad 0x040471547652b82fe	# thirtytwo_by_log2
						.quad 0
.L__real_log2_by_32:		.quad 0x3F962E42FEFA39EF	# log2_by_32
						.quad 0
.L__real_3FC5555555548F7C:	.quad 0x3FC5555555548F7C	# 1.66666666665260878863e-01 used in splitexp
						.quad 0


.L__two_to_jby32_table:
        .quad   0x03FF0000000000000             # 1.0000000000000000
        .quad   0x03FF059B0D3158574             # 1.0218971486541166
        .quad   0x03FF0B5586CF9890F             # 1.0442737824274138
        .quad   0x03FF11301D0125B51             # 1.0671404006768237
        .quad   0x03FF172B83C7D517B             # 1.0905077326652577
        .quad   0x03FF1D4873168B9AA             # 1.1143867425958924
        .quad   0x03FF2387A6E756238             # 1.1387886347566916
        .quad   0x03FF29E9DF51FDEE1             # 1.1637248587775775
        .quad   0x03FF306FE0A31B715             # 1.1892071150027210
        .quad   0x03FF371A7373AA9CB             # 1.2152473599804690
        .quad   0x03FF3DEA64C123422             # 1.2418578120734840
        .quad   0x03FF44E086061892D             # 1.2690509571917332
        .quad   0x03FF4BFDAD5362A27             # 1.2968395546510096
        .quad   0x03FF5342B569D4F82             # 1.3252366431597413
        .quad   0x03FF5AB07DD485429             # 1.3542555469368927
        .quad   0x03FF6247EB03A5585             # 1.3839098819638320
        .quad   0x03FF6A09E667F3BCD             # 1.4142135623730951
        .quad   0x03FF71F75E8EC5F74             # 1.4451808069770467
        .quad   0x03FF7A11473EB0187             # 1.4768261459394993
        .quad   0x03FF82589994CCE13             # 1.5091644275934228
        .quad   0x03FF8ACE5422AA0DB             # 1.5422108254079407
        .quad   0x03FF93737B0CDC5E5             # 1.5759808451078865
        .quad   0x03FF9C49182A3F090             # 1.6104903319492543
        .quad   0x03FFA5503B23E255D             # 1.6457554781539649
        .quad   0x03FFAE89F995AD3AD             # 1.6817928305074290
        .quad   0x03FFB7F76F2FB5E47             # 1.7186192981224779
        .quad   0x03FFC199BDD85529C             # 1.7562521603732995
        .quad   0x03FFCB720DCEF9069             # 1.7947090750031072
        .quad   0x03FFD5818DCFBA487             # 1.8340080864093424
        .quad   0x03FFDFC97337B9B5F             # 1.8741676341103000
        .quad   0x03FFEA4AFA2A490DA             # 1.9152065613971474
        .quad   0x03FFF50765B6E4540             # 1.9571441241754002
	.quad 0					# for alignment

