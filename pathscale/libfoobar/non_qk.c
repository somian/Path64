/*
 *  Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */
#include "qk.h"
#include <errno.h>

/* Wrappers allow normal libraries to refer to fstatfs and sigaltstack */

int _Qk_fstatfs(int fd, struct statfs *buf) {
  return fstatfs(fd, buf);
  }

int _Qk_sigaltstack(const stack_t *ss, stack_t *oss) {
  return sigaltstack(ss, oss);
  }

