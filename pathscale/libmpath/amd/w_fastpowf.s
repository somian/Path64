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
# fastpowf.asm
#
# An implementation of the pow libm function.
#
# Prototype:
#
#     float fastpowf(float x, float y);
#
#   Computes x raised to the y power.  Returns proper C99 values.
#   Uses new tuned fastlogf/fastexpf.
#
#
#       .weak powf
#       .set powf,fastpowf
       .weak fastpowf_
       .set fastpowf_,__fastpowf_



# define local variable storage offsets
.equ	p_temp,0x00		# xmmword
.equ	p_negateres,0x10		# qword
.equ	p_inty,0x18		# dword

.equ	p_xexp,0x20		# qword

.equ	save_rbx,0x030		#qword



.equ	stack_size,0x088	# allocate 40h more than
				# we need to avoid bank conflicts



    .text
    .align 16
    .p2align 4,,15

#FORTRAN FASTPOWF calling interface
# C equivalent
# float fastpowf_(float * x, float * y)
# { return fastpowf(*x,*y); }
#

#.globl __fastpowf_
    .type   __fastpowf_,@function
__fastpowf_:
    sub		$8,%rsp
    movss (%rdi),%xmm0
    movss (%rsi),%xmm1
    call __fastpowf
    add		$8,%rsp
    ret


    .align 16
    .p2align 4,,15
#.globl fastpowf
#    .type   fastpowf,@function
#fastpowf:
	.weak	fastpowf
	.set	fastpowf,__fastpowf
	.type	__fastpowf,@function
__fastpowf:
	sub		$stack_size,%rsp
	movd	%xmm0,%edx			# edx is ux
	mov		%rbx,save_rbx(%rsp)	# save rbx
	movd	%xmm1,%r8d
	


# get the x and y values to integer registers.
	mov		$0x07fffffff,%r9d
	mov		%r9d,%r10d
	and		%edx,%r10d		# r10 is ax

	mov		%r10d,%ecx
	shr		$23,%ecx		#
	sub		$127,%ecx
	cvtsi2sd	%ecx,%xmm3		# xexp
# perform special case and error checking on input values
#
# if x == +1, return +1 for all x
	mov		$0x03f800000,%eax	# one
	cmp		%eax,%edx
	je		.L__powf_cleanup2

	and		%r8d,%r9d		# r9 is ay
# else if |y| ==0 
	test	%r9d,%r9d			# return 1
	je		.L__rone
# else if x is NAN 
	mov		$0x07f800000,%eax	# EXPBITS_SP32
	mov		%eax,%ebx
	and		%r10d,%eax
	mov		  %eax,p_xexp(%rsp)	# save (ax & EXPBITS_SP32)

	cmp		%ebx,%eax
	jnz		.L__fp1
	mov		$0x0007fffff,%eax	# MANTBITS_SP32
	and		%r10d,%eax
	jnz		.Lnp_special3
# else if y is NAN 
.L__fp1:
	mov		%ebx,%eax		# EXPBITS_SP32
	and		%r9d,%eax
	mov		%eax,%esi		# save (ay & EXPBITS_SP32)
	cmp		%ebx,%eax
	jne		.L__fp2
	mov		$0x0007fffff,%eax	# MANTBITS_SP32
	and		%r9d,%eax
	jnz		.Lnp_special4

# else if y == 1.0
.L__fp2:
	mov		$0x03f800000,%eax
	cmp		%r8d,%eax
	je		.Lnp_special5

# else if y is infinite or so large that the result would overflow or underflow
	mov		$0x04f000000,%eax
	cmp		%eax,%r9d
	ja		.Lnp_special6
#
# classify y
#
#  /* See whether y is an integer.
#     inty = 0 means not an integer.
#     inty = 1 means odd integer.
#     inty = 2 means even integer.
#  */
.Lclass_y:
# get yexp
	xor		%edi,%edi
	mov		%esi,%eax		# get (ay & EXPBITS_SP32)
	shr		$23,%eax		#>> EXPSHIFTBITS_SP32
	sub		$126,%eax		# - EXPBIAS_SP32 + 1   - eax is now the unbiased exponent
	mov		$1,%ebx
	cmp		%ebx,%eax		# if (yexp < 1)
	cmovl	%edi,%ebx
	jl		.Lsave_inty

	mov		$24,%ecx
	cmp		%ecx,%eax		# if (yexp >24)
	jle		.L__fp3
	mov		$2,%ebx
	jmp		.Lsave_inty
