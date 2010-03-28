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


/* USMID @(#) libf/include/wnl90def.h	92.0	10/08/98 14:30:10 */
#ifndef _WNL90DEF_H
#define _WNL90DEF_H

#define ERROR0(cond, css, n) {                          \
	if (!(cond))                                    \
		_ferr(css,n);                           \
	else                                            \
		goto finalization;                      \
}

#define ERROR1(cond, css, n, p) {                       \
	if (!(cond))                                    \
		_ferr(css,(n), p);                      \
	else                                            \
		goto finalization;                      \
}

/*
 * NLWFLUSH writes what's in line buffer.
 * Reset pointers and counters so we start at the beginning of the buffer.
 * The first character in line buffer is used for carriage control.
 */

#define NLWFLUSH() {                            \
        if (_fwch(cup, cup->ulinebuf, cup->ulinemax, 1) < 0) { \
                ERROR0(errf, css, errno);       \
        }                                       \
        cup->ulineptr   = cup->ulinebuf;        \
        cup->ulinemax   = 0;                    \
}

/*
 * NLCHAR(X) writes a character to the line buffer
 */

#define NLCHAR(x) {                             \
	if (cup->ulinemax++ > cup->unmlsize) {  \
		ERROR0(errf, css, FEWRLONG); /* output record too long */ \
	} 					\
        *(cup->ulineptr++)      = (long) x;     \
}

/*
 * NLINE determines whether user specified new line for each variable.
 */

#define NLINE() { \
	if (OUT_LINE) {                                                 \
		NLWFLUSH();     /* Write out what's in line buffer */   \
		NLCHAR(' ');    /* write blank                  */      \
		NLCHAR(' ');    /* write blank                  */      \
	}                                                               \
}

extern  long    _wnlrecsiz;
extern  long    OUT_CHAR;
extern  long    OUT_ECHO;
extern  long    OUT_LINE;
extern  long    OUT_EQ;
extern  long    OUT_SEP;

#endif /* !_WNL90DEF_H */
