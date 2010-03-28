/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */
#ifndef _QK_INCL
#define _QK_INCL

#if defined(BUILD_OS_DARWIN)
#include <sys/param.h>
#include <sys/mount.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <sys/statfs.h>
#endif /* defined(BUILD_OS_DARWIN) */
#include <signal.h>

int _Qk_fstatfs(int fd, struct statfs *buf);

int sigaltstack(const stack_t *ss, stack_t *oss);

#endif /* _QK_INCL */