.L__fp3:						# else 1<=yexp<=24
	sub		%eax,%ecx		# build mask for mantissa
	shl		%cl,%ebx
	dec		%ebx			# rbx = mask = (1 << (24 - yexp)) - 1

	mov		%r8d,%eax	
	and		%ebx,%eax		# if ((uy & mask) != 0)
	cmovnz	%edi,%ebx			#   inty = 0;
	jnz		.Lsave_inty

	not		%ebx			# else if (((uy & ~mask) >> (24 - yexp)) & 0x00000001)
	mov		%r8d,%eax	
	and		%ebx,%eax
	shr		%cl,%eax
	inc		%edi			
	and		%edi,%eax
	mov		%edi,%ebx		#  inty = 1
	jnz		.Lsave_inty
	inc		%ebx			# else	inty = 2		


.Lsave_inty:
	mov		  %ebx,p_inty(%rsp)	# save inty

#
# do more x special case checking
#

.Lmore_x:

# if x is infinity (NaN was already ruled out).
	mov		$0x07f800000,%eax	# EXPBITS_SP32
	cmp		p_xexp(%rsp),%eax	# compare with (ax & EXPBITS_SP32)
	je		.Lnp_special_x1		# jump if x is infinite
# else if ax == 0
	test	%r10d,%r10d
	je		.Lnp_special_x2

	
	xor		%rax,%rax		# negateres = 0
# if x is negative
	cmp		%r10d,%edx
	je		.L__fp4

	# x is negative
	test	%ebx,%ebx			 # if (inty)
	je	.Lnp_special_x3			 # jump if not an integer


	movd	%r10d,%xmm0
	mov	%r10d,%edx
	
	cmp	$1,%ebx
	cmove	%ebx,%eax
.L__fp4:
	shl		$63,%rax
	mov	  %rax,p_negateres(%rsp)	# save negateres


#
# finally check if |y| is very small
#
	mov		$0x02e800000,%eax	# if abs(y) < 2^(-34)
	cmp		%eax,%r9d
	jl		.Lnp_special_x4		# jump if so




# **** Here starts the main calculations  ****
# The algorithm used is x**y = exp(y*log(x))
#  Extra precision is required in intermediate steps to meet the 1ulp requirement
#
# log(x) calculation
.Lpow_log:
	cvtss2sd	%xmm0,%xmm0
	cvtss2sd	%xmm1,%xmm1		# convert y to double

#      /* Store the exponent of x in xexp and put
#         f into the range [0.5,1) */
# all of the above integer arithmetic done in single
# precision.  In this version of powf, we now promote
# x and y to double and use the double precision pow
# algorithm, with modifications.
#
# compute the index into the log tables
#
	mov		$0x0007fffff,%eax	# mantissa bit mask
	and		%edx,%eax
	mov		%eax,%r8d		# save the mantissa

#/* Now  x = 2**xexp  * f,  1/2 <= f < 1. */
	shr		$16,%r8d
	mov		%r8d,%r9d
	shr		$1,%r8d
	add		$0x040,%r8d
	and		$1,%r9d
	add		%r9d,%r8d			#index

# reduce and get u
	cvtsi2sd	%r8d,%xmm4			# convert index to float
	mov		$0x03fe0000000000000,%rdx
	shl		$29,%rax			# shift mantissa bits to DP container
	or		%rdx,%rax
	movd	%rax,%xmm2				# f 

	movlpd	.L__real_half(%rip),%xmm5			# .5

	mulsd	.L__real_3f80000000000000(%rip),%xmm4		# f1 = index/128


	lea		.L__np_ln_table(%rip),%r9
        movlpd   -512(%r9,%r8,8),%xmm0          # z1

	subsd	%xmm4,%xmm2											# f2 = f - f1
	mulsd	%xmm2,%xmm5
	addsd	%xmm5,%xmm4				# denominator
	divsd	%xmm4,%xmm2				# u

