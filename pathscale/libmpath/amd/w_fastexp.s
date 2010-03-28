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
#  copyright  notice,  this   list  of   conditions  and  the
#  following disclaimer.
#
#+ Redistributions  in binary  form must reproduce  the above
#  copyright  notice,  this   list  of   conditions  and  the
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
# fastexp.s
#
# An implementation of the exp libm function.
#
# Prototype:
#
#     double fastexp(double x);
#
#   Computes e raised to the x power.
# Returns C99 values for error conditions, but may not
# set flags and other error status.   Denormal results are truncated to 0.
#
#

#	.weak exp
#	.set exp,fastexp
	.weak fastexp_
	.set fastexp_,__fastexp_

	.text
    .align 16
    .p2align 4,,15

#FORTRAN FASTEXP calling interface
# C equivalent
# double fastexp_(double * x)
# { return fastexp(*x); }
#

#.globl __fastexp_
    .type   __fastexp_,@function
__fastexp_:
    sub         $8,%rsp
    movlpd (%rdi),%xmm0
    call __fastexp
    add         $8,%rsp
    ret

        .align 16
        .p2align 4,,15

# define local variable storage offsets
.equ	p_temp,	0		# temporary for get/put bits operation

.equ    stack_size,0x018

#.globl fastexp
#       .type   fastexp,@function
#fastexp:
	.weak	fastexp
	.set	fastexp,__fastexp
	.type	__fastexp,@function
__fastexp:
	sub		$stack_size,%rsp



#        /* Find m, z1 and z2 such that exp(x) = 2**m * (z1 + z2) */
#      Step 1. Reduce the argument.
	#    r = x * thirtytwo_by_logbaseof2;
	movlpd	.__real_thirtytwo_by_log2(%rip),%xmm3	#
	mulsd	%xmm0,%xmm3 

# get x value to integer register.
	movlpd	%xmm0,p_temp(%rsp)
	mov	p_temp(%rsp),%rdx		# rdx is ux

#    /* Set n = nearest integer to r */
	cvtpd2dq	%xmm3,%xmm4			# convert to integer 
	lea		__two_to_jby32_lead_table(%rip),%rdi
	lea		__two_to_jby32_trail_table(%rip),%rsi
	cvtdq2pd	%xmm4,%xmm1				# and back to float.	


#    r1 = x - n * logbaseof2_by_32_lead;
	movlpd	.__real_log2_by_32_lead(%rip),%xmm2	#
	mulsd	%xmm1,%xmm2				#   
	movd	%xmm4,%ecx
	subsd	%xmm2,%xmm0				# r1 in xmm0, 
#    r2 =   - n * logbaseof2_by_32_trail;
	mulsd	.__real_log2_by_32_tail(%rip),%xmm1	# r2 in xmm1 
#    j = n & 0x0000001f;
	mov		$0x1f,%rax
	and		%ecx,%eax
	movsd	%xmm0,%xmm2
#    f1 = two_to_jby32_lead_table[j];
#    f2 = two_to_jby32_trail_table[j];

#    *m = (n - j) / 32;
	sub		%eax,%ecx
	sar		$5,%ecx					#m

	addsd	%xmm1,%xmm2    #r = r1 + r2

#      Step 2. Compute the polynomial.
#    q = r1 + (r2 +	
#              r*r*( 5.00000000000000008883e-01 +
#                      r*( 1.66666666665260878863e-01 +
#                      r*( 4.16666666662260795726e-02 +
#                      r*( 8.33336798434219616221e-03 +
#                      r*( 1.38889490863777199667e-03 ))))));
#    q = r + r^2/2 + r^3/6 + r^4/24 + r^5/120 + r^6/720	
	movsd	%xmm2,%xmm1
	movlpd	.__real_3f56c1728d739765(%rip),%xmm3	# 	1/720
	movlpd	.__real_3FC5555555548F7C(%rip),%xmm0	# 	1/6
	mulsd	%xmm2,%xmm3								# *x
	mulsd	%xmm2,%xmm0								# *x
	mulsd	%xmm2,%xmm1								# x*x
	movsd	%xmm1,%xmm4
	addsd	.__real_3F811115B7AA905E(%rip),%xmm3	# 	+ 1/120
	addsd	.__real_3fe0000000000000(%rip),%xmm0	# 	+ .5
	mulsd	%xmm1,%xmm4								# x^4
	mulsd	%xmm2,%xmm3								# *x
	mulsd	%xmm1,%xmm0								# *x^2
	addsd	.__real_3FA5555555545D4E(%rip),%xmm3	# 	+ 1/24
	addsd	%xmm2,%xmm0								# 	+ x
	mulsd	%xmm4,%xmm3								# *x^4
	addsd	%xmm3,%xmm0								# q = final sum

