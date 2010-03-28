#
# Copyright (C) 2008. PathScale, LLC. All Rights Reserved.
#
# Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.

# Optimized memset for EM64T.  Use movdqa/movntdq with 128-bit xmm registers.

#if defined(BUILD_OS_DARWIN)
	.file		"memset_em64t.s"
	.text
	.p2align 4,,15
.globl memset.pathscale.em64t
#else /* defined(BUILD_OS_DARWIN) */
	.file	1	"memset_em64t.s"
	.text
	.p2align 4,,15
	.section .text
.globl memset.pathscale.em64t
	.type	memset.pathscale.em64t, @function	# memset (rdi, esi, rdx)
#endif /* defined(BUILD_OS_DARWIN) */
memset.pathscale.em64t:	# bb_entry
	movq	%rdi,%rax		# return the first arg

	# Replicate value 8 times.
	mov	$0x101010101010101,%r10		# replicate val 8 times
	movzbl	%sil,%ecx
	imul	%rcx,%r10

	cmpq	$0xf,%rdx
	jbe	bb_8.memset_em64t

	# bb_1
	# Align the main loop by storing the first 15 or fewer bytes.
	testq	$0xf,%rdi
	je	bb_3.memset_em64t

bb_2.memset_em64t:	# bb_2
	movb	%sil,(%rdi)
	incq	%rdi
	decq	%rdx
	testq	$0xf,%rdi
	jne	bb_2.memset_em64t

bb_3.memset_em64t:	# bb_3
	# The loop is 16-byte aligned.
	movq	%rdx,%r8		# see if we need 64-byte block moves
	shr	$6,%r8
	je	bb_8.memset_em64t	# no move of 64-byte blocks, go copy
					# the remaining bytes

	# Replicate the original value 16 times.
	pushq	%r10
	pushq	%r10
	movdqu	(%rsp),%xmm0		# xmm0 has value replicated 16 times
	addq	$16,%rsp

	# bb_4
	cmpq	$0x100000,%rdx	# 1MB
	jae	bb_7.memset_em64t

bb_5.memset_em64t:	# bb_5
	# Use mov for main loop.
	.p2align 4,,15
	movdqa	%xmm0,(%rdi)
	movdqa	%xmm0,16(%rdi)
	movdqa	%xmm0,32(%rdi)
	movdqa	%xmm0,48(%rdi)

	addq	$64,%rdi
	decq	%r8
	jne	bb_5.memset_em64t

	# bb_6
	jmp	bb_8.memset_em64t	# finish up the remaining bytes

bb_7.memset_em64t:	# bb_7
	# Use movnt for main loop.
	.p2align 4,,15
	movntdq	%xmm0,(%rdi)
	movntdq	%xmm0,16(%rdi)
	movntdq	%xmm0,32(%rdi)
	movntdq	%xmm0,48(%rdi)

	addq	$64,%rdi
	decq	%r8
	jne	bb_7.memset_em64t

bb_7a.memset_em64t:
	sfence			# add store fence instruction after movnt

bb_8.memset_em64t:
	andq	$0x3f,%rdx	# reduce byte count by the 64-byte block moves
	movq	%rdx,%r9	# see if we need 8-byte moves, rdx is count
	shrq	$3,%r9
	je	bb_10.memset_em64t	# no 8-byte moves needed, go finish up
					# the 7 or fewer trailing bytes

bb_9.memset_em64t:
	movq	%r10,(%rdi)	# do 8-byte moves
	addq	$8,%rdi
	dec	%r9
	jne	bb_9.memset_em64t

bb_10.memset_em64t:
	# Finish up the remaining 7 or fewer bytes.
	andq	$0x7,%rdx
	je	bb_12.memset_em64t

bb_11.memset_em64t:
	movb	%sil,(%rdi)
	incq	%rdi
	decq	%rdx
	jne	bb_11.memset_em64t

bb_12.memset_em64t:
	retq
