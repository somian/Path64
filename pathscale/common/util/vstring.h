/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

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


#ifndef vstring_INCLUDED
#define vstring_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/*
 *  String utilities that use varying length strings
 */

typedef struct {
	int len;
	int max;
	char *str;
} vstring;

/* only vstring.c should set the values in the vstring structure */
#define vstr_len(v)	((v).len+0)
#define vstr_str(v)	((v).str+0)

/*
 * For now, can only have one vstring buffer live at a time.
 * Someday should enhance this, but not needed now.
 */

/* must call vstr_{begin,end} around use of a vstring buffer */
extern vstring vstr_begin (int len);
extern void vstr_end (vstring v);

/* add char to vstring */
extern vstring vstr_append (vstring v, const char c);

/* add string to vstring */
extern vstring vstr_concat (vstring v, const char *s);

/* sprintf that reallocs space if needed */
extern int vstr_sprintf (vstring *v, int index, const char *format, ... /* args */);

#ifdef __cplusplus
}
#endif
#endif	/* vstring_INCLUDED */
