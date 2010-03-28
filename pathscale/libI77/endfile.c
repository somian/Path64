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
/* sjc  #1951	10Dec87		Use ftruncate rather than fork/exec of  */
/* 				/bin/cp                                 */

#include <cmplrs/fio.h>
#include <mutex.h>
#include <string.h>
#include "endfile.h"

#include "close.h"
#include "err.h"
#include "fmt.h"
#include "iomode.h"
#include "util.h"

#ifdef _BSD
#include <sys/types.h>
#include <sys/file.h>
#endif				/* _BSD */
#if defined(_SYSV) || defined(_SYSTYPE_SVR4)
#include <sys/fcntl.h>
#endif

#include "vmsflags.h"
static int one = 1;
static char     ctlz = '\032';

/* fwd decl. */
#if 00
static int f_end_com (alist64 *a, int lock);
#else
static int f_end_com (alist *a, int lock);
#endif

int
f_end (alist *a)
{
#if 00
    alist64 a64;
    a64.aerr = a->aerr;
    a64.aunit = a->aunit;
    return( f_end_com( &a64, 0 ) );
#else
    return( f_end_com( a, 0 ) );
#endif
}

int
f_end_mp (alist *a)
{
#if 00
    alist64 a64;
    a64.aerr = a->aerr;
    a64.aunit = a->aunit;
    return( f_end_com( &a64, 1 ) );
#else
    return( f_end_com( a, 1 ) );
#endif
}

#if 00
int
f_end64 (alist64 *a)
{
    return( f_end_com( a, 0 ) );
}

int
f_end64_mp (alist64 *a)
{
    return( f_end_com( a, 1 ) );
}

static int
f_end_com (alist64 *a, int lock)
#else
static int
f_end_com (alist *a, int lock)
#endif
{
   unit           *b, *ftnunit;
   int		   n;

   if ((ftnunit = b = map_luno (a->aunit)) == NULL)
      err (a->aerr, 114, "endfile");
   while (lock && test_and_set( &ftnunit->lock_unit, 1L ))
       ;

   if (b->uacc == KEYED || b->uacc == DIRECT)
      errret(a->aerr, 169, "endfile");

   if (b->uconn <= 0)
      goto end;
   b->uend = 1;
   if (b->useek == 0)
      goto end;
   if (f77vms_flag_[VMS_EF]) {	/* write an endfile record */
      if (b->uwrt != WR_READY && f77nowwriting (ftnunit))
         errret(a->aerr, 160, "endfile");
      if (b->ufmt == 1) {	/* ASCII formatted file */
#ifdef I90
	 /* If in Fortran-90 nonadvancing mode, write endfile record (\n only). */
	 if (ftnunit->f90sw == 1 && ftnunit->f90nadv == 1 ) {
	    putc ('\n', ftnunit->ufd);
	    ftnunit->f90nadv = 0;
	 }
#endif
	 putc ('\032', b->ufd);
	 putc ('\n', b->ufd);
      } else {			/* unformatted */
	 (void) fwrite (&one, sizeof (int), 1, b->ufd);
	 (void) fwrite (&ctlz, 1, 1, b->ufd);
	 if (fwrite (&one, sizeof (int), 1, b->ufd) != 1)
	    errret(a->aerr, errno, "system write error");
      }
   } else {
      if (b->uwrt != WR_READY && f77nowwriting (ftnunit))
         errret(a->aerr, 160, "endfile");

#ifdef I90
      /* If in Fortran-90 nonadvancing mode, write endfile record (\n only). */
      if (ftnunit->f90sw == 1 && ftnunit->f90nadv == 1 ) {
	  putc ('\n', b->ufd);
	  ftnunit->f90nadv = 0;
      }
#endif
   }
   
   n = t_runc (b, a->aerr);
   if (lock) ftnunit->lock_unit = 0;
   return (n);
end:
   if (lock) ftnunit->lock_unit = 0;
   return(0);
}

#pragma weak f_end64 = f_end
#pragma weak f_end64_mp = f_end_mp
