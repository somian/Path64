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
	
## In the absence of an assembly version of __vrd2_cosh, this is a simple
## vector wrapper around cosh.

## Prototype:
##    __m128d __vrd2_cosh (__m128d x)

#include "picdefs.S"

.text
.align 16
.p2align 4,,15

.globl __vrd2_cosh
	.type	__vrd2_cosh,@function
__vrd2_cosh:
	sub		$24,%rsp		# 8 + 1 x 16 bytes
	movdqa		%xmm0, 0(%rsp)		# save x

	call		EXTERN(cosh)
	movq		%xmm0, 16(%rsp)		# save result 0

	movq		8(%rsp), %xmm0		# load x[1]
	call		EXTERN(cosh)
	
	movq		16(%rsp), %xmm1		# load result 0
	shufpd		$0, %xmm0, %xmm1	# splice two results
	movdqa		%xmm1, %xmm0		# here is the unified result
	add		$24,%rsp
	ret
