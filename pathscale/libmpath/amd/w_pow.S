##
##
##============================================================
##Copyright (c) 2004 Advanced Micro Devices, Inc.
##
##All rights reserved.
##
##Redistribution and  use in source and binary  forms, with or
##without  modification,  are   permitted  provided  that  the
##following conditions are met:
##
##+ Redistributions  of source  code  must  retain  the  above
##  copyright  notice,   this  list  of   conditions  and  the
##  following disclaimer.
##
##+ Redistributions  in binary  form must reproduce  the above
##  copyright  notice,   this  list  of   conditions  and  the
##  following  disclaimer in  the  documentation and/or  other
##  materials provided with the distribution.
##
##+ Neither the  name of Advanced Micro Devices,  Inc. nor the
##  names  of  its contributors  may  be  used  to endorse  or
##  promote  products  derived   from  this  software  without
##  specific prior written permission.
##
##THIS  SOFTWARE  IS PROVIDED  BY  THE  COPYRIGHT HOLDERS  AND
##CONTRIBUTORS "AS IS" AND  ANY EXPRESS OR IMPLIED WARRANTIES,
##INCLUDING,  BUT NOT  LIMITED TO,  THE IMPLIED  WARRANTIES OF
##MERCHANTABILITY  AND FITNESS  FOR A  PARTICULAR  PURPOSE ARE
##DISCLAIMED.  IN  NO  EVENT  SHALL  ADVANCED  MICRO  DEVICES,
##INC.  OR CONTRIBUTORS  BE LIABLE  FOR ANY  DIRECT, INDIRECT,
##INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES
##(INCLUDING,  BUT NOT LIMITED  TO, PROCUREMENT  OF SUBSTITUTE
##GOODS  OR  SERVICES;  LOSS  OF  USE, DATA,  OR  PROFITS;  OR
##BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON  ANY THEORY OF
##LIABILITY,  WHETHER IN CONTRACT,  STRICT LIABILITY,  OR TORT
##(INCLUDING NEGLIGENCE  OR OTHERWISE) ARISING IN  ANY WAY OUT
##OF  THE  USE  OF  THIS  SOFTWARE, EVEN  IF  ADVISED  OF  THE
##POSSIBILITY OF SUCH DAMAGE.
##
##It is  licensee's responsibility  to comply with  any export
##regulations applicable in licensee's jurisdiction.
##============================================================

## pow.s
##
## An implementation of the pow libm function.
##
## Prototype:
##
##     double pow(double x, double y);
##
##   Computes x raised to the y power.  Performs all error checking.
##   Based on the NAG C implementation.
##
##

#include "picdefs.S"

        .weak   pow
        .set    pow,__pow
        .section        .rodata.cst8,"aM",@progbits,8
        .align 16

.__real_3ff0000000000000: .quad 0x3ff0000000000000	# 1.0
						.quad 0					# for alignment
.__real_fffffffff8000000: .quad 0xfffffffff8000000	# mask for stripping head and tail
						.quad 0					# for alignment

.__real_03d0000000000000: .quad 0x03d0000000000000	# 2.5653355008114851558350183e-290
						.quad 0
.__real_3f80000000000000:	.quad 0x3f80000000000000	# /* 0.0078125 = 1/128 */
						.quad 0
.__real_8000000000000000:	.quad 0x8000000000000000	# -0
						.quad 0

.__real_4090040000000000:	.quad 0x4090040000000000	# 1025
						.quad 0
.__real_C090C80000000000:	.quad 0xC090C80000000000	# -1074
						.quad 0
.__real_4060000000000000:	.quad 0x4060000000000000	# 128
						.quad 0
.__real_4040000000000000:	.quad 0x4040000000000000	# 32
						.quad 0
.__real_3FA0000000000000:	.quad 0x3FA0000000000000	# 1/32
						.quad 0
.__real_3fe0000000000000:	.quad 0x3fe0000000000000	# 1/2
						.quad 0
.__real_3fd5555555555555:	.quad 0x3fd5555555555555	# 1/3
						.quad 0
.__real_3fd0000000000000:	.quad 0x3fd0000000000000	# 1/4
						.quad 0
.__real_3fc999999999999a:	.quad 0x3fc999999999999a	# 1/5
						.quad 0
.__real_3fc5555555555555:	.quad 0x3fc5555555555555	# 1/6
						.quad 0
.__real_3fc2492492492492:	.quad 0x3fc2492492492492	# 1/7
						.quad 0
.__real_3fc0000000000000:	.quad 0x3fc0000000000000	# 1/8
						.quad 0
