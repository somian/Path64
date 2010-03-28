/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */
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

/* $HEADER$ */

#ifndef _IO_BYTESWAP_H
#define _IO_BYTESWAP_H

#ifdef KEY /* Bug 4260 */
typedef enum {
  IO_DEFAULT,		/* Implicit: no swapping, no warning about FILENV */
  IO_NATIVE,		/* Explicit: no swapping, warn about FILENV */
  IO_SWAP,		/* Files are opposite of native, whatever that is */
  IO_BIG_ENDIAN,	/* Files are big-endian */
  IO_LITTLE_ENDIAN	/* Files are little-endian */
} IO_BYTESWAP;
#endif /* KEY Bug 4260 */

#endif	/* !_IO_BYTESWAP_H */
