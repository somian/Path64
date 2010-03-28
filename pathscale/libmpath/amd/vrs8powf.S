## Copyright (c) 2005 PathScale, Inc.  All rights reserved.
## Unpublished -- rights reserved under the copyright laws of the United
## States. USE OF A COPYRIGHT NOTICE DOES NOT IMPLY PUBLICATION OR
## DISCLOSURE. THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
## SECRETS OF PATHSCALE, INC. USE, DISCLOSURE, OR REPRODUCTION IS
## PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF PATHSCALE,
## INC.
## 
## U.S. Government Restricted Rights:
## The Software is a "commercial item," as that term is defined at 48
## C.F.R. 2.101 (OCT 1995), consisting of "commercial computer software"
## and "commercial computer software documentation," as such terms are used
## in 48 C.F.R. 12.212 (SEPT 1995).  Consistent with 48 C.F.R. 12.212 and
## 48 C.F.R. 227-7202-1 through 227-7202-4 (JUNE 1995), all U.S. Government
## End Users acquire the Software with only those rights set forth in the
## accompanying license agreement. PathScale, Inc. 477 N. Mathilda Ave;
## Sunnyvale, CA 94085.
	
## In the absence of an assembly version of __vrd2_pow, this is a simple
## vector wrapper around fastpow.

## Prototype:
##    __m128,__m128 __vrs8_powf (__m128 xlo, __m128 xhi,
##                               __m128 ylo, __m128 yhi)

#include "picdefs.S"

.text
.align 16
.p2align 4,,15

.globl __vrs8_powf
	.type	__vrs8_powf,@function
__vrs8_powf:
	sub		$104,%rsp		# 8 bytes of pad + 6 x 16 bytes
	movdqa		%xmm0, 32(%rsp)		# save xlo
	movdqa		%xmm1, 48(%rsp)		# save xhi
	movdqa		%xmm2, 0(%rsp)		# save ylo
	movdqa		%xmm3, 16(%rsp)		# save yhi

	movq		%xmm2, %xmm1
	call		EXTERN(fastpowf)
	movd		%xmm0, 64(%rsp)		# save result 0

	movd		36(%rsp), %xmm0		# load x[1]
	movd		4(%rsp), %xmm1		# load y[1]
	call		EXTERN(fastpowf)
	movd		%xmm0, 68(%rsp)		# save result 1

	movd		40(%rsp), %xmm0		# load x[2]
	movd		8(%rsp), %xmm1		# load y[2]
	call		EXTERN(fastpowf)
	movd		%xmm0, 72(%rsp)		# save result 2

	movd		44(%rsp), %xmm0		# load x[3]
	movd		12(%rsp), %xmm1		# load y[3]
	call		EXTERN(fastpowf)
	movd		%xmm0, 76(%rsp)		# save result 3

	movd		48(%rsp), %xmm0		# load x[4]
	movd		16(%rsp), %xmm1		# load y[4]
	call		EXTERN(fastpowf)
	movd		%xmm0, 80(%rsp)		# save result 4

	movd		52(%rsp), %xmm0		# load x[5]
	movd		20(%rsp), %xmm1		# load y[5]
	call		EXTERN(fastpowf)
	movd		%xmm0, 84(%rsp)		# save result 5

	movd		56(%rsp), %xmm0		# load x[6]
	movd		24(%rsp), %xmm1		# load y[6]
	call		EXTERN(fastpowf)
	movd		%xmm0, 88(%rsp)		# save result 6

	movd		60(%rsp), %xmm0		# load x[7]
	movd		28(%rsp), %xmm1		# load y[7]
	call		EXTERN(fastpowf)
	movd		%xmm0, 92(%rsp)		# save result 7

	movdqa		64(%rsp), %xmm0		# load result 0, 1, 2, 3
	movdqa		80(%rsp), %xmm1		# load result 4, 5, 6, 7
	add		$104,%rsp
	ret
