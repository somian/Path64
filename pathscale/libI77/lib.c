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
#include <string.h>
#include <cmplrs/fio.h>

int
setcilist (cilist *x, int u, char *fmt, int rec, int xerr, int end)
{
   x->ciunit = u;
   x->cifmt = fmt;
   x->cirec = rec;
   x->cierr = xerr;
   x->ciend = end;
   return 0;
}

int
setolist (olist *x, int xunit, char *fname, char *sta, char *fm, int rl, char *blnk, int oerr)
{
   x->oerr = oerr;
   x->ounit = xunit;
   x->ofnm = fname;
   x->ofnmlen = (int) strlen (fname);
   x->osta = sta;
   x->ofm = fm;
   x->orl = rl;
   x->oblnk = blnk;
   return 0;
}

int
stcllist (cllist *x, int xunit, char *stat, int cerr)
{
   x->cerr = cerr;
   x->cunit = xunit;
   x->csta = stat;
   return 0;
}

int
setalist (alist *x, int xunit, int aerr)
{
   x->aunit = xunit;
   x->aerr = aerr;
   return 0;
}

#if 11
int
setcilist64 (cilist64 *x, ftnint u, char *fmt, XINT64 rec, int xerr, int end)
{
   x->ciunit = u;
   x->cifmt = fmt;
   x->cirec = rec;
   x->cierr = xerr;
   x->ciend = end;
   return 0;
}

int
setolist64 (olist64 *x, ftnint xunit, char *fname, char *sta, char *fm, int rl, char *blnk, int oerr)
{
   x->oerr = oerr;
   x->ounit = xunit;
   x->ofnm = fname;
   x->ofnmlen = (int) strlen (fname);
   x->osta = sta;
   x->ofm = fm;
   x->orl = rl;
   x->oblnk = blnk;
   return 0;
}

#if 00
int
stcllist64 (cllist64 *x, ftnint xunit, char *stat, int cerr)
{
   x->cerr = cerr;
   x->cunit = xunit;
   x->csta = stat;
   return 0;
}
#else
/* #pragma weak stcllist64 = stcllist */
#endif

#if 00
int
setalist64 (alist64 *x, ftnint xunit, int aerr)
{
   x->aunit = xunit;
   x->aerr = aerr;
   return 0;
}
#else
/* #pragma weak setalist64 = setalist */
#endif

#endif	/* -n32 and -64 */
