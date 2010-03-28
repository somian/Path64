;/* ============================================================
;Copyright (c) 2005 Advanced Micro Devices, Inc.
;
;All rights reserved.
;
;Redistribution and  use in source and binary  forms, with or
;without  modification,  are   permitted  provided  that  the
;following conditions are met:
;
;
;+ Redistributions  of source  code  must  retain  the  above
;  copyright  notice,   this  list  of   conditions  and  the
;  following disclaimer.
;
;+ Redistributions  in binary  form must reproduce  the above
;  copyright  notice,   this  list  of   conditions  and  the
;  following  disclaimer in  the  documentation and/or  other
;  materials provided with the distribution.
;
;+ Neither the  name of Advanced Micro Devices,  Inc. nor the
;  names  of  its contributors  may  be  used  to endorse  or
;  promote  products  derived   from  this  software  without
;  specific prior written permission.
;
;THIS  SOFTWARE  IS PROVIDED  BY  THE  COPYRIGHT HOLDERS  AND
;CONTRIBUTORS "AS IS" AND  ANY EXPRESS OR IMPLIED WARRANTIES,
;INCLUDING,  BUT NOT  LIMITED TO,  THE IMPLIED  WARRANTIES OF
;MERCHANTABILITY  AND FITNESS  FOR A  PARTICULAR  PURPOSE ARE
;DISCLAIMED.  IN  NO  EVENT  SHALL  ADVANCED  MICRO  DEVICES,
;INC.  OR CONTRIBUTORS  BE LIABLE  FOR ANY  DIRECT, INDIRECT,
;INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES
;(INCLUDING,  BUT NOT LIMITED  TO, PROCUREMENT  OF SUBSTITUTE
;GOODS  OR  SERVICES;  LOSS  OF  USE, DATA,  OR  PROFITS;  OR
;BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON  ANY THEORY OF
;LIABILITY,  WHETHER IN CONTRACT,  STRICT LIABILITY,  OR TORT
;(INCLUDING NEGLIGENCE  OR OTHERWISE) ARISING IN  ANY WAY OUT
;OF  THE  USE  OF  THIS  SOFTWARE, EVEN  IF  ADVISED  OF  THE
;POSSIBILITY OF SUCH DAMAGE.
;
;In  the  redistribution and use of this software, each party
;shall  at  all times comply with all applicable governmental
;laws,  statutes, ordinances, rules, regulations, orders, and
;other   requirements,   including  without  limitation  such
;governmental   requirements   applicable   to  environmental
;protection,  health,  safety, wages, hours, equal employment
;opportunity,  nondiscrimination,  working conditions, import
;or export control, and transportation.  Without limiting the
;foregoing,  each  party  shall  adhere  to  the  U.S. Export
;Administration   Regulations  (EAR),  currently   found   at
;15 C.F.R. Sections 730  through  744,  and,  unless properly
;authorized  by  the  U.S. Government,  shall not (1) export,
;re-export  or  release  restricted  technology, software, or
;source  code  to  a  national of a country in Country Groups
;D:1 or E:1,  or  (2) export to Country Groups D:1 or E:1 the
;direct  product  of  such  technology  or  software, if such
;foreign  produced  direct  product  is  subject  to national
;security controls as identified on the Commerce Control List
;(currently  found  in  Supplement 1  to Section 774 of EAR).
;These  export  requirements  shall survive any expiration or
;termination of this agreement.
;============================================================ */

## Copyright 2005 PathScale, Inc.  All Rights Reserved.

##
## vrs4powf.s
##
## A vector implementation of the powf libm function.
##
## Prototype:
##
##     __m128 __vrs4_powf(__m128 x,__m128 y);
##
##   Computes x raised to the y power.  Returns proper C99 values.
##   Uses new tuned fastlog/fastexp.
##
##

#include "picdefs.S"

## define local variable storage offsets
.equ	p_temp,0x00		# xmmword
.equ	p_negateres,0x10		# qword

