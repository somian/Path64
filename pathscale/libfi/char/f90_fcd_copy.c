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


#pragma ident "@(#) libfi/char/f90_fcd_copy.c	92.2	01/20/95 09:56:38"
#include <string.h>

void _F90_FCD_COPY( char **ret_addr,
		    int  *ret_len,
                    char *trg_addr,
		    char *src_addr,
                    int  trg_len,
		    int  src_len )
{
    unsigned int len;

    len = (trg_len > src_len) ? src_len : trg_len; /* MIN(trg_len, src_len) */

    if (len > 0){
	(void) memcpy(trg_addr, src_addr, len);
    }

    *ret_addr = trg_addr + len;
    *ret_len  = trg_len - len;
    return;

}
