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


/* USMID @(#) libu/ffio/c1/er90by.h	92.0	10/08/98 14:57:41 */


#include <sys/types.h>
#include "tapeio.h"

typedef struct {
	unsigned 	rwtpmk:1;	/* user read/write tapemarks */
	unsigned	unused:63;
	int		fd;		/* file descriptor */
	int		tpos;		/* positioning? */
	struct tptsireq tsireq;
} ER90BYT;



