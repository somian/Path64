/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

*/


/* $Header: /home/bos/bk/kpro64-pending/libF77/mclock_.c 1.5 04/12/21 14:58:02-08:00 bos@eng-25.internal.keyresearch.com $ */

#include <sys/types.h>
#include <sys/times.h>
#include "cmplrs/host.h"

#ifdef KEY /* Bug 1683 */

static clock_t mclock_ (void);

int
pathf90_mclock(void)
{
return (int) mclock_();
}

long long
pathf90_mclock8(void)
{
return (long long) mclock_();
}

/* Don't pollute the Fortran namespace */
static clock_t
mclock_ (void)

#else

clock_t
mclock_ (void)

#endif /* KEY Bug 1683 */
{
    struct tms buf;
    times(&buf);
    return (buf.tms_utime + buf.tms_cutime + buf.tms_cstime);
}
