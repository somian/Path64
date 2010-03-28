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


#pragma ident "@(#) libfi/char/f90_trim.c	92.1	07/08/99 10:41:51"

#include <cray/dopevec.h>

typedef struct
{
    char *ptr;   		/* character portion */
    unsigned long   len;	/* its length */
} fcd_t;

extern void _TRIM( DopeVectorType *result,
		   fcd_t          source );

void _F90_TRIM( DopeVectorType *result,
	        char           *src_str,
		int            src_len )
{	        
    fcd_t  fcd1;

    fcd1.ptr = src_str;
    fcd1.len = src_len;
  /*
   * _TRIM is defined as returning an FCD, but actually does not.
   */
    _TRIM( result, fcd1 );
}
