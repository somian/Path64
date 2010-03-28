#
# Copyright (C) 2008. PathScale, LLC. All Rights Reserved.
#
# Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.

# Optimized memset for Opteron/Athlon64.  Use mov/movnti with 64-bit integer
# registers.

#if defined(BUILD_OS_DARWIN)
	.file	"memset_opteron.s"
	.section __TEXT,__textcoal_nt,coalesced,pure_instructions
	.p2align 4,,15
.globl memset.pathscale.opteron
	.globl memset
memset:
	.weak_definition memset
#else /* defined(BUILD_OS_DARWIN) */
	.file	1	"memset_opteron.s"
	.text
	.p2align 4,,15
	.section .text
.globl memset.pathscale.opteron
	.type	memset.pathscale.opteron, @function	# memset (rdi, esi, rdx)
#endif /* defined(BUILD_OS_DARWIN) */
memset.pathscale.opteron:	# bb_entry
	movq	%rdi,%rax		# return the first arg
	cmpq	$0x7,%rdx
	jbe	bb_8.memset_opteron

	# bb_1
	# Align the main loop by storing the first 7 or fewer bytes.
	testq	$0x7,%rdi
	je	bb_3.memset_opteron

bb_2.memset_opteron:	# bb_2
	movb	%sil,(%rdi)
	incq	%rdi
	decq	%rdx
	testq	$0x7,%rdi
	jne	bb_2.memset_opteron

bb_3.memset_opteron:	# bb_3
	# The loop is aligned.
	mov	$0x101010101010101,%r10		# replicate val 8 times
	movzbl	%sil,%ecx
	imul	%rcx,%r10
	movq	%rdx,%r8		# see if we need 64-byte block moves
	shr	$6,%r8
	je	bb_8.memset_opteron	# no move of 64-byte blocks, go copy
					# the remaining bytes

	# bb_4
	#cmpq	$120000,%rdx
	cmpq	$0x100000,%rdx	# 1MB
	jae	bb_7.memset_opteron

bb_5.memset_opteron:	# bb_5
	# Use mov for main loop.
	.p2align 4,,15
	movq	%r10,(%rdi)
	movq	%r10,8(%rdi)
	movq	%r10,16(%rdi)
	movq	%r10,24(%rdi)
	movq	%r10,32(%rdi)
	movq	%r10,40(%rdi)
	movq	%r10,48(%rdi)
	movq	%r10,56(%rdi)
	addq	$64,%rdi
	decq	%r8
	jne	bb_5.memset_opteron

	# bb_6
	jmp	bb_8.memset_opteron	# finish up the remaining bytes

bb_7.memset_opteron:	# bb_7
	# Use movnti for main loop.
	.p2align 4,,15
	movnti	%r10,(%rdi)
	movnti	%r10,8(%rdi)
	movnti	%r10,16(%rdi)
	movnti	%r10,24(%rdi)
	movnti	%r10,32(%rdi)
	movnti	%r10,40(%rdi)
	movnti	%r10,48(%rdi)
	movnti	%r10,56(%rdi)
	addq	$64,%rdi
	decq	%r8
	jne	bb_7.memset_opteron

bb_7a.memset_opteron:
	sfence			# add store fence instruction after movnti

bb_8.memset_opteron:
	andq	$0x3f,%rdx	# reduce byte count by the 64-byte block moves
	movq	%rdx,%r9	# see if we need 8-byte moves, rdx is count
	shrq	$3,%r9
	je	bb_10.memset_opteron	# no 8-byte moves needed, go finish up
					# the 7 or fewer trailing bytes

bb_9.memset_opteron:
	movq	%r10,(%rdi)	# do 8-byte moves
	addq	$8,%rdi
	dec	%r9
	jne	bb_9.memset_opteron

bb_10.memset_opteron:
	# Finish up the remaining 7 or fewer bytes.
	andq	$0x7,%rdx
	je	bb_12.memset_opteron

bb_11.memset_opteron:
	movb	%sil,(%rdi)
	incq	%rdi
	decq	%rdx
	jne	bb_11.memset_opteron

bb_12.memset_opteron:
	retq

#if ! defined(BUILD_OS_DARWIN)
.weak memset
	memset = memset.pathscale.opteron
#endif /* defined(BUILD_OS_DARWIN) */