# solve for ln(1+u)
	movsd	%xmm2,%xmm4				# u
	movlpd	.L__real_cb2(%rip),%xmm5
	mulsd	%xmm2,%xmm2				# u^2

	mulsd	.L__real_log2(%rip),%xmm3			# xexp * log2_lead
	mulsd	%xmm2,%xmm5				# Bu2

	addsd	%xmm3,%xmm0				# xexp*log2	+ z1
	mulsd	%xmm4,%xmm2				# u^3

	addsd	.L__real_cb1(%rip),%xmm5			# A + Bu2
	addsd	%xmm4,%xmm0				# xexp*log2	+ z1 + u

	mulsd	%xmm5,%xmm2				# u3 * (A + Bu2)
	addsd	%xmm2,%xmm0				# sum
# y* logx
#  /* just multiply by y */
	mulsd	%xmm1,%xmm0				#  x * y
#  /* Test for overflow and underflow due to y*log2(x)
#     being too large or small. */
#
	comisd	.L__real_40562E4320000000(%rip),%xmm0 	# 88.712
	ja		.Lnp_special_lx1
#
	comisd	.L__real_C059FE3680000000(%rip),%xmm0 	# -103.9720764160
	jc		.Lnp_special_lx2
#
#
#  /* The following code computes r = exp(w) */
#
#        /* Find m, z1 and z2 such that exp(x) = 2**m * (z1 + z2) */
#      splitexp(x, 1.0, thirtytwo_by_log2, log2_by_32_lead,
#	       log2_by_32_tail, &m, &z1, &z2);
#splitexp(double x, double logbase(=1),
#                            double thirtytwo_by_logbaseof2(= thirtytwo_by_log2),
#                            double logbaseof2_by_32_lead(=log2_by_32_lead),
#                            double logbaseof2_by_32_trail(=log2_by_32_tail),
#                            int *m, double *z1, double *z2)
#      Step 1. Reduce the argument.
	#    r = x * thirtytwo_by_logbaseof2;
	movlpd	.L__real_thirtytwo_by_log2(%rip),%xmm3
	mulsd	%xmm0,%xmm3 

# get x value to integer register.
	movd	%xmm0,%rdx

#    /* Set n = nearest integer to r */
	cvtpd2dq	%xmm3,%xmm5
	lea		.L__two_to_jby32_table(%rip),%rsi
	cvtdq2pd	%xmm5,%xmm1


#    r1 = x - n * logbaseof2_by_32_lead;
	movlpd	.L__real_log2_by_32(%rip),%xmm2
	mulsd	%xmm1,%xmm2
	movd		%xmm5,%ecx
	subsd	%xmm2,%xmm0	 			# r1 in xmm0, 
#    j = n & 0x0000001f;
	mov		$0x01f,%rax
	mov		$1024,%edx
	movsd	%xmm0,%xmm2
	and		%ecx,%eax

#    f = two_to_jby32_table[j];
        movlpd    (%rsi,%rax,8),%xmm5			# f (f1+f2)

#    *m = (n - j) / 32;
	sub		%eax,%ecx
	sar		$5,%ecx				#m
	cmp		%edx,%ecx
	cmovg	%edx,%ecx

	movlpd	.L__real_3FC5555555548F7C(%rip),%xmm1		# 	1/6
	mulsd	%xmm2,%xmm2				# x*x
	movlpd	.L__real_3FA5555555545D4E(%rip),%xmm4		# 	1/24
	mulsd	%xmm0,%xmm1				# r/6
	movlpd	.L__real_half(%rip),%xmm3			# 	1/2
	mulsd	%xmm2,%xmm4				# r2/24
	mulsd	%xmm2,%xmm3				# r2/2

	addsd	%xmm4,%xmm1				# r/6 + r2/24
	addsd	%xmm3,%xmm0				# r + r2/2

	mulsd	%xmm2,%xmm1				# r3/6 + r4/24
	addsd	%xmm1,%xmm0				# q