.__real_3f56c1728d739765:	.quad 0x3f56c1728d739765	# 1.38889490863777199667e-03 used in splitexp
						.quad 0
.__real_3F811115B7AA905E:	.quad 0x3F811115B7AA905E	# 8.33336798434219616221e-03 used in splitexp
						.quad 0
.__real_3FA5555555545D4E:	.quad 0x3FA5555555545D4E	# 4.16666666662260795726e-02 used in splitexp
						.quad 0
.__real_3FC5555555548F7C:	.quad 0x3FC5555555548F7C	# 1.66666666665260878863e-01 used in splitexp
						.quad 0
.__real_3feffc6f012164ad:	.quad 0x3feffc6f012164ad	# 9.99564649780173692e-1 used in small w
						.quad 0
.__real_3f437d98a3a06a02:	.quad 0x3f437d98a3a06a02	# 5.9480622371960190616 used in small w
						.quad 0
.__real_3e9208a80efa0f9c:	.quad 0x3e9208a80efa0f9c	# 2.68724774856111190e-7 used in small w
						.quad 0
.__real_3f885dd726cbfa41:	.quad 0x3f885dd726cbfa41	# 1.189773642681502232e-2 used in small w
						.quad 0
.__real_3f985eaf935d972d:	.quad 0x3f985eaf935d972d	# 2.37986978239838493 used in small w
						.quad 0
.__real_3fbb6a194bd1cdeb:	.quad 0x3fbb6a194bd1cdeb	# 1.070876894098586184e-1 used in small w
						.quad 0
.__real_3ef0e88e25daf0f3:	.quad 0x3ef0e88e25daf0f3	# 1.61251249355268050e-5 used in small w
						.quad 0
.__real_3fdffc2b5ee8cd41:	.quad 0x3fdffc2b5ee8cd41	# 4.99766199765151309e-1 used in small w
						.quad 0
.__real_3fe62e42fefa39ef:	.quad 0x3fe62e42fefa39ef	# log(2)
						.quad 0
.__real_3fe62e42f8000000:	.quad 0x3fe62e42f8000000	# log(2) lead
						.quad 0
.__real_3e4be8e7bcd5e4f1:	.quad 0x3e4be8e7bcd5e4f1	# log(2) tail
						.quad 0
.__real_3FF71547652B82FE:	.quad 0x3FF71547652B82FE	# 1/log(2) lead
						.quad 0
.__real_3FF7154760000000:	.quad 0x3FF7154760000000	# 1/log(2) lead	head
						.quad 0
.__real_3e54ae0bf8000000:	.quad 0x3e54ae0bf8000000	# 1/log(2) lead tail
						.quad 0
.__real_3C7777D0FFDA0D20:	.quad 0x3C7777D0FFDA0D20	# 1/log(2) tail
						.quad 0

## define local variable storage offsets
.equ		p_bits, 		8		# temporary for get/put bits operation
.equ		p_inty,			0x18		# dword
.equ		p_expadjust,		0x1c		# dword
.equ		p_xexp,     		0x20		# dword
.equ		p_negateres,		0x24		# dword
.equ		p_x,        		0x38		# xmmword
.equ		p_y,        		0x48		# xmmword
.equ		p_temp,     		0x58		# xmmword
.equ		p_rbx,      		0x68		# qword
.equ		r,			0x70		# qword
.equ		restore_ctrl_wrd,	0x78		# word
.equ		ctrl_wrd,		0x7A		# word


	.extern __pow_error

        .text
        .align 2
        .p2align 4,,15
.globl __pow
        .type   __pow,@function
__pow:

	sub		$0x80,%rsp
	mov		%rbx,p_rbx(%rsp)	# save rbx


##;IFDEF ALLOW_AMD64_X87
	#;get x87 control word
		fnstcw restore_ctrl_wrd(%rsp)
	#;done getting x87 control word
##;ENDIF ALLOW_AMD64_X87

## get the x and y values to integer registers.
	movsd	 %xmm0,p_x(%rsp)
	mov	 p_x(%rsp),%rdx		# rdx is ux


##;IFDEF ALLOW_AMD64_X87
	#;set x87 control word
		movzx restore_ctrl_wrd(%rsp), %ebx
##;ENDIF ALLOW_AMD64_X87


	movsd	 %xmm1,p_y(%rsp)
	mov	 p_y(%rsp),%r8		# r8 is uy

	mov		$0x7fffffffffffffff,%rax
	mov		%rax,%r10
	and		%rdx,%r10		# r10 is ax
	mov		%rax,%r9
	and		%r8,%r9			# r9 is ay


