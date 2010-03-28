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


#pragma ident "@(#) libfi/subrtn/f90_date_and_time.c	92.1	07/13/99 10:47:32"

#include <fortran.h>
#include <cray/dopevec.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

typedef struct
{
    char *ptr;   		/* character portion */
    unsigned long   len;	/* its length */
} fcd_t;

extern int _DATE_AND_TIME( fcd_t dat,
			   fcd_t tim,
			   fcd_t zon,
			   DopeVectorType *values );


int _F90_DATE_AND_TIME( char           *dat,
                        char           *tim,
	                char           *zon,
	                DopeVectorType *values,
		        int            dat_len,
	                int            tim_len,
	                int            zon_len )
{
    fcd_t  fcd1, fcd2, fcd3;

    fcd1.ptr = dat;
    fcd1.len = dat_len;
    fcd2.ptr = tim;
    fcd2.len = tim_len;
    fcd3.ptr = zon;
    fcd3.len = zon_len;
    
    return( _DATE_AND_TIME( fcd1, fcd2, fcd3, values ) );
}