.equ	p_xexp,0x20		# qword

.equ	p_ux,0x030		# storage for X
.equ	p_uy,0x040		# storage for Y

.equ	p_ax,0x050		# absolute x
.equ	p_sx,0x060		# sign of x's

.equ	p_ay,0x070		# absolute y
.equ	p_yexp,0x080		# unbiased exponent of y

.equ	p_inty,0x090		# integer y indicators
.equ    p_rbx_save,0x0A0        # save rbx register to restore
.equ	stack_size,0x0A8	# allocate ??h more than
				# we need to avoid bank conflicts



    .text
    .align 16
    .p2align 4,,15
.globl __vrs4_powf
    .type   __vrs4_powf,@function
__vrs4_powf:
	sub		$stack_size,%rsp

	movaps	  %xmm0,p_ux(%rsp)		# save x
	movaps	  %xmm1,p_uy(%rsp)		# save y

	movq    %rbx,p_rbx_save(%rsp)         # save %rbx

	movaps	%xmm0,%xmm2
	andps	.L__mask_nsign(%rip),%xmm0		# get abs x
	andps	.L__mask_sign(%rip),%xmm2		# mask for the sign bits
	movaps	  %xmm0,p_ax(%rsp)		# save them
	movaps	  %xmm2,p_sx(%rsp)		# save them
## convert all four x's to double	
	cvtps2pd   p_ax(%rsp),%xmm0
	cvtps2pd   p_ax+8(%rsp),%xmm1
##
## classify y
## vector 32 bit integer method	 25 cycles to here
##  /* See whether y is an integer.
##     inty = 0 means not an integer.
##     inty = 1 means odd integer.
##     inty = 2 means even integer.
##  */
	movdqa  p_uy(%rsp),%xmm4
	pxor	%xmm3,%xmm3
	pand	.L__mask_nsign(%rip),%xmm4		# get abs y in integer format
	movdqa    %xmm4,p_ay(%rsp)			# save it

## see if the number is less than 1.0
	psrld	$23,%xmm4			#>> EXPSHIFTBITS_SP32

	psubd	.L__mask_127(%rip),%xmm4			# yexp, unbiased exponent
	movdqa    %xmm4,p_yexp(%rsp)		# save it
	paddd	.L__mask_1(%rip),%xmm4			# yexp+1
	pcmpgtd	%xmm3,%xmm4		# 0 if exp less than 126 (2^0) (y < 1.0), else FFs
## xmm4 is ffs if abs(y) >=1.0, else 0

## see if the mantissa has fractional bits
##build mask for mantissa
	movdqa  .L__mask_23(%rip),%xmm2
	psubd	p_yexp(%rsp),%xmm2		# 24-yexp
	pmaxsw	%xmm3,%xmm2							# no shift counts less than 0
	movdqa    %xmm2,p_temp(%rsp)		# save the shift counts
## create mask for all four values
## SSE can't individual shifts so have to do 0xeac one seperately
	mov		p_temp(%rsp),%rcx
	mov		$1,%rbx
	shl		%cl,%ebx			#1 << (24 - yexp)
	shr		$32,%rcx
	mov		$1,%eax
	shl		%cl,%eax			#1 << (24 - yexp)
	shl		$32,%rax
	add		%rax,%rbx
	mov		%rbx,p_temp(%rsp)
	mov		p_temp+8(%rsp),%rcx
	mov		$1,%rbx
	shl		%cl,%ebx			#1 << (24 - yexp)
	shr		$32,%rcx
	mov		$1,%eax
	shl		%cl,%eax			#1 << (24 - yexp)
	shl		$32,%rax
	add		%rbx,%rax
	mov		%rax,p_temp+8(%rsp)
	movdqa  p_temp(%rsp),%xmm5
	psubd	.L__mask_1(%rip),%xmm5	#= mask = (1 << (24 - yexp)) - 1
	
