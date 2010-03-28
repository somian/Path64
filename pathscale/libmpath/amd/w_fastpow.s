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
# pow.s
#
# An implementation of the pow libm function.
#
# Prototype:
#
#     double pow(double x, double y);
#
#   Computes x raised to the y power.  Returns proper C99 values.
#   Uses new tuned fastlog/fastexp.
#
#




# define local variable storage offsets
.equ	p_temp,0x00		# xmmword
.equ	p_negateres,0x10		# qword
.equ	p_inty,0x18		# dword


.equ	save_rbx,0x020		#qword

.equ	p_xexp,0x28		# qword
.equ	p_yexp,0x30		# qword

.equ	p_hy,0x38		# qword
.equ	p_ty,0x40		# qword
.equ	p_corr,0x48		# qword

.equ	stack_size,0x068


#        .weak pow
#        .set pow,fastpow
        .weak fastpow_
        .set fastpow_,__fastpow_


    .align 16
    .p2align 4,,15

#FORTRAN FASTPOW calling interface
# C equivalent
# double fastpow_(double * x, double * y)
# { return fastpow(*x,*y); }
#

#.globl __fastpow_
    .type   __fastpow_,@function
__fastpow_:
    sub         $8,%rsp
    movlpd (%rdi),%xmm0
    movlpd (%rsi),%xmm1
    call __fastpow
    add         $8,%rsp
    ret

    .text
    .align 16
    .p2align 4,,15
#.globl fastpow
#    .type   fastpow,@function
#fastpow:
	.weak	fastpow
	.set	fastpow,__fastpow
	.type	__fastpow,@function
__fastpow:
	sub		$stack_size,%rsp
	movd		%xmm0,%rdx
	mov		%rbx,save_rbx(%rsp)	# save rbx
	movd		%xmm1,%r8
	


# get the x and y values to integer registers.
#	movlpd	QWORD PTR p_x[rsp],xmm0
#	mov		,%rdx  p_x[rsp]		; rdx is ux
#	movd	%xmm0,%rdx
	mov		$0x07fffffffffffffff,%r9
	mov		%r9,%r10
	and		%rdx,%r10			# r10 is ax

	mov		%r10,%rcx
	shr		$52,%rcx					#
	sub		$1023,%ecx
	cvtsi2sd	%ecx,%xmm6			# xexp
# perform special case and error checking on input values
#
# if x == +1, return +1 for all x
	mov		$0x03ff0000000000000,%rax	# one
	cmp		%rax,%rdx
	je		.L__pow_cleanup

# y to gpr
#	movlpd	QWORD PTR p_y[rsp],xmm1
#	mov		%r8  p_y[rsp]		; r8 is uy
#	movd	%xmm1,%r8
	and		%r8,%r9						# r9 is ay
# else if |y| == 0
	test	%r9,%r9						# return 1
	je		.L__rone
# else if x is NAN 
	mov		$0x07ff0000000000000,%rax	# EXPBITS_DP64
	mov		%rax,%rbx
	and		%r10,%rax
	mov		 %rax,p_xexp(%rsp)					# save (ax & EXPBITS_DP64)

	cmp		%rbx,%rax
	jnz		.L__fp1
	mov		$0x0000fffffffffffff,%rax	# MANTBITS_DP64
	and		%r10,%rax
	jnz		.Lnp_special3
# else if y is NAN 
.L__fp1:

	mov		%rbx,%rax		# EXPBITS_DP64
	and		%r9,%rax
	mov		%rax,%rsi		# save (ay & EXPBITS_DP64)
	cmp		%rbx,%rax
	jne		.L__fp2
	mov		$0x0000fffffffffffff,%rax	# MANTBITS_DP64
	and		%r9,%rax
	jnz		.Lnp_special4

# else if y == 1.0
#	.align 16
.L__fp2:
	mov		$0x03ff0000000000000,%rax
	cmp		%r8,%rax
	je		.Lnp_special5

# else if y is infinite or so large that the result would overflow or underflow
	mov		$0x043e0000000000000,%rax
	cmp		%rax,%r9
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
	xor		%rdi,%rdi
	mov		%rsi,%rax			# get (ay & EXPBITS_DP64)
	shr		$52,%rax			#>> EXPSHIFTBITS_DP64
	sub		$1022,%eax		# - EXPBIAS_DP64 + 1   - eax is now the unbiased exponent
	mov		$1,%ebx
	cmp		%ebx,%eax			# if (yexp < 1)
	cmovl		%edi,%ebx
	jl		.Lsave_inty

	mov		$53,%rcx
	cmp		%ecx,%eax			# if (yexp >53)
	jle		.L__fp3
	mov		$2,%ebx
	jmp		.Lsave_inty
.L__fp3:					 # else 1<=yexp<=53
	sub		%eax,%ecx			# build mask for mantissa
	shl		%cl,%rbx
	dec		%rbx				# rbx = mask = (1 << (53 - yexp)) - 1

	mov		%r8,%rax	
	and		%rbx,%rax			# if ((uy & mask) != 0)
	cmovnz		%edi,%ebx			#   inty = 0;
	jnz		.Lsave_inty

	not		%rbx				# else if (((uy & ~mask) >> (53 - yexp)) & 0x0000000000000001)
	mov		%r8,%rax	
	and		%rbx,%rax
	shr		%cl,%rax
	inc		%rdi			
	and		%rdi,%rax
	mov		%edi,%ebx			#  inty = 1
	jnz		.Lsave_inty
	inc		%ebx				# else	inty = 2		


.Lsave_inty:
	mov		 %ebx,p_inty(%rsp)		# save inty

#
# do more x special case checking
#

.Lmore_x:

# if x is infinity (NaN was already ruled out).
	mov		$0x07ff0000000000000,%rax	# EXPBITS_DP64
	cmp		p_xexp(%rsp),%rax	# compare with (ax & EXPBITS_DP64)
	je		.Lnp_special_x1			# jump if x is infinite
# else if ax == 0
	test		%r10,%r10
	je		.Lnp_special_x2

	
	xor		%eax,%eax			# negateres = 0
# if x is negative
	cmp		%r10,%rdx
	je		.L__fp4

	# x is negative
	test		%ebx,%ebx			 # if (inty)
	je		.Lnp_special_x3			 # jump if not an integer


	movd		%r10,%xmm0
	mov		%r10,%rdx
	
	
	cmp		$1,%ebx
	cmove		%ebx,%eax