##;IFDEF ALLOW_AMD64_X87
	#;set x87 control word
	and   $0xfffff0ff, %ebx
	or    $0x00000200, %ebx
	mov   %bx, ctrl_wrd(%rsp)
##;ENDIF ALLOW_AMD64_X87

##
## perform special case and error checking on input values
##

##; if x == 1
	mov		$0x3ff0000000000000,%rax


##;IFDEF ALLOW_AMD64_X87
	#;set x87 control word
		fldcw ctrl_wrd(%rsp)
	#;done setting x87 control word
##;ENDIF ALLOW_AMD64_X87


	cmp		%rdx,%rax
	je		np_special1
##	align 16

##; else if y == 0
	test	%r9,%r9
	je		np_special2

##; else if x is NAN 
	mov		$0x7ff0000000000000,%rax	# EXPBITS_DP64
	mov		%rax,%rbx
	and		%r10,%rax
	mov		%rax,%r11					# save (ax & EXPBITS_DP64)
	cmp		%rbx,%rax
	jne		.L1
	mov		$0x000fffffffffffff,%rax	# MANTBITS_DP64
	and		%r10,%rax
	jnz		np_special3

##; else if y is NAN 
.L1:
	mov		%rbx,%rax		# EXPBITS_DP64
	and		%r9,%rax
	mov		%rax,%rsi		# save (ay & EXPBITS_DP64)
	cmp		%rbx,%rax
	jne		.L2
	mov		$0x000fffffffffffff,%rax	# MANTBITS_DP64
	and		%r9,%rax
	jnz		np_special4

##; else if y == 1.0
##	align 16
.L2:
	mov		$0x3ff0000000000000,%rax
	cmp		%r8,%rax
	je		np_special5

##; else if y is infinite or so large that the result would overflow or underflow
	mov		$0x43e0000000000000,%rax
	cmp		%rax,%r9
	ja		np_special6
	jmp		class_y

##
## here is the code for special cases
##	align 16

## x == 1
np_special1: 
      /* x = +1.0. Return +1.0 for all y, even NaN,
         raising invalid only if y is a signalling NaN */

	addsd	%xmm0,%xmm1				# raise exception if SNaN
	jmp		pow_cleanup

## y == 0
np_special2:
	movlpd	.__real_3ff0000000000000(%rip),%xmm1
	addsd		%xmm1,%xmm0			# raise exception if SNaN
	movsd		%xmm1,%xmm0 			# return a 1.0
	jmp		pow_cleanup

## x is a NaN
np_special3:
    /* x is NaN. Return NaN, with invalid exception if it's
       a signalling NaN. */

	movsd		%xmm0,%xmm1 			# compute x + x to... 
	addsd		%xmm1,%xmm0			# ...raise exception if SNaN 
	jmp		pow_cleanup
														
## y is a NaN
np_special4:
   /* y is NaN. Return NaN, with invalid exception if y
      is a signalling NaN. */
	movsd		%xmm1,%xmm0 			# compute y + y to... 
	addsd		%xmm1,%xmm0			# ...raise exception if SNaN 
	jmp		pow_cleanup



## y == 1.0, just return x.
np_special5:
	jmp		pow_cleanup

##      /* y is infinite or so large that the result would
##         overflow or underflow. Flags should be raised
##         unless y is an exact infinity. */

np_special6:
	cmp		%r9,%r8										# is y positive
	jnz		nps6										# jump if not
	test	%r10,%r10
	jnz		.L9
npx61:
	xorpd	%xmm0,%xmm0			#            /* abs(x) = 0.0. */
	jmp		pow_cleanup
.L9:
	mov		$0x3ff0000000000000,%rax						# is |x| < 1.0
	cmp		%rax,%r10
	jge		.L10
##  abs(x) < 1.0
	mov		$0x7ff0000000000000,%rax						# is y == inf
	cmp		%rax,%r9
	jz		npx61										# return 0 if so
	mov		$8,%rdi				#else return 0 with error
	call	EXTERN(__pow_error)
	jmp		pow_cleanup
.L10:	
	jnz		npx62				# jump if |x| != 1.0 
	movlpd	 .__real_3ff0000000000000(%rip),%xmm0		# return a 1.0
	jmp		pow_cleanup
npx62:
	mov		$0x7ff0000000000000,%rax						# is y == inf
	cmp		%rax,%r9
	jz		npx63				# return inf if so
	mov		$10,%rdi			#else return 0 with error
	call	EXTERN(__pow_error)
	jmp		pow_cleanup
npx63:
	movd	%r9,%xmm0										# return infinity
	jmp		pow_cleanup