## now use the mask to see if there are any fractional bits
	movdqa  p_uy(%rsp),%xmm2 # get uy
	pand	%xmm5,%xmm2		# uy & mask
	pcmpeqd	%xmm3,%xmm2		# 0 if not zero (y has fractional mantissa bits), else FFs
	pand	%xmm4,%xmm2		# either 0s or ff
## xmm2 now accounts for y< 1.0 or y>=1.0 and y has fractional mantissa bits,
## it has the value 0 if we know it's non-integer or ff if integer.

## now see if it's even or odd.

## if yexp > 24, then it has to be even
	movdqa  .L__mask_24(%rip),%xmm4
	psubd	p_yexp(%rsp),%xmm4		# 24-yexp
	paddd	.L__mask_1(%rip),%xmm5	# mask+1 = least significant integer bit
	pcmpgtd	%xmm3,%xmm4		 # if 0, then must be even, else ff's

 	pand	%xmm4,%xmm5		# set the integer bit mask to zero if yexp>24
 	paddd	.L__mask_2(%rip),%xmm4
 	por		.L__mask_2(%rip),%xmm4
 	pand	%xmm2,%xmm4		 # result can be 0, 2, or 3

## now for integer numbers, see if odd or even
	pand	.L__mask_mant(%rip),%xmm5	# mask out exponent bits
	movdqa	.L__float_one(%rip),%xmm2
	pand    p_uy(%rsp),%xmm5 #  & uy -> even or odd
	pcmpeqd	p_ay(%rsp),%xmm2	# is ay equal to 1, ff's if so, then it's odd
	pand	.L__mask_nsign(%rip),%xmm2 # strip the sign bit so the gt comparison works.
	por		%xmm2,%xmm5
	pcmpgtd	%xmm3,%xmm5		 # if odd then ff's, else 0's for even
	paddd	.L__mask_2(%rip),%xmm5 # gives us 2 for even, 1 for odd
	pand	%xmm5,%xmm4

	movdqa		  %xmm4,p_inty(%rsp)		# save inty
##
## do more x special case checking
##
	movdqa	%xmm4,%xmm5
	pcmpeqd	%xmm3,%xmm5						# is not an integer? ff's if so
	pand	.L__mask_NaN(%rip),%xmm5		# these values will be NaNs, if x<0
	movdqa	%xmm4,%xmm2
	pcmpeqd	.L__mask_1(%rip),%xmm2		# is it odd? ff's if so
	pand	.L__mask_sign(%rip),%xmm2	# these values will get their sign bit set
	por		%xmm2,%xmm5

	pcmpeqd	p_sx(%rsp),%xmm3		# if the signs are set
	pandn	%xmm5,%xmm3						# then negateres gets the values as shown below
	movdqa	  %xmm3,p_negateres(%rsp)	# save negateres

##  /* p_negateres now means the following.
##  ** 7FC00000 means x<0, y not an integer, return NaN.
##  ** 80000000 means x<0, y is odd integer, so set the sign bit.
##  ** 0 means even integer, and/or x>=0.
##  */


## **** Here starts the main calculations  ****
## The algorithm used is x**y = exp(y*log(x))
##  Extra precision is required in intermediate steps to meet the 1ulp requirement
##
## log(x) calculation
	call		EXTERN(__vrd4_log)
						# get the double precision log value 
											# for all four x's
## y* logx
## convert all four y's to double	
	lea	p_uy(%rsp),%rdx		# get pointer to y
	cvtps2pd   (%rdx),%xmm2
	cvtps2pd   8(%rdx),%xmm3

##  /* just multiply by y */
	mulpd	%xmm2,%xmm0
	mulpd	%xmm3,%xmm1

##  /* The following code computes r = exp(w) */
	call		EXTERN(__vrd4_exp)
						# get the double exp value 
											# for all four y*log(x)'s
