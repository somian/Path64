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
#include <limits.h>
#include <malloc.h>

#ifdef _LONGLONG

#define  __LONGLONG_MAX  LONGLONG_MAX
#define  __ULONGLONG_MAX  ULONGLONG_MAX
#define  __LONGLONG_MIN  LONGLONG_MIN

#else

#define __LONGLONG_MAX  LONG_MAX
#define  __ULONGLONG_MAX  ULONG_MAX
#define  __LONGLONG_MIN  LONG_MIN
#endif				/* _LONGLONG */

#include <cmplrs/fio.h>
#include <stdlib.h>
#include "fmtlib.h"

char *icvt(int value, int *ndigit, int *sign, char *buf)
{
   char           *abuf;

   /* LHL 5/17/89 fix bug 4533 */
   short           minint = 0;

   if (value > 0)
      *sign = 0;
   else if (value < 0) {
      /* LHL 5/17/89 fix bug 4533 */
      if (value == INT_MIN) {
	 minint++;
	 value++;
      }
      value = -value;
      *sign = 1;
   } else {
      *sign = 0;
      *ndigit = 1;
      *buf = '0';
      *(buf + 1) = '\0';
      return (buf);
   }
   abuf = buf + MAXOCTLENGTH - 1;
   (*abuf--) = '\0';
   while (value > 0) {
      (*abuf--) = (char) (value % 10 + '0');
      value /= 10;
   }
   *ndigit = buf + MAXOCTLENGTH - 2 - abuf;
   /* LHL 5/17/89 fix bug 4533 */
   if (minint) {
      char           *ch;

      ch = buf + MAXOCTLENGTH - 2;
      (*ch)++;
   }
   return (abuf + 1);
}

char *llcvt(ftnll value, int *ndigit, int *sign, char *buf)
{
   char           *abuf;

   /* LHL 5/17/89 fix bug 4533 */
   short           minint = 0;

   if (value > 0)
      *sign = 0;
   else if (value < 0) {
      /* LHL 5/17/89 fix bug 4533 */
      if (value == (long long) -__LONGLONG_MAX - 1) {
	 minint++;
	 value++;
      }
      value = -value;
      *sign = 1;
   } else {
      *sign = 0;
      *ndigit = 1;
      *buf = '0';
      *(buf + 1) = '\0';
      return (buf);
   }
   abuf = buf + MAXOCTLENGTH - 1;
   (*abuf--) = '\0';
#if 0
   while ((*(long *) &value > 0) || (*(long *) &value == 0 &&
				     *((long *) &value + 1) != 0))
      /* basically 'while (value > (long long 0))' */
#endif
   while (value > 0LL) {
      long long       temp;

      temp = value % 10;
      (*abuf--) = (char) (temp + '0');
      value = value / 10;
   }
   *ndigit = buf + MAXOCTLENGTH - 2 - abuf;
   /* LHL 5/17/89 fix bug 4533 */
   if (minint) {
      char           *ch;

      ch = buf + MAXOCTLENGTH - 2;
      (*ch)++;
   }
   return (abuf + 1);
}

char *ozcvt(unsigned char *value, int len, int *ndigit, int base, char *buf)
{
   register char  *abuf;
   register unsigned char c, *vbuf;
   register unsigned int d;
   register int    ibits, bits, maxbytes;

   bits = ((--base) < 8) ? 3 : 4;
   maxbytes = (len * 8) / bits + 3;
#ifdef _MIPSEB
   vbuf = value;
   while (*vbuf == '\0' && len > 0) {
      vbuf++;
      len--;
   }
   vbuf += len - 1;
#else
   vbuf = value + len - 1;
   while (*vbuf == '\0' && len > 0) {
      vbuf--;
      len--;
   }
   vbuf = value;
#endif
   if (len == 0) {
      *buf = '0';
      *(buf + 1) = '\0';
      *ndigit = 1;
      return (buf);
   }
   abuf = buf + maxbytes - 1;
   (*abuf--) = '\0';
   d = 0;
   ibits = 0;

   while (len--) {
#ifdef _MIPSEB
      c = (*vbuf--);
#else
      c = *vbuf++;
#endif
      d |= c << ibits;
      ibits += 8;
      while (ibits >= bits) {
	 c = (unsigned char) (d & base);
	 (*abuf--) = (char) (c + (c > 9 ? ('A' - 10) : '0'));
	 d >>= bits;
	 ibits -= bits;
      }
   }
   if (ibits) {
      c = (unsigned char) (d & base);
      (*abuf--) = (char) (c + (c > 9 ? ('A' - 10) : '0'));
   }
   while (*++abuf == '0');
   *ndigit = buf + maxbytes - abuf - 1;
   return (abuf);
}

#ifdef I90
char *bcvt(unsigned char *value, int len, int *ndigit, char *buf)
{
   register char  *abuf;
   register unsigned char c, *vbuf;
   register int    maxbytes;

   maxbytes = len * 8;

#ifdef _MIPSEB
   vbuf = value;
   while (*vbuf == '\0' && len > 0) {
      vbuf++;
      len--;
   }
   vbuf += len - 1;
#else
   vbuf = value + len - 1;
   while (*vbuf == '\0' && len > 0) {
      vbuf--;
      len--;
   }
   vbuf = value;
#endif

   if (len == 0) {
      *ndigit = 0;
      return (0);
   }

   abuf = buf + maxbytes;
   (*abuf--) = '\0';

   while (len--) {

#ifdef _MIPSEB
      c = (*vbuf--);
#else
      c = *vbuf++;
#endif

      if (c & 1) (*abuf--) = '1';
      else (*abuf--) = '0';
      if (c & 2) (*abuf--) = '1';
      else (*abuf--) = '0';
      if (c & 4) (*abuf--) = '1';
      else (*abuf--) = '0';
      if (c & 8) (*abuf--) = '1';
      else (*abuf--) = '0';
      if (c & 16) (*abuf--) = '1';
      else (*abuf--) = '0';
      if (c & 32) (*abuf--) = '1';
      else (*abuf--) = '0';
      if (c & 64) (*abuf--) = '1';
      else (*abuf--) = '0';
      if (c & 128) (*abuf--) = '1';
      else (*abuf--) = '0';

   }

   while (*(++abuf) == '0' );
   *ndigit = buf + maxbytes - abuf;
   return (abuf);

}
#endif