## y < 0
nps6:
	test	%r10,%r10
	jnz		.L12				# if |x| == 0
	mov		$11,%rdi			# return inf with error
	call	EXTERN(__pow_error)
	jmp		pow_cleanup
.L12:
	mov		$0x3ff0000000000000,%rax						# is |x| < 1.0
	cmp		%rax,%r10										
	jge		.L13
##  abs(x) < 1.0
	mov		$0x7ff0000000000000,%rax						# is y == inf
	cmp		%rax,%r9
	jz		npx63				# return inf if so
	mov		$12,%rdi			#else return 0 with error
	call	EXTERN(__pow_error)
	jmp		pow_cleanup
.L13:	
	jnz		npx64				# jump if |x| != 1.0 
## |x| == 1.0
	movd	%r10,%xmm0				# return 1
	jmp		pow_cleanup
## |x| > 1.0
npx64:						
	mov		$0x7ff0000000000000,%rax						# is y == inf
	cmp		%rax,%r9
	jz		.L15
	mov		$14,%rdi			#else return ind with error
	call	EXTERN(__pow_error)
	jmp		pow_cleanup
.L15:
	xorpd	%xmm0,%xmm0										# return 0
	jmp		pow_cleanup

## more x special case handlers 

np_special_x1:						# x is infinite

	cmp		%r10,%rdx			# is x positive
	jnz		nsx11				# jump if not
	cmp		%r9,%r8				# is y positive
	jz		.L16				# just return if so
	xorpd	%xmm0,%xmm0				# else return 0
.L16:	jmp		pow_cleanup

nsx11:							# x is -infinity 
	mov		p_inty(%rsp),%eax		# if inty ==1
	cmp		$1,%eax
	jnz		nsx12				# jump if not
	cmp		%r9,%r8				# is ypos
	jz		.L17				# just return if so
	movlpd	.__real_8000000000000000(%rip),%xmm0		# else return -0
.L17:	jmp		pow_cleanup
nsx12:							# inty <>1
	cmp		%r9,%r8				# is y positive
	xorpd	%xmm0,%xmm0				# else return 0
	jnz		.L18				# just return if so
	movd	%r10,%xmm0				# return -x (|x|)
.L18:	jmp		pow_cleanup

np_special_x2:						#ax == 0
	cmp		%r10,%rdx			# is x positive
	jnz		nsx21				# jump if not
	cmp		%r9,%r8				# is ypos
	jz		pow_cleanup			# return x (0) if so
	xorpd	%xmm0,%xmm0				# y is negative; return +infinity with div-by-zero
	mov		$15,%rdi
	call	EXTERN(__pow_error)
	jmp		pow_cleanup
nsx21:
	cmp		%r9,%r8				# is ypos
	jnz		nsx22				# 
	mov		p_inty(%rsp),%eax		# if inty ==1
	cmp		$1,%eax
	jz		pow_cleanup			# just return if so
	xorpd	%xmm0,%xmm0				# return +0 if -0 raised to + odd integer
	jmp		pow_cleanup
nsx22:
	mov		p_inty(%rsp),%eax		# if inty ==1
	cmp		$1,%eax
	mov		$16,%eax
	mov		$17,%rdi
	cmovz	%eax,%edi
	call	EXTERN(__pow_error)
	jmp		pow_cleanup

np_special_x3:
	mov		$18,%rdi			# x < 0 and y is not an integer
	call	EXTERN(__pow_error)
	jmp		pow_cleanup

np_special_x4:
	movlpd	.__real_3ff0000000000000(%rip),%xmm0
	addsd	%xmm1,%xmm0				 # return 1.0 + y
	jmp		pow_cleanup
	
##
## classify y
##
##  /* See whether y is an integer.
##     inty = 0 means not an integer.
##     inty = 1 means odd integer.
##     inty = 2 means even integer.
##  */
        .p2align 4,,15
class_y:
## get yexp
	xorpd	%xmm6,%xmm6
	xorpd	%xmm2,%xmm2
	xor		%rdi,%rdi
	mov		%rsi,%rax			# get (ay & EXPBITS_DP64)
	shr		$52,%rax			#>> EXPSHIFTBITS_DP64
	sub		$1022,%eax		# - EXPBIAS_DP64 + 1   - eax is now the unbiased exponent
	mov		$1,%rbx
	cmp		%ebx,%eax			# if (yexp < 1)
	cmovl	%edi,%ebx
	jl		save_inty

	mov		$53,%rcx
	cmp		%ecx,%eax			# if (yexp >53)
	jle		.L30
	mov		$2,%ebx
	jmp		save_inty
