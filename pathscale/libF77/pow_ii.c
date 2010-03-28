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


/* $Header: /home/bos/bk/kpro64-pending/libF77/pow_ii.c 1.5 04/12/21 14:58:03-08:00 bos@eng-25.internal.keyresearch.com $ */

#include "cmplrs/host.h"

extern int32 __powii(int32 ap, int32 n);
extern int64 __powll(int64 ap, int64 n);

int64 __powli(int64 ap, int32 n)
{
int64 pow;
/* 10/9/89 fix bug 5116 */

if(n != 0) {
	if (n<0) {
		/* if ap <> 1 or -1, then any other integer raised to
		 * a negative power would return a fraction, which is 
		 * rounded to 0.  if ap = 1 or -1 then depending on the
		 * power, if the power is odd, then result = -1, else = 1
  		 */
		if ((ap!=1)&&(ap!=-1)) return(0); 
		n = -n;
		}
	pow=1;
	for( ; ; )
		{
		if(n & 01)
			pow = pow * ap;
		if(n >>= 1)
			ap = ap * ap;
		else
			return(pow);
		}
     }
else return(1);
}

int32 __powil(int32 ap, int64 n)
{
int32 pow;
/* 10/9/89 fix bug 5116 */

if(n != 0) {
	if (n<0) {
		/* if ap <> 1 or -1, then any other integer raised to
		 * a negative power would return a fraction, which is 
		 * rounded to 0.  if ap = 1 or -1 then depending on the
		 * power, if the power is odd, then result = -1, else = 1
  		 */
		if ((ap!=1)&&(ap!=-1)) return(0); 
		n = -n;
		}
	pow=1;
	for( ; ; )
		{
		if(n & 01)
			pow = pow * ap;
		if(n >>= 1)
			ap = ap * ap;
		else
			return(pow);
		}
     }
else return(1);
}

/* By-reference versions for backward compatibility. */

int32 pow_ii(int32 *ap, int32 *bp)
{
int32 pow;
pow=__powii(*ap,*bp);
return pow;
}

int64 pow_il(int32 *ap, int64 *bp)
{
int64 pow;
pow=__powil(*ap,*bp);
return pow;
}

int64 pow_li(int64 *ap, int32 *bp)
{
int64 pow;
pow=__powli(*ap,*bp);
return pow;
}

int64 pow_ll(int64 *ap, int64 *bp)
{
int64 pow;
pow=__powll(*ap,*bp);
return pow;
}