#    *z2 = f2 + ((f1 + f2) * q);
	movlpd	(%rsi,%rax,8),%xmm5		# f2
	addsd	(%rdi,%rax,8),%xmm5		# f1 + f2

# deal with infinite results
        mov     $1024, %rax
        movsx  %ecx,%rcx
        cmp     %rax,%rcx
        cmovg   %rax,%rcx                       # if infinite, then set rcx to multiply
                                                # by infinity
	mulsd	%xmm5,%xmm0
	addsd	%xmm5,%xmm0			#z = z1 + z2   done with 1,2,3,4,5

# deal with denormal results
        xor     %rax,%rax
        add	$1023, %rcx			# add bias
        cmovs   %rax,%rcx                       # if denormal  then multiply by 0
        shl	$52,%rcx                       # build 2^n

#  #if NaN or inf
	mov		$0x7ff0000000000000,%rax
	mov		%rax,%r10
	and		%rdx,%r10
	cmp		%rax,%r10

# end of splitexp				
#        /* Scale (z1 + z2) by 2.0**m */
#      Step 3. Reconstitute.

	mov		 %rcx,p_temp(%rsp) # get 2^n to memory		
	mulsd	p_temp(%rsp),%xmm0	# result *= 2^n

# we'd like to avoid a branch, and can use cmp's and and's to
# eliminate them.  But it adds cycles for normal cases which
# are supposed to be exceptions.  Using this branch with the
# check above results in faster code for the normal cases.
	jae		.Lexp_naninf

.Lfinal_check:
	add		$stack_size,%rsp
	ret

.Lexp_naninf:
	mov		$0x000FFFFFFFFFFFFF,%rax
	test	%rax,%rdx
	jnz		.Lenan			# jump if mantissa not zero, so it's a NaN
# inf
	mov		%rdx,%rax
	rcl		$1,%rax
	jnc		.Lr			# exp(+inf) = inf
	xor		%rdx ,%rdx 		# exp(-inf) = 0
	jmp		.Lr

#NaN
.Lenan:	
	mov		$0x0008000000000000,%rax	# convert to quiet
	or		%rax,%rdx
.Lr:
	movd	%rdx,%xmm0
	jmp		.Lfinal_check

	.data
        .align 16

.__real_3ff0000000000000: .quad 0x3ff0000000000000	# 1.0
			.quad 0					# for alignment
.__real_4040000000000000:	.quad 0x4040000000000000	# 32
			.quad 0
.__real_3fa0000000000000:	.quad 0x3fa0000000000000	# 1/32
			.quad 0
.__real_3fe0000000000000:	.quad 0x3fe0000000000000	# 1/2
			.quad 0
.__real___infinity:		.quad 0x7ff0000000000000	# 
			.quad 0					# for alignment
.__real_thirtytwo_by_log2: .quad 0x40471547652b82fe	# thirtytwo_by_log2
			.quad 0
.__real_log2_by_32_lead:  .quad 0x3f962e42fe000000	# log2_by_32_lead
			.quad 0
.__real_log2_by_32_tail:  .quad 0xbdcf473de6af278e	# -log2_by_32_tail
			.quad 0
.__real_3f56c1728d739765:	.quad 0x3f56c1728d739765	# 1.38889490863777199667e-03 used in splitexp
			.quad 0
.__real_3F811115B7AA905E:	.quad 0x3f811115b7aa905e	# 8.33336798434219616221e-03 used in splitexp
			.quad 0
.__real_3FA5555555545D4E:	.quad 0x3fa5555555545D4e	# 4.16666666662260795726e-02 used in splitexp
			.quad 0
.__real_3FC5555555548F7C:	.quad 0x3fC5555555548f7c	# 1.66666666665260878863e-01 used in splitexp
			.quad 0

        .align 16

