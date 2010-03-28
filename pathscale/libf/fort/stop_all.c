/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fort/stop_all.c	92.1	06/24/99 10:18:36"

/*
 * This routine is provided on MPP systems, to allow one PE in a
 * parallel region to interrupt the other PEs and exit.
 * (STOP is defined to stop all PEs in a master region, but only
 * the calling PE in a parallel region).
 *
 * We just #include the STOP code here, after setting the STOPALL
 * symbol (this alters how STOP builds itself slightly).  We then
 * call STOP from STOP_ALL (we inline it so we get a good traceback).
 */
#include <signal.h>
#define STOPALL
#include "stop.c"

void
STOP_ALL(s)
_fcd	s;
{
#pragma inline _STOP
	_STOP(s);
}
void
STOP_ALL_DISABLE()
{
	sigset_t bufiomask = sigmask(SIGBUFIO);
	(void)sigprocmask(SIG_BLOCK, &bufiomask, NULL);
}
void
STOP_ALL_ENABLE()
{
	sigset_t bufiomask = sigmask(SIGBUFIO);
	(void)sigprocmask(SIG_UNBLOCK, &bufiomask, NULL);
}