.L__fp4:
	shl		$63,%rax
	mov		 %rax,p_negateres(%rsp)		# save negateres


#
# finally check if |y| is very small
#
	mov		$0x03c00000000000000,%rax	# if abs(y) < 2^(-63)
	cmp		%rax,%r9
	jl		.Lnp_special_x4			# jump if so




# **** Here starts the main calculations  ****
# The algorithm used is x**y = exp(y*log(x))
#  Extra precision is required in intermediate steps to meet the 1ulp requirement
#
# log(x) calculation
#	.align 16
.Lpow_log:

#  /* log_thresh1 = 9.39412117004394531250e-1 = 0x3fee0faa00000000
#     log_thresh2 = 1.06449508666992187500 = 0x3ff1082c00000000 */
#  if (ux >= log_thresh1 && ux <= log_thresh2)
	mov		$0x03fee0faa00000000,%r8
	cmp		%r8,%rdx
	jb		.Lpl1
	mov		$0x03ff1082c00000000,%r8
	cmp		%r8,%rdx
	jl		.L__near_one
.Lpl1:
#      /* Store the exponent of x in xexp and put
#         f into the range [0.5,1) */

#
# compute the index into the log tables
#
	mov		$0x0000fffffffffffff,%rax	# mantissa bit mask
	and		%rdx,%rax
	mov		%rax,%r8					# save the mantissa

#/* Now  x = 2**xexp  * f,  1/2 <= f < 1. */
	shr		$45,%r8
	mov		%r8,%r9
	shr		$1,%r8d
	add		$0x040,%r8d
	and		$1,%r9d
	add		%r9d,%r8d					#index

# reduce and get u
	cvtsi2sd	%r8d,%xmm7			# convert index to float
	mov		$0x03fe0000000000000,%rdx
	or		%rdx,%rax
	movd	%rax,%xmm2				# f 

	movlpd	.L__real_half(%rip),%xmm5		# .5

	mulsd	.L__real_3f80000000000000(%rip),%xmm7		# f1 = index/128


	lea		.L__np_ln_lead_table(%rip),%r9
	movlpd	 -512(%r9,%r8,8),%xmm0			# z1
	subsd	%xmm7,%xmm2				# f2 = f - f1
	mulsd	%xmm2,%xmm5
	addsd	%xmm5,%xmm7				# denominator
	divsd	%xmm7,%xmm2				# u

# solve for ln(1+u)
	movsd	%xmm2,%xmm7				# u
	mulsd	%xmm2,%xmm2				# u^2
	movlpd	.L__real_cb3(%rip),%xmm3
	mulsd	%xmm2,%xmm3				#Cu2
	movsd	%xmm2,%xmm5
	mulsd	%xmm7,%xmm5				# u^3
	addsd	.L__real_cb2(%rip),%xmm3 		#B+Cu2
	movsd	%xmm2,%xmm4
	mulsd	%xmm5,%xmm4				# u^5
	movlpd	.L__real_log2_lead(%rip),%xmm2

	mulsd	.L__real_cb1(%rip),%xmm5 		#Au3
	mulsd	%xmm3,%xmm4				# u5(B+Cu2)

	addsd	%xmm5,%xmm4				# Au3+u5(B+Cu2)

# recombine

	mulsd	%xmm6,%xmm2				# xexp * log2_lead
	addsd	%xmm2,%xmm0				#r1,A
	mulsd	.L__real_log2_tail(%rip),%xmm6	# xexp * log2_tail
	lea	.L__np_ln_tail_table(%rip),%r9
        addsd   -512(%r9,%r8,8),%xmm4                   #z2    +=q ,C
	addsd	%xmm4,%xmm6				# C

# redistribute the result
	movsd	%xmm7,%xmm2		# B
	addsd	%xmm6,%xmm7		# 0xB = B+C
	subsd	%xmm7,%xmm2		# -0xC = B-Bh
	addsd	%xmm2,%xmm6		# Ct = C-0xC

	movsd	%xmm0,%xmm3		#  
	addsd	%xmm7,%xmm0		# H = A+0xB
	subsd	%xmm0,%xmm3		# -Bhead = A-H
	addsd	%xmm3,%xmm7		# +Btail = 0xB-Bhead

	movsd	%xmm0,%xmm4		# 
	 
	andpd	.L__real_fffffffff8000000(%rip),%xmm0	#Head
	subsd	%xmm0,%xmm4		# Ht = H - Head
	addsd	%xmm4,%xmm7		# tail = Btail +Ht


	addsd	%xmm6,%xmm7		# Tail = tail + ct
# r in .Lrr,%xmm0 in xmm7

	jmp		 .L__ylogx

	.align	16

.L__near_one:
# saves 10 cycles
#      r = x - 1.0;
	movlpd	.L__real_two(%rip),%xmm2
	subsd	.L__real_one(%rip),%xmm0	   # r
#      u          = r / (2.0 + r);
	addsd	%xmm0,%xmm2
	movsd	%xmm0,%xmm7
	divsd	%xmm2,%xmm7		# u
	movlpd	.L__real_ca4(%rip),%xmm4	  #D
	movlpd	.L__real_ca3(%rip),%xmm5	  #C
#      correction = r * u;
	movsd	%xmm0,%xmm6
	mulsd	%xmm7,%xmm6		# correction
#      u          = u + u;
	addsd	%xmm7,%xmm7		#u
	movsd	%xmm7,%xmm2
	mulsd	%xmm2,%xmm2		#v =u^2
#      r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4))) - correction);
	mulsd	%xmm7,%xmm5		# Cu
	movsd	%xmm7,%xmm3
	mulsd	%xmm2,%xmm3		# u^3
	mulsd	.L__real_ca2(%rip),%xmm2	#Bu^2
	mulsd	%xmm3,%xmm4		#Du^3

	addsd	.L__real_ca1(%rip),%xmm2	# +A
	movsd	%xmm3,%xmm7
	mulsd	%xmm7,%xmm7		# u^6
	addsd	%xmm4,%xmm5		#Cu+Du3
	subsd	%xmm6,%xmm0		# -correction			part A

	mulsd	%xmm3,%xmm2		#u3(A+Bu2)				part B
	movsd	%xmm0,%xmm4
	mulsd	%xmm5,%xmm7		#u6(Cu+Du3)				part C