__two_to_jby32_lead_table:
	.quad	0x3ff0000000000000 # 1 
	.quad	0x3ff059b0d0000000		# 1.0219
	.quad	0x3ff0b55860000000		# 1.04427
	.quad	0x3ff11301d0000000		# 1.06714
	.quad	0x3ff172b830000000		# 1.09051
	.quad	0x3ff1d48730000000		# 1.11439
	.quad	0x3ff2387a60000000		# 1.13879
	.quad	0x3ff29e9df0000000		# 1.16372
	.quad	0x3ff306fe00000000		# 1.18921
	.quad	0x3ff371a730000000		# 1.21525
	.quad	0x3ff3dea640000000		# 1.24186
	.quad	0x3ff44e0860000000		# 1.26905
	.quad	0x3ff4bfdad0000000		# 1.29684
	.quad	0x3ff5342b50000000		# 1.32524
	.quad	0x3ff5ab07d0000000		# 1.35426
	.quad	0x3ff6247eb0000000		# 1.38391
	.quad	0x3ff6a09e60000000		# 1.41421
	.quad	0x3ff71f75e0000000		# 1.44518
	.quad	0x3ff7a11470000000		# 1.47683
	.quad	0x3ff8258990000000		# 1.50916
	.quad	0x3ff8ace540000000		# 1.54221
	.quad	0x3ff93737b0000000		# 1.57598
	.quad	0x3ff9c49180000000		# 1.61049
	.quad	0x3ffa5503b0000000		# 1.64576
	.quad	0x3ffae89f90000000		# 1.68179
	.quad	0x3ffb7f76f0000000		# 1.71862
	.quad	0x3ffc199bd0000000		# 1.75625
	.quad	0x3ffcb720d0000000		# 1.79471
	.quad	0x3ffd5818d0000000		# 1.83401
	.quad	0x3ffdfc9730000000		# 1.87417
	.quad	0x3ffea4afa0000000		# 1.91521
	.quad	0x3fff507650000000		# 1.95714
	.quad 0					# for alignment
__two_to_jby32_trail_table:
	.quad	0x0000000000000000 # 0 
	.quad	0x3e48ac2ba1d73e2a		# 1.1489e-008
	.quad	0x3e69f3121ec53172		# 4.83347e-008
	.quad	0x3df25b50a4ebbf1b		# 2.67125e-010
	.quad	0x3e68faa2f5b9bef9		# 4.65271e-008
	.quad	0x3e368b9aa7805b80		# 5.24924e-009
	.quad	0x3e6ceac470cd83f6		# 5.38622e-008
	.quad	0x3e547f7b84b09745		# 1.90902e-008
	.quad	0x3e64636e2a5bd1ab		# 3.79764e-008
	.quad	0x3e5ceaa72a9c5154		# 2.69307e-008
	.quad	0x3e682468446b6824		# 4.49684e-008
	.quad	0x3e18624b40c4dbd0		# 1.41933e-009
	.quad	0x3e54d8a89c750e5e		# 1.94147e-008
	.quad	0x3e5a753e077c2a0f		# 2.46409e-008
	.quad	0x3e6a90a852b19260		# 4.94813e-008
	.quad	0x3e0d2ac258f87d03		# 8.48872e-010
	.quad	0x3e59fcef32422cbf		# 2.42032e-008
	.quad	0x3e61d8bee7ba46e2		# 3.3242e-008
	.quad	0x3e4f580c36bea881		# 1.45957e-008
	.quad	0x3e62999c25159f11		# 3.46453e-008
	.quad	0x3e415506dadd3e2a		# 8.0709e-009
	.quad	0x3e29b8bc9e8a0388		# 2.99439e-009
	.quad	0x3e451f8480e3e236		# 9.83622e-009
	.quad	0x3e41f12ae45a1224		# 8.35492e-009
	.quad	0x3e62b5a75abd0e6a		# 3.48493e-008
	.quad	0x3e47daf237553d84		# 1.11085e-008
	.quad	0x3e6b0aa538444196		# 5.03689e-008
	.quad	0x3e69df20d22a0798		# 4.81896e-008
	.quad	0x3e69f7490e4bb40b		# 4.83654e-008
	.quad	0x3e4bdcdaf5cb4656		# 1.29746e-008
	.quad	0x3e452486cc2c7b9d		# 9.84533e-009
	.quad	0x3e66dc8a80ce9f09		# 4.25828e-008
	.quad 0					# for alignment


