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


#pragma ident "@(#) libu/multi/atx@ckstrm.c	92.1	05/19/99 14:19:43"


/*
 *      Multi-streaming MSPs are not supported.
 */

extern void ATX@CHECKSTRM(int *arg1);

/*
 * Note that asm entry point $STREAM$ON is set to zero by libc
 * in lib/libc/multi to indicate the presence of the streaming
 * library.  It may mean that streaming is being done or could
 * be done.
 *
 * This is for PVP SV1 systems only.
 */

#pragma _CRI soft $STREAM$ON
extern int $STREAM$ON(void);

void
ATX@CHECKSTRM(int *stream_flag)
{
	*stream_flag = 1;
	if ($STREAM$ON == 0)
		*stream_flag = 0;
}