# we now have 3 terms, develop a head and tail for the sum

	movsd	%xmm2,%xmm3		# B
	addsd	%xmm3,%xmm0		# H = A+B
	subsd	%xmm0,%xmm4		# 0xB = A - H
	addsd	%xmm4,%xmm2		# Bt = B-0xB

	movsd	%xmm0,%xmm3		#  split the top term
	andpd	.L__real_fffffffff8000000(%rip),%xmm0	#Head
	subsd	%xmm0,%xmm3		# Ht = H - Head
	addsd	%xmm3,%xmm2		# Tail = Bt +Ht

	addsd	%xmm2,%xmm7		# Tail = tail + C

#  /* end of log(x) calculation*/
# Head in %xmm0, tail  in xmm7
#
#  /* Carefully compute w =  log(x) * y */
#
#	.align 16
.L__ylogx:
#  /* Split y into hy (head) + ty (tail). */

# hx in xmm0,tx in xmm7

	movsd	%xmm1,%xmm2						# copy y
	andpd	.L__real_fffffffff8000000(%rip),%xmm2	# hy


	movsd	%xmm0,%xmm3
	mulsd	%xmm1,%xmm3					# (c)z = x * y
	movsd	%xmm1,%xmm5
	subsd	%xmm2,%xmm5					#ty
# compute cc
	movsd	%xmm0,%xmm4
	mulsd	%xmm2,%xmm4					# hx*hy
	movsd	%xmm0,%xmm6
	mulsd	%xmm5,%xmm6					# hx*ty
	subsd	%xmm3,%xmm4					# -z
	mulsd	%xmm7,%xmm2					# hy*tx

	mulsd	%xmm7,%xmm5					# tx*ty
#	mulsd	%xmm1,%xmm7					; y * tx    

	addsd	%xmm6,%xmm4					# + hx*ty
	addsd	%xmm2,%xmm4					# + hy*tx
	addsd	%xmm5,%xmm4					# cc += tx*ty 


#  w1 = c + cc;
	movsd	%xmm3,%xmm0					#c(or z)
	addsd	%xmm4,%xmm0					#z += cc(or zz)    


	subsd	%xmm0,%xmm3					# -0xcc = c - w1
	addsd	%xmm3,%xmm4					# w2 = cc - 0xcc

	#  w = w1 + w2;
#
#  /* Test for overflow and underflow due to y*log2(x)
#     being too large or small. */
#
	comisd	.L__real_4090040000000000(%rip),%xmm0 # 1025
	ja		.Lnp_special_lx1

	comisd	.L__real_C090C80000000000(%rip),%xmm0 # -1074
	jc		.Lnp_special_lx2

#
#  /* The following code computes r = exp(w) */
#
#        /* Find m, z1 and z2 such that exp(x) = 2**m * (z1 + z2) */
#      splitexp(x, 1.0, thirtytwo_by_log2, log2_by_32_lead,
#	       log2_by_32_tail, &m, &z1, &z2);
#      Step 1. Reduce the argument.
	#    r = x * thirtytwo_by_logbaseof2;
	movlpd	.L__real_thirtytwo_by_log2(%rip),%xmm3
	mulsd	%xmm0,%xmm3 

# get x value to integer register.
	movd	%xmm0,%rdx

#    /* Set n = nearest integer to r */
#	cvtsd2si	%xmm3,%ecx				# convert to integer 
	cvtpd2dq	%xmm3,%xmm5
	lea		.L__two_to_jby32_lead_table(%rip),%rdi
	lea		.L__two_to_jby32_trail_table(%rip),%rsi
	cvtdq2pd	%xmm5,%xmm1
#	cvtsi2sd	%ecx,%xmm1				# and back to float.	


#    r1 = x - n * logbaseof2_by_32_lead;
	movlpd	.L__real_log2_by_32_lead(%rip),%xmm2	
	mulsd	%xmm1,%xmm2			
	movd		%xmm5,%ecx
	subsd	%xmm2,%xmm0		 			# r1 in xmm0, 
#    r2 =   - n * logbaseof2_by_32_trail;
	mulsd	.L__real_log2_by_32_tail(%rip),%xmm1		# r2 in xmm1
	addsd	%xmm1,%xmm4 
#    j = n & 0x0000001f;
	mov		$0x01f,%rax
	mov		$1024,%edx
	movsd	%xmm0,%xmm2
	and		%ecx,%eax
#    f1 = two_to_jby32_lead_table[j];
#    f2 = two_to_jby32_trail_table[j];

	movsd	%xmm0,%xmm6					# copy r1
	addsd	%xmm4,%xmm2					#  r = r1 + r2
	movlpd	 (%rsi,%rax,8),%xmm5				# f2
#    *m = (n - j) / 32;
	sub		%eax,%ecx
	sar		$5,%ecx					#m
	cmp		%edx,%ecx
#	jg		.Lnp_special_lx1
	cmovg	%edx,%ecx
#	movsd	%xmm2,%xmm6	  				# copy r1
#	addsd	%xmm4,%xmm2					#  r = r1 + r2
#	movlpd	.LQWORD,%xmm5 PTR [rsi+rax*8]			# f2
#    q = r1 + (r2 +	
#              r*r*( 5.00000000000000008883e-01 +
#                      r*( 1.66666666665260878863e-01 +
#                      r*( 4.16666666662260795726e-02 +
#                      r*( 8.33336798434219616221e-03 +
#                      r*( 1.38889490863777199667e-03 ))))));
#    q = r + r^2/2 + r^3/6 + r^4/24 + r^5/120 + r^6/720	
	movsd	%xmm2,%xmm1
	movlpd	.L__real_3f56c1728d739765(%rip),%xmm3		# 	1/720
	movlpd	.L__real_3FC5555555548F7C(%rip),%xmm0		# 	1/6
	mulsd	%xmm2,%xmm3					# *x
	mulsd	%xmm2,%xmm0					# *x
	mulsd	%xmm2,%xmm1					# x*x
	movsd	%xmm1,%xmm7
	addsd	.L__real_3F811115B7AA905E(%rip),%xmm3		# 	+ 1/120
	addsd	.L__real_3fe0000000000000(%rip),%xmm0		# 	+ .5
	mulsd	%xmm1,%xmm7					# x^4
	mulsd	%xmm2,%xmm3					# *x
	mulsd	%xmm1,%xmm0					# *x^2
	addsd	.L__real_3FA5555555545D4E(%rip),%xmm3		# 	+ 1/24
	addsd	%xmm4,%xmm0					# 	+ r2