##
## convert all four results to double	
	cvtpd2ps	%xmm0,%xmm0
	cvtpd2ps	%xmm1,%xmm1
	movlhps		%xmm1,%xmm0

## perform special case and error checking on input values

## special case checking is done first in the scalar version since
## it allows for early fast returns.  But for vectors, we consider them
## to be rare, so early returns are not necessary.  So we first compute
## the x**y values, and then check for special cases.

## we do some of the checking in reverse order of the scalar version.
	lea	p_uy(%rsp),%rdx		# get pointer to y
## apply the negate result flags
	orps	p_negateres(%rsp),%xmm0	# get negateres

## if y is infinite or so large that the result would overflow or underflow
	movdqa	p_ay(%rsp),%xmm4
	cmpps	$5,.L__mask_ly(%rip),%xmm4	# y not less than large value, ffs if so.
	movmskps %xmm4,%edx
	test	$0x0f,%edx
	jnz		.Ly_large
.Lrnsx3:

## if x is infinite 
	movdqa	p_ax(%rsp),%xmm4
	cmpps	$0,.L__mask_inf(%rip),%xmm4	# equal to infinity, ffs if so.
	movmskps %xmm4,%edx
	test	$0x0f,%edx
	jnz		.Lx_infinite
.Lrnsx1:
## if x is zero
	xorps	%xmm4,%xmm4 
	cmpps	$0,p_ax(%rsp),%xmm4	# equal to zero, ffs if so.
	movmskps %xmm4,%edx
	test	$0x0f,%edx
	jnz		.Lx_zero
.Lrnsx2:
## if y is NAN 
	lea		p_uy(%rsp),%rdx		# get pointer to y
	movdqa	(%rdx),%xmm4			# get y
	cmpps	$4,%xmm4,%xmm4						# a compare not equal  of y to itself should
											# be false, unless y is a NaN. ff's if NaN.
	movmskps %xmm4,%ecx
	test	$0x0f,%ecx
	jnz		.Ly_NaN
.Lrnsx4:
## if x is NAN 
	lea		p_ux(%rsp),%rdx		# get pointer to x
	movdqa	(%rdx),%xmm4			# get x
	cmpps	$4,%xmm4,%xmm4						# a compare not equal  of x to itself should
											# be false, unless x is a NaN. ff's if NaN.
	movmskps %xmm4,%ecx
	test	$0x0f,%ecx
	jnz		.Lx_NaN
.Lrnsx5:

## if |y| == 0	then return 1
	movdqa	.L__float_one(%rip),%xmm3	# one
	xorps	%xmm2,%xmm2
	cmpps	$4,p_ay(%rsp),%xmm2	# not equal to 0.0?, ffs if not equal.
	andps	%xmm2,%xmm0						# keep the others
	andnps	%xmm3,%xmm2						# mask for ones
	orps	%xmm2,%xmm0
## if x == +1, return +1 for all x
	lea		p_ux(%rsp),%rdx		# get pointer to x
	movdqa	%xmm3,%xmm2
	cmpps	$4,(%rdx),%xmm2		# not equal to +1.0?, ffs if not equal.
	andps	%xmm2,%xmm0						# keep the others
	andnps	%xmm3,%xmm2						# mask for ones
	orps	%xmm2,%xmm0

.L__powf_cleanup2:
        
	movq          p_rbx_save(%rsp), %rbx         # restore %rbx
	add		$stack_size,%rsp
	ret

	.align 16
##      /* y is a NaN. */
.Ly_NaN:
	lea	p_uy(%rsp),%rdx		# get pointer to y
	movdqa	(%rdx),%xmm4			# get y
	movdqa	%xmm4,%xmm3
	movdqa	%xmm4,%xmm5
	movdqa	.L__mask_sigbit(%rip),%xmm2	# get the signalling bits
	cmpps	$0,%xmm4,%xmm4						# a compare equal  of y to itself should
											# be true, unless y is a NaN. 0's if NaN.
	cmpps	$4,%xmm3,%xmm3						# compare not equal, ff's if NaN.
	andps	%xmm4,%xmm0						# keep the other results
	andps	%xmm3,%xmm2						# get just the right signalling bits
	andps	%xmm5,%xmm3						# mask for the NaNs
	orps	%xmm2,%xmm3	# convert to QNaNs
	orps	%xmm3,%xmm0						# combine
	jmp	   	.Lrnsx4

