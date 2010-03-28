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


#pragma ident "@(#) libu/util/rbn.c	92.1	07/07/99 13:18:33"

#include <cray/portdefs.h>

/* 
 *       RBN	Convert trailing blanks to nulls
 *
 *	Returns:
 *
 *		The Hollerith value with trailing blanks converted to nulls.
 */

#ifdef _UNICOS
#pragma _CRI duplicate RBN as _RBN
#pragma _CRI duplicate RBN as RBN@
#else
#pragma weak _RBN = RBN
#endif
int64
RBN(int64 *value)
{
#ifdef	_WORD32
	const int64	blanks = 0x2020202020202020LL;
#else
	const int64	blanks = 0x2020202020202020;
#endif
	int64		ret;

	if (*value == blanks)
		ret = 0;
	else
		ret = (((uint64)(((~blanks ^ *value) + 1) ^
			 (~blanks ^ *value) ) >> 3) & blanks) ^ *value;

	return(ret);
}
