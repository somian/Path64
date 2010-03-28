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



/* $Header$ */

#include <cmplrs/fio.h>
#include "fio_direct_io.h"


#ifdef SIZEOF_LUNO_IS_64
#pragma weak flush64_ = __flush64_f77
int __flush64_f77(ftnll *luno)
#else
#pragma weak flush_ = __flush_f77
int __flush_f77(ftnint *luno)
#endif
{
   int             i;

   for (i = 0; i < mxunit; i++)
      if (f77units[i].luno == *luno) {
	 if (f77units[i].ufd != NULL)
            if ((f77units[i].uacc == DIRECT) && (f77units[i].ufmt == 0)) {
               /* DIRECT UNFORMATTED */
               (void) _fio_du_flush((int)f77units[i].ufd);
            } else {
               (void) fflush (f77units[i].ufd);
            }
	 break;
      }
   return (0);
}

#if SIZEOF_LUNO_IS_64
#pragma weak flush_ = __flush_f77
int __flush_f77 (ftnint *luno)
{
	ftnll lu_number = *luno;
	return __flush64_f77(&lu_number);
}
#endif