##      /* y is a NaN. */
.Lx_NaN:
	lea		p_ux(%rsp),%rcx		# get pointer to x
	movdqa	(%rcx),%xmm4			# get x
	movdqa	%xmm4,%xmm3
	movdqa	%xmm4,%xmm5
	movdqa	.L__mask_sigbit(%rip),%xmm2	# get the signalling bits
	cmpps	$0,%xmm4,%xmm4						# a compare equal  of x to itself should
											# be true, unless x is a NaN. 0's if NaN.
	cmpps	$4,%xmm3,%xmm3						# compare not equal, ff's if NaN.
	andps	%xmm4,%xmm0						# keep the other results
	andps	%xmm3,%xmm2						# get just the right signalling bits
	andps	%xmm5,%xmm3						# mask for the NaNs
	orps	%xmm2,%xmm3	# convert to QNaNs
	orps	%xmm3,%xmm0						# combine
	jmp	   	.Lrnsx5

##      /* y is infinite or so large that the result would */
##      /* overflow or underflow. */
.Ly_large:
	movdqa	  %xmm0,p_temp(%rsp)

	test	$1,%edx
	jz		.Lylrga
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		(%rcx),%eax
	mov		(%rbx),%ebx
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special6					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp(%rsp)
.Lylrga:
	test	$2,%edx
	jz		.Lylrgb
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		4(%rcx),%eax
	mov		4(%rbx),%ebx
	mov		p_inty+4(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special6					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+4(%rsp)
.Lylrgb:
	test	$4,%edx
	jz		.Lylrgc
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		8(%rcx),%eax
	mov		8(%rbx),%ebx
	mov		p_inty+8(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special6					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+8(%rsp)
.Lylrgc:
	test	$8,%edx
	jz		.Lylrgd
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		12(%rcx),%eax
	mov		12(%rbx),%ebx
	mov		p_inty+12(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special6					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+12(%rsp)
.Lylrgd:
	movdqa	p_temp(%rsp),%xmm0
	jmp 	.Lrnsx3

## a subroutine to treat an individual x,y pair when y is large or infinity
## assumes x in .Ly,%eax in ebx.
## returns result in eax
.Lnp_special6:
## handle |x|==1 cases first
	mov		$0x07FFFFFFF,%r8d
	and		%eax,%r8d
	cmp		$0x03f800000,%r8d					  # jump if |x| !=1 
	jnz		.Lnps6
	mov		$0x03f800000,%eax					  # return 1 for all |x|==1
	jmp 	.Lnpx64

## cases where  |x| !=1
.Lnps6:
	mov		$0x07f800000,%ecx
	xor		%eax,%eax							  # assume 0 return
	test	$0x080000000,%ebx
	jnz		.Lnps62							  # jump if y negative
## y = +inf
	cmp		$0x03f800000,%r8d
	cmovg	%ecx,%eax							  # return inf if |x| < 1
	jmp 	.Lnpx64
.Lnps62:
## y = -inf
	cmp		$0x03f800000,%r8d
	cmovl	%ecx,%eax							  # return inf if |x| < 1
	jmp 	.Lnpx64

.Lnpx64:													 
	ret

## handle cases where x is +/- infinity.  edx is the mask
	.align 16
.Lx_infinite:
	movdqa	  %xmm0,p_temp(%rsp)

	test	$1,%edx
	jz		.Lxinfa
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		(%rcx),%eax
	mov		(%rbx),%ebx
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x1					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp(%rsp)
.Lxinfa:
	test	$2,%edx
	jz		.Lxinfb
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		4(%rcx),%eax
	mov		4(%rbx),%ebx
	mov		p_inty+4(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x1					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+4(%rsp)
.Lxinfb:
	test	$4,%edx
	jz		.Lxinfc
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		8(%rcx),%eax
	mov		8(%rbx),%ebx
	mov		p_inty+8(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x1					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+8(%rsp)
.Lxinfc:
	test	$8,%edx
	jz		.Lxinfd
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		12(%rcx),%eax
	mov		12(%rbx),%ebx
	mov		p_inty+12(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x1					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+12(%rsp)
.Lxinfd:
	movdqa	p_temp(%rsp),%xmm0
	jmp 	.Lrnsx1

## a subroutine to treat an individual x,y pair when x is +/-infinity
## assumes x in .Ly,%eax in ebx, inty in ecx.
## returns result in eax
.Lnp_special_x1:											# x is infinite
	test	$0x080000000,%eax								# is x positive
	jnz		.Lnsx11										# jump if not
	test	$0x080000000,%ebx								# is y positive
	jz		.Lnsx13											# just return if so
	xor		%eax,%eax										# else return 0
	jmp 	.Lnsx13

.Lnsx11:
	cmp		$1,%ecx										# if inty ==1
	jnz		.Lnsx12										# jump if not
	test	$0x080000000,%ebx								# is y positive
	jz		.Lnsx13											# just return if so
	mov		$0x080000000,%eax								# else return -0
	jmp 	.Lnsx13
.Lnsx12:													# inty <>1
	and		$0x07FFFFFFF,%eax								# return -x (|x|)  if y<0
	test	$0x080000000,%ebx								# is y positive
	jz		.Lnsx13											# 
	xor		%eax,%eax										# return 0  if y >=0
.Lnsx13:
	ret


## handle cases where x is +/- zero.  edx is the mask of x,y pairs with |x|=0
	.align 16
.Lx_zero:
	movdqa	  %xmm0,p_temp(%rsp)

	test	$1,%edx
	jz		.Lxzera
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		(%rcx),%eax
	mov		(%rbx),%ebx
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x2					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp(%rsp)
.Lxzera:
	test	$2,%edx
	jz		.Lxzerb
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		4(%rcx),%eax
	mov		4(%rbx),%ebx
	mov		p_inty+4(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x2					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+4(%rsp)
.Lxzerb:
	test	$4,%edx
	jz		.Lxzerc
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		8(%rcx),%eax
	mov		8(%rbx),%ebx
	mov		p_inty+8(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x2					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+8(%rsp)
.Lxzerc:
	test	$8,%edx
	jz		.Lxzerd
	lea		p_ux(%rsp),%rcx		# get pointer to x
	lea		p_uy(%rsp),%rbx		# get pointer to y
	mov		12(%rcx),%eax
	mov		12(%rbx),%ebx
	mov		p_inty+12(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x2					# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+12(%rsp)
.Lxzerd:
	movdqa	p_temp(%rsp),%xmm0
	jmp 	.Lrnsx2

## a subroutine to treat an individual x,y pair when x is +/-0
## assumes x in .Ly,%eax in ebx, inty in ecx.
## returns result in eax
	.align 16
.Lnp_special_x2:
	cmp		$1,%ecx										# if inty ==1
	jz		.Lnsx21										# jump if so
## handle cases of x=+/-0, y not integer
	xor		%eax,%eax
	mov		$0x07f800000,%ecx
	test	$0x080000000,%ebx								# is ypos
	cmovnz	%ecx,%eax
	jmp		.Lnsx23
## y is an integer
.Lnsx21:
	xor		%r8d,%r8d
	mov		$0x07f800000,%ecx
	test	$0x080000000,%ebx								# is ypos
	cmovnz	%ecx,%r8d										# set to infinity if not
	and		$0x080000000,%eax								# pickup the sign of x
	or		%r8d,%eax										# and include it in the result
.Lnsx23:
	ret


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



__two_to_jby32_table:
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

.L__real_3f80000000000000:	.quad 0x03f80000000000000	# /* 0.0078125 = 1/128 */
						.quad 0


.L__real_3FA5555555545D4E:	.quad 0x03FA5555555545D4E	# 4.16666666662260795726e-02 used in splitexp
						.quad 0
.L__real_3FC5555555548F7C:	.quad 0x03FC5555555548F7C	# 1.66666666665260878863e-01 used in splitexp
						.quad 0


.L__real_one:				.quad 0x03ff0000000000000	# 1.0
						.quad 0					# for alignment
.L__real_two:				.quad 0x04000000000000000	# 1.0
						.quad 0

.L__real_ca1:				.quad 0x03fb55555555554e6	# 8.33333333333317923934e-02
						.quad 0					# for alignment
.L__real_ca2:				.quad 0x03f89999999bac6d4	# 1.25000000037717509602e-02
						.quad 0					# for alignment

## from Maple:
##Digits :=40;
##minimax(2*ln(1+v/2), v=0..1/256, 7,1 ,'maxerror');
.L__real_cb1:				.quad 0x03fb555555555552c	# 0.08333333333333275459088388736767942281572 from maple
						.quad 0					# for alignment
.L__real_cb2:				.quad 0x03F8999998EAB53DB	# 0.01249999968187325554473232707489405493533
						.quad 0					# for alignment

.L__real_log2:		  		.quad 0x03FE62E42FEFA39EF	# log2_lead	  6.9314718055994530941723e-01
						.quad 0					# for alignment

.L__real_half:				.quad 0x03fe0000000000000	# 1/2
						.quad 0					# for alignment

.L__real_infinity:			.quad 0x07ff0000000000000	# 
						.quad 0					# for alignment
.L__real_thirtytwo_by_log2: .quad 0x040471547652b82fe	# thirtytwo_by_log2
						.quad 0
.L__real_log2_by_32:       .quad 0x03F962E42FEFA39EF	# log2_by_32
						.quad 0

.L__mask_sign:				.quad 0x08000000080000000	# a sign bit mask
						.quad 0x08000000080000000

.L__mask_nsign:			.quad 0x07FFFFFFF7FFFFFFF	# a not sign bit mask
						.quad 0x07FFFFFFF7FFFFFFF

## used by inty
.L__mask_127:				.quad 0x00000007F0000007F	# EXPBIAS_SP32
						.quad 0x00000007F0000007F

.L__mask_mant:				.quad 0x0007FFFFF007FFFFF	# mantissa bit mask
						.quad 0x0007FFFFF007FFFFF

.L__mask_1:				.quad 0x00000000100000001	# 1
						.quad 0x00000000100000001

.L__mask_2:				.quad 0x00000000200000002	# 2
						.quad 0x00000000200000002

.L__mask_24:				.quad 0x00000001800000018	# 24
						.quad 0x00000001800000018

.L__mask_23:				.quad 0x00000001700000017	# 23
						.quad 0x00000001700000017

## used by special case checking

.L__float_one:				.quad 0x03f8000003f800000	# one
						.quad 0x03f8000003f800000

.L__mask_inf:				.quad 0x07f8000007F800000	# inifinity
						.quad 0x07f8000007F800000

.L__mask_ninf:				.quad 0x0ff800000fF800000	# -inifinity
						.quad 0x0ff800000fF800000

.L__mask_NaN:				.quad 0x07fC000007FC00000	# NaN
						.quad 0x07fC000007FC00000

.L__mask_sigbit:			.quad 0x00040000000400000	# QNaN bit
						.quad 0x00040000000400000

.L__mask_impbit:			.quad 0x00080000000800000	# implicit bit
						.quad 0x00080000000800000

.L__mask_ly:				.quad 0x04f0000004f000000	# large y
						.quad 0x04f0000004f000000


