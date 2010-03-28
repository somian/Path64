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


#pragma ident "@(#) libfi/array/allocated.c	92.1	07/07/99 15:52:02"

#include <cray/dopevec.h>

#define TRUE 1
#define FALSE 0

/*
 *    ALLOCATED Returns a logical scalar indicating whether an allocatable
 *                array is allocated.
 *              Returns TRUE if the allocatable array is allocated.
 *                Otherwise returrn FALSE.
 *              The result is undefined if the allocation status of the
 *                array is undefined.
 */

_f_log
_ALLOCATED (DopeVectorType * source)
{

	_f_log iresult;
	iresult = FALSE;
	/* Is source is an allocatable array and allocated. */
	if ((source->p_or_a == ALLOC_ARRY) && (source->assoc))
		iresult = TRUE;
        return(_btol(iresult));
}


#ifdef	_F_LOG4
_f_log4
_ALLOCATED_4 (DopeVectorType * source)
{

	_f_log4 iresult;
	iresult = FALSE;
	/* Is source is an allocatable array and allocated. */
	if ((source->p_or_a == ALLOC_ARRY) && (source->assoc))
		iresult = TRUE;
        return(_btol(iresult));
}
#endif


#ifdef	_F_LOG8
_f_log8
_ALLOCATED_8 (DopeVectorType * source)
{

	_f_log8 iresult;
	iresult = FALSE;
	/* Is source is an allocatable array and allocated. */
	if ((source->p_or_a == ALLOC_ARRY) && (source->assoc))
		iresult = TRUE;
        return(_btol(iresult));
}
#endif