#	addsd	%xmm2,%xmm0					; 	+ x
	mulsd	%xmm7,%xmm3					# *x^4
	addsd	%xmm3,%xmm0					# q = final sum
	addsd	%xmm6,%xmm0					# +r1

#    *z2 = f2 + ((f1 + f2) * q);
	addsd	(%rdi,%rax,8),%xmm5				# f1 + f2
	xor		%eax,%eax
	mulsd	%xmm5,%xmm0
	addsd	%xmm5,%xmm0					#z = z1 + z2 
# end of splitexp				
#        /* Scale (z1 + z2) by 2.0**m */
#	addsd	%xmm4,%xmm0					# z = z1+z2;	done with 4
#          r = scaleDouble_1(z, n);

	add		$1023,%ecx				# add bias
#	js		.L__denormal
	cmovs	%eax,%ecx
	orpd 	p_negateres(%rsp),%xmm0
	shl		$52,%rcx				# build 2^n
	mov		 %rcx,p_temp(%rsp)
	mulsd	p_temp(%rsp),%xmm0
#
#  /* Check for  overflow or underflow */
#
#__final_check:
#	xorpd	%xmm3,%xmm3
#  /* If r overflowed or underflowed we need to deal with errno */
#	movd	%xmm0,%rdx
#	mov		$0x07fffffffffffffff,%rcx
#	and		%rdx,%rcx					; rcx is ax



#  if (r > large)
#	mov		$0x07ff0000000000000,%rax	; EXPBITS_DP64
#	mov		%rax,%rbx
#	and		%rcx,%rax
#	cmp		%rbx,%rax
#	je		.Lnp_special_lx1



# if ax==0
#	test	%rcx,%rcx
#	je		.Lnp_special_lx2



# done with all computation and error checking, now just adjust the sign
#	mov		.LQWORD,%eax PTR p_negateres[rsp]				; save negateres
#	cmp		$0,%eax
#	cmp		QWORD PTR p_negateres[rsp],0
#	jz		__pow_cleanup		; done
#	subpd	%xmm0,%xmm3		; negate the result
#	movsd	%xmm3,%xmm0
#	orpd 	.LQWORD,%xmm0 PTR p_negateres[rsp]

.L__pow_cleanup:

	mov		save_rbx(%rsp),%rbx		# restore rbx
	add		$stack_size,%rsp
	ret

	.align 16
#__denormal:
#	xorpd	%xmm0,%xmm0
#	jmp		.Lnp_special_lx2

.L__rzero:
	xorpd	%xmm0,%xmm0
	jmp		.L__pow_cleanup

	.align 16
.L__rone:
	movlpd	.L__real_one(%rip),%xmm0
	jmp		.L__pow_cleanup

	.align 16
.L__infinity:
	movlpd	.L__real_infinity(%rip),%xmm0
	jmp		.L__pow_cleanup


	.align 16
# y ==NaN
.Lnp_special4:
	movd	%xmm1,%rdx
# x is a NaN
.Lnp_special3:
#NaN
.L__enan:	
	mov		$0x00008000000000000,%rax	# convert to quiet
	or		%rax,%rdx
	movd	%rdx,%xmm0
	jmp		.L__pow_cleanup
#
# here is the code for special cases


# y == 1.0, just return x.
.Lnp_special5:
	jmp		.L__pow_cleanup


	.align 16

#      /* y is infinite or so large that the result would
#         overflow or underflow.

.Lnp_special6:
	cmp		%r9,%r8										# is y positive
	jnz		.Lnps6										# jump if not
	test	%r10,%r10
	jz		.L__rzero

	mov		$0x0bff0000000000000,%rax	# is x== -1
	cmp		%rax,%rdx										
	jz		.L__rone
	mov		$0x03ff0000000000000,%rax	# is |x| < 1.0
	cmp		%rax,%r10										
	jl		.L__rzero
#  abs(x) < 1.0
	jmp		.L__infinity 						 
# y < 0
.Lnps6:
	test	%r10,%r10
	jz		.L__infinity									# if |x| == 0

	mov		$0x03ff0000000000000,%rax	# is |x| < 1.0
	cmp		%rax,%r10										
	jge		.Lnps6_1
#  abs(x) < 1.0
	mov		$0x07ff0000000000000,%rax	# is y == inf
	cmp		%rax,%r9
	jz		.L__infinity			# return inf if so
	jmp		.L__infinity
.Lnps6_1:
	jnz		.Lnpx64				# jump if |x| != 1.0 

	movd	%r10,%xmm0				# return 1
	jmp		.L__pow_cleanup
# |x| > 1.0
.Lnpx64:											 
	xorpd	%xmm0,%xmm0										# return 0
	jmp		.L__pow_cleanup

	.align 16
.Lnp_special_x1:											# x is infinite
	cmp		%r10,%rdx										# is x positive
	jnz		.Lnsx11										# jump if not
	cmp		%r9,%r8										# is y positive
	jz	.Lnps6_2				# just return if so
	xorpd	%xmm0,%xmm0				# else return 0
.Lnps6_2:	jmp		.L__pow_cleanup

.Lnsx11:
	mov		p_inty(%rsp),%eax		# if inty ==1
	cmp		$1,%eax
	jnz		.Lnsx12				# jump if not
	cmp		%r9,%r8				# is ypos
	jz	.Lnsx11_1				# just return if so
	movlpd	.L__real_8000000000000000(%rip),%xmm0	# else return -0
.Lnsx11_1:	jmp		.L__pow_cleanup
.Lnsx12:						# inty <>1
	cmp		%r9,%r8				# is y positive
	xorpd	%xmm0,%xmm0									# else return 0
	jnz		.Lnsx12_1			# just return if so
	movd	%r10,%xmm0				# return -x (|x|)
.Lnsx12_1:	jmp		.L__pow_cleanup

	.align 16
.Lnp_special_x2:					#ax == 0
	cmp		%r10,%rdx			# is x positive
	jnz		.Lnsx21				# jump if not
	cmp		%r9,%r8				# is ypos
	xorpd	%xmm0,%xmm0			
	jz		.L__pow_cleanup			# return x (0) if so
	jmp		.L__infinity
.Lnsx21:
	cmp		%r9,%r8				# is ypos
	jnz		.Lnsx22			
	mov		p_inty(%rsp),%eax		# if inty ==1
	cmp		$1,%eax
	jz		.L__pow_cleanup			# just return if so
	xorpd	%xmm0,%xmm0									# return +0 if -0 raised to + odd integer
