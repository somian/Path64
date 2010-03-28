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


/* $Header: /home/bos/bk/kpro64-pending/libF77/_powll.c 1.3 04/12/21 14:58:01-08:00 bos@eng-25.internal.keyresearch.com $ */

#include "cmplrs/host.h"

int64 __powll(int64 ap, int64 n)
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