.L30:							# else 1<=yexp<=53
	sub		%eax,%ecx			# build mask for mantissa
	shl		%cl,%rbx
	dec		%rbx				# rbx = mask = (1 << (53 - yexp)) - 1

	mov		%r8,%rax	
	and		%rbx,%rax			# if ((uy & mask) != 0)
	cmovnz	%edi,%ebx			#   inty = 0;
	jnz		save_inty

	not		%rbx				# else if (((uy & ~mask) >> (53 - yexp)) & 0x0000000000000001)
	mov		%r8,%rax	
	and		%rbx,%rax
	shr		%cl,%rax
	inc		%rdi			
	and		%rdi,%rax
	mov		%edi,%ebx			#  inty = 1
	jnz		save_inty
	inc		%ebx				# else	inty = 2		

save_inty:
	mov		 %ebx,p_inty(%rsp)		# save inty
	xorpd	%xmm8,%xmm8
	
##
## do more x special case checking
##

more_x:

##; if x is infinity (NaN was already ruled out).
	mov		$0x7ff0000000000000,%rax	# EXPBITS_DP64
	cmp		%r11,%rax					# compare with (ax & EXPBITS_DP64)
	je		np_special_x1			# jump if x is infinite
##	align 16

##; else if ax == 0
	test	%r10,%r10
	je		np_special_x2

	xor		%r11,%r11					# negateres = 0
##; if x is negative
	cmp		%r10,%rdx
	je		.L31

	# x is negative
	test	%ebx,%ebx			 # if (inty)
	je	np_special_x3			 # jump if not an integer
	mov	%r10,%rdx
	movd	%rdx,%xmm0
	
	
##;IFDEF ALLOW_AMD64_X87
	#;x = abs(x)
	movsd	 %xmm0, p_x(%rsp)	
##;ENDIF	
	
	cmp	$1,%ebx
	cmove	%ebx,%r11d
.L31:
	mov		 %r11d,p_negateres(%rsp)	# save negateres
##
## finally check if |y| is very small
##
	mov		$0x3c00000000000000,%rax	# if abs(y) < 2^(-63)
	cmp		%rax,%r9
	jl		np_special_x4			# jump if so


##;IFDEF ALLOW_AMD64_X87
##; main algorithm for pow in x87
	fldl        p_y(%rsp)
	fldl        p_x(%rsp)
	fyl2x
	fld         %st(0)
	frndint 
	fxch        %st(1)
	fsub        %st(1),%st(0)
	f2xm1  
	fld1   
	faddp       %st(0),%st(1)
	fscale 
	fstp        %st(1)
	fstpl        r(%rsp)

##;restore original x87 control word
	fldcw       restore_ctrl_wrd(%rsp)

##; Move result from memory to xmm3
	movlpd 	    r(%rsp), %xmm0 	


##;ENDIF


##
##  /* Check for  overflow or underflow */
##
final_check:
##  /* If r overflowed or underflowed we need to deal with errno */
	mov		r(%rsp),%rdx			#ux
fc_1:
	mov		$0x7fffffffffffffff,%rcx
	and		%rdx,%rcx			# rcx is ax
##; if (r > large)
	mov		$0x7ff0000000000000,%rax	# EXPBITS_DP64
	mov		%rax,%rbx
	and		%rcx,%rax
	cmp		%rbx,%rax
	je		np_special_lx1
##; if ax==0
	test		%rcx,%rcx
	je		np_special_lx2
## done with all computation and error checking, now just adjust the sign
adjust_sign:
	mov		p_negateres(%rsp),%eax		# save negateres
	cmp		$0,%eax
	jz		pow_cleanup			# done
	xorpd		%xmm1,%xmm1			# negate the result
	subpd		%xmm0,%xmm1
	movsd		%xmm1,%xmm0
	jmp		pow_cleanup			# done
	.p2align 4,,15



## result will be too large or too small
np_special_lx2:								# too small
	mov		p_negateres(%rsp),%eax				# get negateres
	cmp		$0,%eax
	mov		$26,%eax
	mov		$25,%rdi
	cmovz	%eax,%edi
	call	EXTERN(__pow_error)
	jmp		pow_cleanup
np_special_lx1:								# too large
	mov		p_negateres(%rsp),%eax				# get negateres
	cmp		$0,%eax
	mov		$24,%eax
	mov		$23,%rdi
	cmovz	%eax,%edi
	call	EXTERN(__pow_error)
	jmp		pow_cleanup


pow_cleanup:
	mov		p_rbx(%rsp),%rbx		# restore rbx
	add		$0x80,%rsp
	ret