#    *z2 = f2 + ((f1 + f2) * q);
	xor		%eax,%eax
	mulsd	%xmm5,%xmm0
	addsd	%xmm5,%xmm0				#z = z1 + z2
# end of splitexp				
#        /* Scale (z1 + z2) by 2.0**m */
#          r = scaleDouble_1(z, n);

	add		$1023,%ecx			# add bias
#	js		.L__denormal
	cmovs	%eax,%ecx
	orpd 	p_negateres(%rsp),%xmm0
	shl		$52,%rcx			# build 2^n
	mov		%rcx,p_temp(%rsp)
	mulsd	p_temp(%rsp),%xmm0
#
.L__powf_cleanup:
	cvtsd2ss	%xmm0,%xmm0
.L__powf_cleanup2:

	mov		save_rbx(%rsp),%rbx		# restore rbx
	add		$stack_size,%rsp
	ret

	.align 16

.L__rzero:
	xorpd	%xmm0,%xmm0
	jmp		.L__powf_cleanup

	.align 16
.L__rone:
	movlpd	.L__real_one(%rip),%xmm0
	jmp		.L__powf_cleanup

	.align 16
.L__infinity:
	movlpd	.L__real_infinity(%rip),%xmm0
	jmp		.L__powf_cleanup


#
# here is the code for special cases

	.align 16
# y ==NaN
.Lnp_special4:
	movd	%xmm1,%edx
# x is a NaN
.Lnp_special3:
#NaN
.L__enan:	
	mov		$0x000400000,%eax	# convert to quiet
	or		%eax,%edx
	movd	%edx,%xmm0
	jmp		.L__powf_cleanup2


# y == 1.0, just return x.
.Lnp_special5:
	jmp		.L__powf_cleanup2


	.align 16

#      /* y is infinite or so large that the result would
#         overflow or underflow.

.Lnp_special6:
	cmp		%r9d,%r8d										# is y positive
	jnz		.Lnps6				# jump if not
	test	%r10d,%r10d
	jz		.L__rzero

	mov		$0x0bf800000,%eax		# is x== -1
	cmp		%eax,%edx										
	jz		.L__rone
	mov		$0x03f800000,%eax		# is |x| < 1.0
	cmp		%eax,%r10d										
	jl		.L__rzero
#  abs(x) < 1.0
	jmp		.L__infinity 									 
# y < 0
.Lnps6:
	test	%r10d,%r10d
	jz		.L__infinity									# if |x| == 0

	mov		$0x03f800000,%eax		# is |x| < 1.0
	cmp		%eax,%r10d										
	jge		.L__fp5
#  abs(x) < 1.0
	mov		$0x07f800000,%eax		# is y == inf
	cmp		%eax,%r9d
	jz		.L__infinity										# return inf if so
	jmp		.L__infinity
.L__fp5:
	jnz		.Lnpx64										# jump if |x| != 1.0 

	movd	%r10d,%xmm0										# return 1
	jmp		.L__powf_cleanup2
# |x| > 1.0
.Lnpx64:													 
	xorps	%xmm0,%xmm0										# return 0
	jmp		.L__powf_cleanup2

	.align 16
.Lnp_special_x1:											# x is infinite
	cmp		%r10d,%edx			# is x positive
	jnz		.Lnsx11				# jump if not
	cmp		%r9d,%r8d			# is y positive
	jz		.L__fp6				# just return if so
	xorps	%xmm0,%xmm0				# else return 0
.L__fp6:
	jmp		.L__powf_cleanup2

.Lnsx11:
	mov		p_inty(%rsp),%eax		# if inty ==1
	cmp		$1,%eax
	jnz		.Lnsx12				# jump if not
	cmp		%r9d,%r8d			# is ypos
	jz		.L__fp7				# just return if so
	mov		$0x080000000,%eax		# else return -0
	movd	%eax,%xmm0
.L__fp7:
	jmp		.L__powf_cleanup2
.Lnsx12:						# inty <>1
	cmp		%r9d,%r8d			# is y positive
	xorps	%xmm0,%xmm0				# else return 0
	jnz		.L__fp8				# just return if so
	movd	%r10d,%xmm0									# return -x (|x|)
