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



#include "ecvt_mp.h"
#include <stdio.h>

#define	NDIG 82
#define _ROUNDUP_RM 1

char *
ecvt_mp (double arg, int ndigits, int *decpt, int *sign, char *buffer)
{
    /*
    extern FILE *debugfile;
    */
    if (ndigits > 17) {
	register char *p, *e;
	*decpt = __dtoa (buffer, 17, arg, 0, _ROUNDUP_RM) + 1;
        for (p = buffer+18, e = buffer + 1 + (ndigits > NDIG ? NDIG : ndigits);
             p != e; ) *p++ = '0';
        *p++ = '\0';
    }
    else if (ndigits <= 0) {
	*decpt = __dtoa (buffer, 1, arg, 0, _ROUNDUP_RM) + 1;
	buffer[1] = '\0';
    }
    else {
	*decpt = __dtoa (buffer, ndigits, arg, 0, _ROUNDUP_RM) + 1;
    }
    *sign = buffer[0] == '-';
    return buffer+1;
}

char *
fcvt_mp (double arg, int ndigits, int *decpt, int *sign, char *buffer)
{
    *decpt = __dtoa (buffer, ndigits, arg, 1, _ROUNDUP_RM) + 1;
    if (*decpt + ndigits <= 0 && buffer[1] >= '5' && buffer[1] <= '9') {
	/* need to round up the digit beyond 'ndigits' */
	buffer[1] = '1';
	buffer[2] = '0';
	buffer[3] = '\0';
	(*decpt)++;
    }
    *sign = buffer[0] == '-';
    return buffer+1;
}

char *
qecvt_mp (long double arg, int ndigits, int *decpt, int *sign, char *buffer)
{
    if (ndigits > 34) {
	register char *p, *e;
	*decpt = _qtoa (buffer, 34, arg, 0) + 1;
        for (p = buffer+35, e = buffer + 1 + (ndigits > NDIG ? NDIG : ndigits);
             p != e; ) *p++ = '0';
        *p++ = '\0';
    }
    else if (ndigits <= 0) {
	*decpt = _qtoa (buffer, 1, arg, 0) + 1;
	buffer[1] = '\0';
    }
    else {
	*decpt = _qtoa (buffer, ndigits, arg, 0) + 1;
    }
    *sign = buffer[0] == '-';
    return buffer+1;
}

char *
qfcvt_mp (long double arg, int ndigits, int *decpt, int *sign, char *buffer)
{
    *decpt = _qtoa (buffer, ndigits, arg, 1) + 1;
    if (*decpt + ndigits <= 0 && buffer[1] >= '5' && buffer[1] <= '9') {
	/* need to round up the digit beyond 'ndigits' */
	buffer[1] = '1';
	buffer[2] = '0';
	buffer[3] = '\0';
	(*decpt)++;
    }
    *sign = buffer[0] == '-';
    return buffer+1;
}
