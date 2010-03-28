/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */
#include "qk.h"
#include <errno.h>

/* Wrappers allow special libraries to avoid referring to fstatfs and
 * sigaltstack, which are present in the Cray Catamount environment, but
 * which cause link-time warnings and which do not work at execution time.
 */

int _Qk_fstatfs(int fd, struct statfs *buf) {
  errno = ENOSYS;
  return -1;
  }

int _Qk_sigaltstack(const stack_t *ss, stack_t *oss) {
  errno = ENOSYS;
  return -1;
  }