.L__fp8:	jmp		.L__powf_cleanup2

	.align 16
.Lnp_special_x2:					#ax == 0
	cmp		%r10d,%edx			# is x positive
	jnz		.Lnsx21				# jump if not
	cmp		%r9d,%r8d			# is ypos
	xorps	%xmm0,%xmm0				# 
	jz		.L__powf_cleanup2		# return x (0) if so
	jmp		.L__infinity
.Lnsx21:
	cmp		%r9d,%r8d			# is ypos
	jnz		.Lnsx22				# 
	mov		p_inty(%rsp),%eax		# if inty ==1
	cmp		$1,%eax
	jz		.L__powf_cleanup2		# just return if so
	xorps	%xmm0,%xmm0				# return +0 if -0 raised to + odd integer
	jmp		.L__powf_cleanup2
.Lnsx22:
	mov		p_inty(%rsp),%eax		# if inty ==1
	cmp		$1,%eax
	mov		$0x0ff800000,%eax
	mov		$0x07f800000,%r8d
	cmovz	%eax,%r8d
	movd	%r8d,%xmm0
	jmp		.L__powf_cleanup2

	.align 16
.Lnp_special_x3:
	# x < 0 and y is not an integer
	mov		$0x07fC00000,%r8d
	movd	%r8d,%xmm0
	jmp		.L__powf_cleanup2

.Lnp_special_x4:
	mov		$0x03f800000,%eax
	movd	%eax,%xmm0
	addss	%xmm1,%xmm0
	jmp		.L__powf_cleanup2

	.align 16
# result will be too large or too small
.Lnp_special_lx2:											# too small
	mov		p_negateres(%rsp),%rax		# get negateres
	xor		%rdx,%rdx
	cmp		$0,%rax
	mov		$0x08000000000000000,%r8
	cmovz	%rdx,%r8
	movd	%r8,%xmm0
	jmp		.L__powf_cleanup
	.align 16
.Lnp_special_lx1:											# too large
	mov		p_negateres(%rsp),%rax		# get negateres
	cmp		$0,%rax
	mov		$0x0fff0000000000000,%rax
	mov		$0x07ff0000000000000,%r8
	cmovnz	%rax,%r8
	movd	%r8,%xmm0
	jmp		.L__powf_cleanup
	.align 16
.Lnp_special_lx3:											# denormal
	mov		p_negateres(%rsp),%rax		# get negateres
	xor		%rdx,%rdx
	cmp		$0,%rax
	mov		$0x08000000000000000,%r8
	cmovnz	%rdx,%r8
	movd	%r8,%xmm1
	orpd	%xmm1,%xmm0
	jmp		.L__powf_cleanup	

        .data
        .align 64