.Lnsx21_1:	jmp		.L__pow_cleanup
.Lnsx22:
	mov		p_inty(%rsp),%eax		# if inty ==1
	cmp		$1,%eax
	mov		$0x0fff0000000000000,%rax
	mov		$0x07ff0000000000000,%r8
	cmovz	%rax,%r8
	movd	%r8,%xmm0
	jmp		.L__pow_cleanup

	.align 16
.Lnp_special_x3:
	# x < 0 and y is not an integer
	movlpd	.L__real_nan(%rip),%xmm0		# return a NaN
	jmp		.L__pow_cleanup

.Lnp_special_x4:
	mov		$0x03ff0000000000000,%rax
	xorpd	%xmm5,%xmm5
	movd	%rax,%xmm0
	addsd	%xmm1,%xmm0
	jmp		.L__pow_cleanup

	.align 16
# result will be too large or too small
.Lnp_special_lx2:											# too small
	mov		p_negateres(%rsp),%rax				# get negateres
	xor		%rdx,%rdx
	cmp		$0,%rax
	mov		$0x08000000000000000,%r8
	cmovz	%rdx,%r8
	movd	%r8,%xmm0
	jmp		.L__pow_cleanup
	.align 16
.Lnp_special_lx1:											# too large
	mov		p_negateres(%rsp),%rax				# get negateres
	cmp		$0,%rax
	mov		$0x0fff0000000000000,%rax
	mov		$0x07ff0000000000000,%r8
	cmovnz	%rax,%r8
	movd	%r8,%xmm0
	jmp		.L__pow_cleanup
	.align 16
.Lnp_special_lx3:											# denormal
	mov		p_negateres(%rsp),%rax				# get negateres
	xor		%rdx,%rdx
	cmp		$0,%rax
	mov		$0x08000000000000000,%r8
	cmovnz	%rdx,%r8
	movd	%r8,%xmm1
	orpd	%xmm1,%xmm0
	jmp		.L__pow_cleanup	

	.data
	.align 16

.L__np_ln_lead_table:
	.quad	0x0000000000000000 		# 0.00000000000000000000e+00
	.quad	0x3f8fc0a800000000		# 1.55041813850402832031e-02
	.quad	0x3f9f829800000000		# 3.07716131210327148438e-02
	.quad	0x3fa7745800000000		# 4.58095073699951171875e-02
	.quad	0x3faf0a3000000000		# 6.06245994567871093750e-02
	.quad	0x3fb341d700000000		# 7.52233862876892089844e-02
	.quad	0x3fb6f0d200000000		# 8.96121263504028320312e-02
	.quad	0x3fba926d00000000		# 1.03796780109405517578e-01
	.quad	0x3fbe270700000000		# 1.17783010005950927734e-01
	.quad	0x3fc0d77e00000000		# 1.31576299667358398438e-01
	.quad	0x3fc2955280000000		# 1.45181953907012939453e-01
	.quad	0x3fc44d2b00000000		# 1.58604979515075683594e-01
	.quad	0x3fc5ff3000000000		# 1.71850204467773437500e-01
	.quad	0x3fc7ab8900000000		# 1.84922337532043457031e-01
	.quad	0x3fc9525a80000000		# 1.97825729846954345703e-01
	.quad	0x3fcaf3c900000000		# 2.10564732551574707031e-01
	.quad	0x3fcc8ff780000000		# 2.23143517971038818359e-01
	.quad	0x3fce270700000000		# 2.35566020011901855469e-01
	.quad	0x3fcfb91800000000		# 2.47836112976074218750e-01
	.quad	0x3fd0a324c0000000		# 2.59957492351531982422e-01
	.quad	0x3fd1675c80000000		# 2.71933674812316894531e-01
	.quad	0x3fd22941c0000000		# 2.83768117427825927734e-01
	.quad	0x3fd2e8e280000000		# 2.95464158058166503906e-01
	.quad	0x3fd3a64c40000000		# 3.07025015354156494141e-01
	.quad	0x3fd4618bc0000000		# 3.18453729152679443359e-01
	.quad	0x3fd51aad80000000		# 3.29753279685974121094e-01
	.quad	0x3fd5d1bd80000000		# 3.40926527976989746094e-01
	.quad	0x3fd686c800000000		# 3.51976394653320312500e-01
	.quad	0x3fd739d7c0000000		# 3.62905442714691162109e-01
	.quad	0x3fd7eaf800000000		# 3.73716354370117187500e-01
	.quad	0x3fd89a3380000000		# 3.84411692619323730469e-01
	.quad	0x3fd9479400000000		# 3.94993782043457031250e-01
	.quad	0x3fd9f323c0000000		# 4.05465066432952880859e-01
	.quad	0x3fda9cec80000000		# 4.15827870368957519531e-01
	.quad	0x3fdb44f740000000		# 4.26084339618682861328e-01
	.quad	0x3fdbeb4d80000000		# 4.36236739158630371094e-01
	.quad	0x3fdc8ff7c0000000		# 4.46287095546722412109e-01
	.quad	0x3fdd32fe40000000		# 4.56237375736236572266e-01
	.quad	0x3fddd46a00000000		# 4.66089725494384765625e-01
	.quad	0x3fde744240000000		# 4.75845873355865478516e-01
	.quad	0x3fdf128f40000000		# 4.85507786273956298828e-01
	.quad	0x3fdfaf5880000000		# 4.95077252388000488281e-01
	.quad	0x3fe02552a0000000		# 5.04556000232696533203e-01
	.quad	0x3fe0723e40000000		# 5.13945698738098144531e-01
	.quad	0x3fe0be72e0000000		# 5.23248136043548583984e-01
	.quad	0x3fe109f380000000		# 5.32464742660522460938e-01
	.quad	0x3fe154c3c0000000		# 5.41597247123718261719e-01
	.quad	0x3fe19ee6a0000000		# 5.50647079944610595703e-01
	.quad	0x3fe1e85f40000000		# 5.59615731239318847656e-01
	.quad	0x3fe23130c0000000		# 5.68504691123962402344e-01
	.quad	0x3fe2795e00000000		# 5.77315330505371093750e-01
	.quad	0x3fe2c0e9e0000000		# 5.86049020290374755859e-01
	.quad	0x3fe307d720000000		# 5.94707071781158447266e-01
	.quad	0x3fe34e2880000000		# 6.03290796279907226562e-01
	.quad	0x3fe393e0c0000000		# 6.11801505088806152344e-01
	.quad	0x3fe3d90260000000		# 6.20240390300750732422e-01
	.quad	0x3fe41d8fe0000000		# 6.28608644008636474609e-01
	.quad	0x3fe4618bc0000000		# 6.36907458305358886719e-01
	.quad	0x3fe4a4f840000000		# 6.45137906074523925781e-01
	.quad	0x3fe4e7d800000000		# 6.53301239013671875000e-01
	.quad	0x3fe52a2d20000000		# 6.61398470401763916016e-01
	.quad	0x3fe56bf9c0000000		# 6.69430613517761230469e-01
	.quad	0x3fe5ad4040000000		# 6.77398800849914550781e-01
	.quad	0x3fe5ee02a0000000		# 6.85303986072540283203e-01
	.quad	0x3fe62e42e0000000		# 6.93147122859954833984e-01
	.quad 0					# for alignment

	.align 16

