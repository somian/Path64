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


#ifndef __WHIRL2F_H__
#define __WHIRL2F_H__



/* Assuming two's complement representation, we do this as follows
 * for a 32 bits integral type:
 *
 *   i >> n  ==>  ((-(2**n) | (i>>n))
 */
#define I4ASHR(v1, v2) \
   IOR(ISHFT(IAND('80000000'X, v1), -31)*ISHFT(-1, 32-v2), ISHFT(v1, -v2))
#define I8ASHR(v1, v2) \
   IOR(ISHFT(IAND('8000000000000000'X, v1), -63_8)*ISHFT(-1, 64-v2), ISHFT(v1, -v2))
#define U4ASHR(v1, v2) I4ASHR(v1, v2)
#define U8ASHR(v1, v2) I8ASHR(v1, v2)


/* See whirl2c.h for comments on this implementation of DIVFLOOR and
 * DIVCEIL.
 */
#define _I4SIGN(y) (2 * I4ASHR(y, 31) + 1)
#define _I4MASK(x, y, v) IAND(I4ASHR(IEOR(x,y), 31), v)
#define _I8SIGN(y) (2_8 * I8ASHR(y, 63) + 1_8)
#define _I8MASK(x, y, v) IAND(I8ASHR(IEOR(x,y), 63), v)

#define INTRN_I4DIVFLOOR(v1, v2) \
   ((v1) + _I4MASK(v1, v2, _I4SIGN(v2) - (v2))) / (v2)
#define INTRN_I8DIVFLOOR(v1, v2) \
   ((v1) + _I8MASK(v1, v2, _I8SIGN(v2) - (v2))) / (v2)
#define INTRN_U4DIVFLOOR(v1, v2) I4DIVFLOOR(v1, v2)
#define INTRN_U8DIVFLOOR(v1, v2) I8DIVFLOOR(v1, v2)

#define INTRN_I4DIVCEIL(v1, v2) -I4DIVFLOOR(-(v1), v2)
#define INTRN_I8DIVCEIL(v1, v2) -I8DIVFLOOR(-(v1), v2)
#define INTRN_U4DIVCEIL(v1, v2) ((v1)+(v2)-1)/(v2)
#define INTRN_U8DIVCEIL(v1, v2) ((v1)+(v2)-1)/(v2)

#define INTRN_I4MODFLOOR(v1, v2) (v1 - v2*I4DIVFLOOR(v1, v2))
#define INTRN_I8MODFLOOR(v1, v2) (v1 - v2*I8DIVFLOOR(v1, v2))
#define INTRN_U4MODFLOOR(v1, v2) (v1 - v2*(v1/v2))
#define INTRN_U8MODFLOOR(v1, v2) (v1 - v2*(v1/v2))

#define INTRN_I4MODCEIL(v1, v2) (v1 - v2*I4DIVCEIL(v1, v2))
#define INTRN_I8MODCEIL(v1, v2) (v1 - v2*I8DIVCEIL(v1, v2))
#define INTRN_U4MODCEIL(v1, v2) (v1 - v2*U4DIVCEIL(v1, v2))
#define INTRN_U8MODCEIL(v1, v2) (v1 - v2*U8DIVCEIL(v1, v2))


/* How do we do this without writing a function?
*/
#define CEIL(v) '?????TODO(CEIL)?????'
#define FLOOR(v) '?????TODO(FLOOR)?????'

#endif __WHIRL2F_H__