.L__np_ln_table:
   .quad 0x0000000000000000   #                   0
   .quad 0x3F8FC0A8B0FC03E4   #0.015504186535965254
   .quad 0x3F9F829B0E783300   #0.030771658666753687
   .quad 0x3FA77458F632DCFC   #0.045809536031294201
   .quad 0x3FAF0A30C01162A6   # 0.06062462181643484
   .quad 0x3FB341D7961BD1D1   #0.075223421237587532
   .quad 0x3FB6F0D28AE56B4C   #0.089612158689687138
   .quad 0x3FBA926D3A4AD563   # 0.10379679368164356
   .quad 0x3FBE27076E2AF2E6   # 0.11778303565638346
   .quad 0x3FC0D77E7CD08E59   # 0.13157635778871926
   .quad 0x3FC29552F81FF523   # 0.14518200984449789
   .quad 0x3FC44D2B6CCB7D1E   # 0.15860503017663857
   .quad 0x3FC5FF3070A793D4   # 0.17185025692665923
   .quad 0x3FC7AB890210D909   # 0.18492233849401199
   .quad 0x3FC9525A9CF456B4   # 0.19782574332991987
   .quad 0x3FCAF3C94E80BFF3   # 0.21056476910734964
   .quad 0x3FCC8FF7C79A9A22   # 0.22314355131420976
   .quad 0x3FCE27076E2AF2E6   # 0.23556607131276691
   .quad 0x3FCFB9186D5E3E2B   # 0.24783616390458127
   .quad 0x3FD0A324E27390E3   # 0.25995752443692605
   .quad 0x3FD1675CABABA60E   # 0.27193371548364176
   .quad 0x3FD22941FBCF7966   # 0.28376817313064462
   .quad 0x3FD2E8E2BAE11D31   #  0.2954642128938359
   .quad 0x3FD3A64C556945EA   # 0.30702503529491187
   .quad 0x3FD4618BC21C5EC2   # 0.31845373111853459
   .quad 0x3FD51AAD872DF82D   # 0.32975328637246798
   .quad 0x3FD5D1BDBF5809CA   # 0.34092658697059319
   .quad 0x3FD686C81E9B14AF   #  0.3519764231571782
   .quad 0x3FD739D7F6BBD007   # 0.36290549368936847
   .quad 0x3FD7EAF83B82AFC3   # 0.37371640979358406
   .quad 0x3FD89A3386C1425B   # 0.38441169891033206
   .quad 0x3FD947941C2116FB   # 0.39499380824086899
   .quad 0x3FD9F323ECBF984C   # 0.40546510810816438
   .quad 0x3FDA9CEC9A9A084A   # 0.41582789514371099
   .quad 0x3FDB44F77BCC8F63   # 0.42608439531090009
   .quad 0x3FDBEB4D9DA71B7C   # 0.43623676677491807
   .quad 0x3FDC8FF7C79A9A22   # 0.44628710262841953
   .quad 0x3FDD32FE7E00EBD5   # 0.45623743348158757
   .quad 0x3FDDD46A04C1C4A1   # 0.46608972992459924
   .quad 0x3FDE744261D68788   # 0.47584590486996392
   .quad 0x3FDF128F5FAF06ED   # 0.48550781578170082
   .quad 0x3FDFAF588F78F31F   # 0.49507726679785152
   .quad 0x3FE02552A5A5D0FF   # 0.50455601075239531
   .quad 0x3FE0723E5C1CDF40   # 0.51394575110223428
   .quad 0x3FE0BE72E4252A83   # 0.52324814376454787
   .quad 0x3FE109F39E2D4C97   # 0.53246479886947184
   .quad 0x3FE154C3D2F4D5EA   # 0.54159728243274441
   .quad 0x3FE19EE6B467C96F   #  0.5506471179526623
   .quad 0x3FE1E85F5E7040D0   # 0.55961578793542266
   .quad 0x3FE23130D7BEBF43   # 0.56850473535266877
   .quad 0x3FE2795E1289B11B   # 0.57731536503482361
   .quad 0x3FE2C0E9ED448E8C   # 0.58604904500357824
   .quad 0x3FE307D7334F10BE   # 0.59470710774669278
   .quad 0x3FE34E289D9CE1D3   # 0.60329085143808425
   .quad 0x3FE393E0D3562A1A   # 0.61180154110599294
   .quad 0x3FE3D9026A7156FB   # 0.62024040975185757
   .quad 0x3FE41D8FE84672AE   # 0.62860865942237409
   .quad 0x3FE4618BC21C5EC2   # 0.63690746223706918
   .quad 0x3FE4A4F85DB03EBB   #  0.6451379613735847
   .quad 0x3FE4E7D811B75BB1   # 0.65330127201274568
   .quad 0x3FE52A2D265BC5AB   # 0.66139848224536502
   .quad 0x3FE56BF9D5B3F399   # 0.66943065394262924
   .quad 0x3FE5AD404C359F2D   # 0.67739882359180614
   .quad 0x3FE5EE02A9241675   # 0.68530400309891937
   .quad 0x3FE62E42FEFA39EF   # 0.69314718055994529
	.quad 0					# for alignment



