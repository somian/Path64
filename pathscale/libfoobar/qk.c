/*
 *  Copyright (C) 2007. PathScale Inc. All Rights Reserved.
 */

/*
 
   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

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

