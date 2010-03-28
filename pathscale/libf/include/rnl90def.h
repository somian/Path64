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


/* USMID @(#) libf/include/rnl90def.h	92.0	10/08/98 14:30:10 */
#ifndef _RNL90DEF_H
#define _RNL90DEF_H

#define BLANK	((int) ' ')
#define NULLC	((int) '\0')
#define OCTAL	1		/* reading octal input */
#define HEX	2		/* reading hex input */
#define SKIPMSG " - Skipped record named: "
#define UNITSTR " On Unit: "

#define OCTSHFT 61		/* allow 20 octal digits */
#define HEXSHFT 60		/* allow 15 hex digits */
#define OCTSHFT4 30		/* allow 10 octal digits */
#define HEXSHFT4 28		/* allow 7 hex digits */

#ifdef KEY /* Bug 14479 */
#define MAXNAML 63		/* 63-character name length */
#else /* KEY Bug 14479 */
#define MAXNAML 31		/* 31-character name length */
#endif /* KEY Bug 14479 */
#define MAXDIMS 8		/* Maximum number of dimensions for an array */
#define RNL_ERROR -3

#define ISBLANK(c)	(((c) == ' ') || ((c) == '\t'))
#define ISSEP(c)	(isspace(c) || (c) =='/' || (c) ==',')

#define ENDD(cond, css, n) {		\
	if (!(cond))			\
		_ferr(css, n);		\
	else {				\
		errn	= n;		\
		goto finalization;	\
	}				\
}
#define ERROR0(cond, css, n) {		\
	if (!(cond))			\
		_ferr(css, n);		\
	else				\
		goto finalization;	\
}

#define ERROR1(cond, css, n, p) {	\
	if (!(cond))			\
		_ferr(css, n, p);	\
	else				\
		goto finalization;	\
}

#define ERROR3(cond, css, n, p, t) {	\
	if (!(cond))			\
		_ferr(css, n, p, t);	\
	else				\
		goto finalization;	\
}

/*
 * Use GETSTRD to read a character string surrounded by quotes or
 * apostrophes. Comment characters are not recognized as such inside a
 * quoted string, so SUBGTC is used.
 */
#define GETSTRD() {				\
	SUBGTC(ch);				\
	if (ch == enddelim) {			\
		eos	= -1; /* end of string */\
		SUBGTC(ch); /* unless string delimiter is doubled */ \
		if (ch == enddelim)		\
		eos	= 0;			\
		else {				\
			cup->ulineptr--;	\
			cup->ulinecnt++;	\
		}				\
	}					\
}

/* Logical equates */

#ifndef	TRUE
#define TRUE	(_btol(1))
#endif

#ifndef	FALSE
#define FALSE	(_btol(0))
#endif

#ifndef	_UNICOS
/*
 * TOGGLE_CHAR sets or clears the bit for a particular character.
 *	TOGGLE_CHAR(c,a,b)
 *	where:	c	is the character
 *		cat	is the category
 *		val	set bit if != 0, clear bit if 0
 */
#define TOGGLE_CHAR(c,cat,val) { \
	if (val)							\
		_MASKS[(c >= 0x3f) ? cat+1 : cat] |= (1 << (IND(c)));	\
	else								\
		_MASKS[(c >= 0x3f) ? cat+1 : cat] &= ~(1 << (IND(c)));	\
}

/* IND computes the bit index of a character */
#define IND(c)	((c >= 0x3f) ? 0x7f - (unsigned)c : (unsigned)(0x40 - c - 1))
#endif

/* Indicies into _MASKS */

#ifndef MRNLDELIM
#define MRNLDELIM	0
#endif

#ifndef MRNLREP
#define MRNLREP		4
#endif

#ifndef MRNLSEP
#define MRNLSEP		6
#endif

#ifndef MRNLFLAG
#define MRNLFLAG	8
#endif

#ifndef MRNLCOMM
#define MRNLCOMM	10
#endif

extern	long	_BLNKSEP;
#ifdef _WORD32
extern	long long _MASKS[];
#else
extern	long	_MASKS[];
#endif
extern	unum_t	_OUT_UNIT;
extern	long	_SKP_MESS;
extern	long	_TYP_CONV;

#endif /* !_RNL90DEF_H */