.L__two_to_jby32_table:
	.quad	0x03FF0000000000000 # 1 
	.quad	0x03FF059B0D3158574		# 1.0219
	.quad	0x03FF0B5586CF9890F		# 1.04427
	.quad	0x03FF11301D0125B51		# 1.06714
	.quad	0x03FF172B83C7D517B		# 1.09051
	.quad	0x03FF1D4873168B9AA		# 1.11439
	.quad	0x03FF2387A6E756238		# 1.13879
	.quad	0x03FF29E9DF51FDEE1		# 1.16372
	.quad	0x03FF306FE0A31B715		# 1.18921
	.quad	0x03FF371A7373AA9CB		# 1.21525
	.quad	0x03FF3DEA64C123422		# 1.24186
	.quad	0x03FF44E086061892D		# 1.26905
	.quad	0x03FF4BFDAD5362A27		# 1.29684
	.quad	0x03FF5342B569D4F82		# 1.32524
	.quad	0x03FF5AB07DD485429		# 1.35426
	.quad	0x03FF6247EB03A5585		# 1.38391
	.quad	0x03FF6A09E667F3BCD		# 1.41421
	.quad	0x03FF71F75E8EC5F74		# 1.44518
	.quad	0x03FF7A11473EB0187		# 1.47683
	.quad	0x03FF82589994CCE13		# 1.50916
	.quad	0x03FF8ACE5422AA0DB		# 1.54221
	.quad	0x03FF93737B0CDC5E5		# 1.57598
	.quad	0x03FF9C49182A3F090		# 1.61049
	.quad	0x03FFA5503B23E255D		# 1.64576
	.quad	0x03FFAE89F995AD3AD		# 1.68179
	.quad	0x03FFB7F76F2FB5E47		# 1.71862
	.quad	0x03FFC199BDD85529C		# 1.75625
	.quad	0x03FFCB720DCEF9069		# 1.79471
	.quad	0x03FFD5818DCFBA487		# 1.83401
	.quad	0x03FFDFC97337B9B5F		# 1.87417
	.quad	0x03FFEA4AFA2A490DA		# 1.91521
	.quad	0x03FFF50765B6E4540		# 1.95714
	.quad 0					# for alignment

.L__real_3f80000000000000:	.quad 0x03f80000000000000	# /* 0.0078125 = 1/128 */
				.quad 0


.L__real_40562E4320000000:	.quad 0x040562E4320000000	# 88.7228469849
				.quad 0
.L__real_C059FE3680000000:	.quad 0x0C059FE3680000000	#  -87.3365402222 smallest x for non-denormal expf
				.quad 0

.L__real_3FA5555555545D4E:	.quad 0x03FA5555555545D4E	# 4.16666666662260795726e-02 used in splitexp
				.quad 0
.L__real_3FC5555555548F7C:	.quad 0x03FC5555555548F7C	# 1.66666666665260878863e-01 used in splitexp
				.quad 0


.L__real_one:			.quad 0x03ff0000000000000	# 1.0
				.quad 0			
.L__real_two:			.quad 0x04000000000000000	# 1.0
				.quad 0

.L__real_ca1:			.quad 0x03fb55555555554e6	# 8.33333333333317923934e-02
				.quad 0		
.L__real_ca2:			.quad 0x03f89999999bac6d4	# 1.25000000037717509602e-02
				.quad 0	

# from Maple:
#Digits :=40;
#minimax(2*ln(1+v/2), v=0..1/256, 7,1 ,'maxerror');
.L__real_cb1:			.quad 0x03fb555555555552c	# 0.08333333333333275459088388736767942281572 from maple
				.quad 0		
.L__real_cb2:			.quad 0x03F8999998EAB53DB	# 0.01249999968187325554473232707489405493533
				.quad 0		

.L__real_log2:	  		.quad 0x03FE62E42FEFA39EF	# log2_lead	  6.9314718055994530941723e-01
				.quad 0			

.L__real_half:			.quad 0x03fe0000000000000	# 1/2
				.quad 0			

.L__real_infinity:		.quad 0x07ff0000000000000	# 
				.quad 0		
.L__real_thirtytwo_by_log2: .quad 0x040471547652b82fe	# thirtytwo_by_log2
				.quad 0
.L__real_log2_by_32:       .quad 0x03F962E42FEFA39EF	# log2_by_32
				.quad 0