.L__np_ln_tail_table:
	.quad	0x00000000000000000 # 0	; 0.00000000000000000000e+00
	.quad	0x03e361f807c79f3db		# 5.15092497094772879206e-09
	.quad	0x03e6873c1980267c8		# 4.55457209735272790188e-08
	.quad	0x03e5ec65b9f88c69e		# 2.86612990859791781788e-08
	.quad	0x03e58022c54cc2f99		# 2.23596477332056055352e-08
	.quad	0x03e62c37a3a125330		# 3.49498983167142274770e-08
	.quad	0x03e615cad69737c93		# 3.23392843005887000414e-08
	.quad	0x03e4d256ab1b285e9		# 1.35722380472479366661e-08
	.quad	0x03e5b8abcb97a7aa2		# 2.56504325268044191098e-08
	.quad	0x03e6f34239659a5dc		# 5.81213608741512136843e-08
	.quad	0x03e6e07fd48d30177		# 5.59374849578288093334e-08
	.quad	0x03e6b32df4799f4f6		# 5.06615629004996189970e-08
	.quad	0x03e6c29e4f4f21cf8		# 5.24588857848400955725e-08
	.quad	0x03e1086c848df1b59		# 9.61968535632653505972e-10
	.quad	0x03e4cf456b4764130		# 1.34829655346594463137e-08
	.quad	0x03e63a02ffcb63398		# 3.65557749306383026498e-08
	.quad	0x03e61e6a6886b0976		# 3.33431709374069198903e-08
	.quad	0x03e6b8abcb97a7aa2		# 5.13008650536088382197e-08
	.quad	0x03e6b578f8aa35552		# 5.09285070380306053751e-08
	.quad	0x03e6139c871afb9fc		# 3.20853940845502057341e-08
	.quad	0x03e65d5d30701ce64		# 4.06713248643004200446e-08
	.quad	0x03e6de7bcb2d12142		# 5.57028186706125221168e-08
	.quad	0x03e6d708e984e1664		# 5.48356693724804282546e-08
	.quad	0x03e556945e9c72f36		# 1.99407553679345001938e-08
	.quad	0x03e20e2f613e85bda		# 1.96585517245087232086e-09
	.quad	0x03e3cb7e0b42724f6		# 6.68649386072067321503e-09
	.quad	0x03e6fac04e52846c7		# 5.89936034642113390002e-08
	.quad	0x03e5e9b14aec442be		# 2.85038578721554472484e-08
	.quad	0x03e6b5de8034e7126		# 5.09746772910284482606e-08
	.quad	0x03e6dc157e1b259d3		# 5.54234668933210171467e-08
	.quad	0x03e3b05096ad69c62		# 6.29100830926604004874e-09
	.quad	0x03e5c2116faba4cdd		# 2.61974119468563937716e-08
	.quad	0x03e665fcc25f95b47		# 4.16752115011186398935e-08
	.quad	0x03e5a9a08498d4850		# 2.47747534460820790327e-08
	.quad	0x03e6de647b1465f77		# 5.56922172017964209793e-08
	.quad	0x03e5da71b7bf7861d		# 2.76162876992552906035e-08
	.quad	0x03e3e6a6886b09760		# 7.08169709942321478061e-09
	.quad	0x03e6f0075eab0ef64		# 5.77453510221151779025e-08
	.quad	0x03e33071282fb989b		# 4.43021445893361960146e-09
	.quad	0x03e60eb43c3f1bed2		# 3.15140984357495864573e-08
	.quad	0x03e5faf06ecb35c84		# 2.95077445089736670973e-08
	.quad	0x03e4ef1e63db35f68		# 1.44098510263167149349e-08
	.quad	0x03e469743fb1a71a5		# 1.05196987538551827693e-08
	.quad	0x03e6c1cdf404e5796		# 5.23641361722697546261e-08
	.quad	0x03e4094aa0ada625e		# 7.72099925253243069458e-09
	.quad	0x03e6e2d4c96fde3ec		# 5.62089493829364197156e-08
	.quad	0x03e62f4d5e9a98f34		# 3.53090261098577946927e-08
	.quad	0x03e6467c96ecc5cbe		# 3.80080516835568242269e-08
	.quad	0x03e6e7040d03dec5a		# 5.66961038386146408282e-08
	.quad	0x03e67bebf4282de36		# 4.42287063097349852717e-08
	.quad	0x03e6289b11aeb783f		# 3.45294525105681104660e-08
	.quad	0x03e5a891d1772f538		# 2.47132034530447431509e-08
	.quad	0x03e634f10be1fb591		# 3.59655343422487209774e-08
	.quad	0x03e6d9ce1d316eb93		# 5.51581770357780862071e-08
	.quad	0x03e63562a19a9c442		# 3.60171867511861372793e-08
	.quad	0x03e54e2adf548084c		# 1.94511067964296180547e-08
	.quad	0x03e508ce55cc8c97a		# 1.54137376631349347838e-08
	.quad	0x03e30e2f613e85bda		# 3.93171034490174464173e-09
	.quad	0x03e6db03ebb0227bf		# 5.52990607758839766440e-08
	.quad	0x03e61b75bb09cb098		# 3.29990737637586136511e-08
	.quad	0x03e496f16abb9df22		# 1.18436010922446096216e-08
	.quad	0x03e65b3f399411c62		# 4.04248680368301346709e-08
	.quad	0x03e586b3e59f65355		# 2.27418915900284316293e-08
	.quad	0x03e52482ceae1ac12		# 1.70263791333409206020e-08
	.quad	0x03e6efa39ef35793c		# 5.76999904754328540596e-08
	.quad 0					# for alignment

	.align 16

