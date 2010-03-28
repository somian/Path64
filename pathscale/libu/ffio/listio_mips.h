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


/* USMID @(#) libu/ffio/listio_mips.h	92.0	10/08/98 14:57:41 */

#ifndef _LISTIO_MIPS_FF
#define _LISTIO_MIPS_FF

/* Some defines that translate SGI's constants to Cray's constants */
#ifdef __mips
#include <aio.h>
#define LC_START LIO_NOWAIT
#define LC_WAIT LIO_WAIT
#define LF_LSEEK 1
#define LO_READ LIO_READ
#define LO_WRITE LIO_WRITE
#endif

#endif