.L__two_to_jby32_lead_table:
	.quad	0x03ff0000000000000 # 1 
	.quad	0x03ff059b0d0000000		# 1.0219
	.quad	0x03ff0b55860000000		# 1.04427
	.quad	0x03ff11301d0000000		# 1.06714
	.quad	0x03ff172b830000000		# 1.09051
	.quad	0x03ff1d48730000000		# 1.11439
	.quad	0x03ff2387a60000000		# 1.13879
	.quad	0x03ff29e9df0000000		# 1.16372
	.quad	0x03ff306fe00000000		# 1.18921
	.quad	0x03ff371a730000000		# 1.21525
	.quad	0x03ff3dea640000000		# 1.24186
	.quad	0x03ff44e0860000000		# 1.26905
	.quad	0x03ff4bfdad0000000		# 1.29684
	.quad	0x03ff5342b50000000		# 1.32524
	.quad	0x03ff5ab07d0000000		# 1.35426
	.quad	0x03ff6247eb0000000		# 1.38391
	.quad	0x03ff6a09e60000000		# 1.41421
	.quad	0x03ff71f75e0000000		# 1.44518
	.quad	0x03ff7a11470000000		# 1.47683
	.quad	0x03ff8258990000000		# 1.50916
	.quad	0x03ff8ace540000000		# 1.54221
	.quad	0x03ff93737b0000000		# 1.57598
	.quad	0x03ff9c49180000000		# 1.61049
	.quad	0x03ffa5503b0000000		# 1.64576
	.quad	0x03ffae89f90000000		# 1.68179
	.quad	0x03ffb7f76f0000000		# 1.71862
	.quad	0x03ffc199bd0000000		# 1.75625
	.quad	0x03ffcb720d0000000		# 1.79471
	.quad	0x03ffd5818d0000000		# 1.83401
	.quad	0x03ffdfc9730000000		# 1.87417
	.quad	0x03ffea4afa0000000		# 1.91521
	.quad	0x03fff507650000000		# 1.95714
	.quad 0					# for alignment
.L__two_to_jby32_trail_table:
	.quad	0x00000000000000000 # 0 
	.quad	0x03e48ac2ba1d73e2a		# 1.1489e-008
	.quad	0x03e69f3121ec53172		# 4.83347e-008
	.quad	0x03df25b50a4ebbf1b		# 2.67125e-010
	.quad	0x03e68faa2f5b9bef9		# 4.65271e-008
	.quad	0x03e368b9aa7805b80		# 5.24924e-009
	.quad	0x03e6ceac470cd83f6		# 5.38622e-008
	.quad	0x03e547f7b84b09745		# 1.90902e-008
	.quad	0x03e64636e2a5bd1ab		# 3.79764e-008
	.quad	0x03e5ceaa72a9c5154		# 2.69307e-008
	.quad	0x03e682468446b6824		# 4.49684e-008
	.quad	0x03e18624b40c4dbd0		# 1.41933e-009
	.quad	0x03e54d8a89c750e5e		# 1.94147e-008
	.quad	0x03e5a753e077c2a0f		# 2.46409e-008
	.quad	0x03e6a90a852b19260		# 4.94813e-008
	.quad	0x03e0d2ac258f87d03		# 8.48872e-010
	.quad	0x03e59fcef32422cbf		# 2.42032e-008
	.quad	0x03e61d8bee7ba46e2		# 3.3242e-008
	.quad	0x03e4f580c36bea881		# 1.45957e-008
	.quad	0x03e62999c25159f11		# 3.46453e-008
	.quad	0x03e415506dadd3e2a		# 8.0709e-009
	.quad	0x03e29b8bc9e8a0388		# 2.99439e-009
	.quad	0x03e451f8480e3e236		# 9.83622e-009
	.quad	0x03e41f12ae45a1224		# 8.35492e-009
	.quad	0x03e62b5a75abd0e6a		# 3.48493e-008
	.quad	0x03e47daf237553d84		# 1.11085e-008
	.quad	0x03e6b0aa538444196		# 5.03689e-008
	.quad	0x03e69df20d22a0798		# 4.81896e-008
	.quad	0x03e69f7490e4bb40b		# 4.83654e-008
	.quad	0x03e4bdcdaf5cb4656		# 1.29746e-008
	.quad	0x03e452486cc2c7b9d		# 9.84533e-009
	.quad	0x03e66dc8a80ce9f09		# 4.25828e-008
	.quad 0					# for alignment

	.align 16
.L__real_3ff0000000000000: 	.quad 0x03ff0000000000000	# 1.0
				.quad 0				# for alignment
.L__real_fffffffff8000000: 	.quad 0x0fffffffff8000000	# mask for stripping head and tail
				.quad 0				# for alignment

.L__real_03d0000000000000: 	.quad 0x003d0000000000000	# 2.5653355008114851558350183e-290
				.quad 0
.L__real_mant:			.quad 0x0000FFFFFFFFFFFFF	# mantissa bits
				.quad 0
.L__real_3f80000000000000:	.quad 0x03f80000000000000	# /* 0.0078125 = 1/128 */
				.quad 0
.L__real_notsign:		.quad 0x07ffFFFFFFFFFFFFF	# ^sign bit
				.quad 0x07ffFFFFFFFFFFFFF
.L__real_threshold:		.quad 0x03F9EB85000000000	# .03
				.quad 0

.L__mask_1023:			.quad 0x000000000000003ff	# 
				.quad 0x000000000000003ff
.L__mask_040:			.quad 0x00000000000000040	# 
				.quad 0x00000000000000040
.L__mask_001:			.quad 0x00000000000000001	# 
				.quad 0x00000000000000001

.L__real_8000000000000000:	.quad 0x08000000000000000	# -0
				.quad 0

.L__real_4090040000000000:	.quad 0x04090040000000000	# 1025
				.quad 0
.L__real_C090C80000000000:	.quad 0x0C090C80000000000	# -1074
				.quad 0
.L__real_4060000000000000:	.quad 0x04060000000000000	# 128
				.quad 0
.L__real_4040000000000000:	.quad 0x04040000000000000	# 32
				.quad 0
.L__real_3FA0000000000000:	.quad 0x03FA0000000000000	# 1/32
				.quad 0
.L__real_3fe0000000000000:	.quad 0x03fe0000000000000	# 1/2
				.quad 0
.L__real_3fd5555555555555:	.quad 0x03fd5555555555555	# 1/3
				.quad 0
.L__real_3fd0000000000000:	.quad 0x03fd0000000000000	# 1/4
				.quad 0
.L__real_3fc999999999999a:	.quad 0x03fc999999999999a	# 1/5
				.quad 0
.L__real_3fc5555555555555:	.quad 0x03fc5555555555555	# 1/6
				.quad 0
.L__real_3fc2492492492492:	.quad 0x03fc2492492492492	# 1/7
				.quad 0
.L__real_3fc0000000000000:	.quad 0x03fc0000000000000	# 1/8
				.quad 0
.L__real_3f56c1728d739765:	.quad 0x03f56c1728d739765	# 1.38889490863777199667e-03 used in splitexp
				.quad 0
.L__real_3F811115B7AA905E:	.quad 0x03F811115B7AA905E	# 8.33336798434219616221e-03 used in splitexp
				.quad 0
.L__real_3FA5555555545D4E:	.quad 0x03FA5555555545D4E	# 4.16666666662260795726e-02 used in splitexp
				.quad 0
.L__real_3FC5555555548F7C:	.quad 0x03FC5555555548F7C	# 1.66666666665260878863e-01 used in splitexp
				.quad 0
.L__real_3feffc6f012164ad:	.quad 0x03feffc6f012164ad	# 9.99564649780173692e-1 used in small w
				.quad 0
.L__real_3f437d98a3a06a02:	.quad 0x03f437d98a3a06a02	# 5.9480622371960190616 used in small w
				.quad 0
.L__real_3e9208a80efa0f9c:	.quad 0x03e9208a80efa0f9c	# 2.68724774856111190e-7 used in small w
				.quad 0
.L__real_3f885dd726cbfa41:	.quad 0x03f885dd726cbfa41	# 1.189773642681502232e-2 used in small w
				.quad 0
.L__real_3f985eaf935d972d:	.quad 0x03f985eaf935d972d	# 2.37986978239838493 used in small w
				.quad 0
.L__real_3fbb6a194bd1cdeb:	.quad 0x03fbb6a194bd1cdeb	# 1.070876894098586184e-1 used in small w
				.quad 0
.L__real_3ef0e88e25daf0f3:	.quad 0x03ef0e88e25daf0f3	# 1.61251249355268050e-5 used in small w
				.quad 0
.L__real_3fdffc2b5ee8cd41:	.quad 0x03fdffc2b5ee8cd41	# 4.99766199765151309e-1 used in small w
				.quad 0
.L__real_3fe62e42fefa39ef:	.quad 0x03fe62e42fefa39ef	# log(2)
				.quad 0
.L__real_3fe62e42f8000000:	.quad 0x03fe62e42f8000000	# log(2) lead
				.quad 0
.L__real_3e4be8e7bcd5e4f1:	.quad 0x03e4be8e7bcd5e4f1	# log(2) tail
				.quad 0
.L__real_3FF71547652B82FE:	.quad 0x03FF71547652B82FE	# 1/log(2) lead
				.quad 0
.L__real_3FF7154760000000:	.quad 0x03FF7154760000000	# 1/log(2) lead	head
				.quad 0
.L__real_3e54ae0bf8000000:	.quad 0x03e54ae0bf8000000	# 1/log(2) lead tail
				.quad 0
.L__real_3C7777D0FFDA0D20:	.quad 0x03C7777D0FFDA0D20	# 1/log(2) tail
				.quad 0


.L__real_one:			.quad 0x03ff0000000000000	# 1.0
				.quad 0					# for alignment
.L__real_two:			.quad 0x04000000000000000	# 1.0
				.quad 0
.L__real_ninf:			.quad 0x0fff0000000000000	# -inf
				.quad 0
.L__real_nan:			.quad 0x07ff8000000000000	# NaN
				.quad 0

.L__real_ca1:			.quad 0x03fb55555555554e6	# 8.33333333333317923934e-02
				.quad 0					# for alignment
.L__real_ca2:			.quad 0x03f89999999bac6d4	# 1.25000000037717509602e-02
				.quad 0					# for alignment
.L__real_ca3:			.quad 0x03f62492307f1519f	# 2.23213998791944806202e-03
				.quad 0					# for alignment
.L__real_ca4:			.quad 0x03f3c8034c85dfff0	# 4.34887777707614552256e-04
				.quad 0					# for alignment

#.L__real_cb1:			.quad 0x03fb5555555555557	; 8.33333333333333593622e-02
#.L__real_cb2:			.quad 0x03f89999999865ede	; 1.24999999978138668903e-02
#.L__real_cb3:			.quad 0x03f6249423bd94741	; 2.23219810758559851206e-03

# from Maple:
#Digits :=40;
#minimax(2*ln(1+v/2), v=0..1/256, 7,1 ,'maxerror');
.L__real_cb1:			.quad 0x03fb555555555552c	# 0.08333333333333275459088388736767942281572 from maple
				.quad 0					# for alignment
.L__real_cb2:			.quad 0x03F8999998EAB53DB	# 0.01249999968187325554473232707489405493533
				.quad 0					# for alignment
.L__real_cb3:			.quad 0x03F622946AE40F6EE	# 0.002216947601509803143606018496083165773938
				.quad 0					# for alignment

.L__real_log2_lead:  		.quad 0x03fe62e42e0000000	# log2_lead	  6.93147122859954833984e-01
				.quad 0					# for alignment
.L__real_log2_tail: 		.quad 0x03e6efa39ef35793c	# log2_tail	  5.76999904754328540596e-08
				.quad 0					# for alignment

.L__real_half:			.quad 0x03fe0000000000000	# 1/2
				.quad 0					# for alignment

.L__real_infinity:		.quad 0x07ff0000000000000	# 
				.quad 0					# for alignment
.L__real_thirtytwo_by_log2: .quad 0x040471547652b82fe	# thirtytwo_by_log2
				.quad 0
.L__real_log2_by_32_lead:  .quad 0x03f962e42fe000000	# log2_by_32_lead
				.quad 0
.L__real_log2_by_32_tail:  .quad 0x0Bdcf473de6af278e	# -log2_by_32_tail
				.quad 0
#.L__fake_w:			.quad 0x04085DFE30EE7550A
.L__fake_w:			.quad 0x04085DFE30EE75509
				.quad 0					# for alignment
#.L__fake_w:			.quad 0x0BFD0425004045B95
							

