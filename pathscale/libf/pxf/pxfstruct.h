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


/* USMID @(#) libf/pxf/pxfstruct.h	92.0	10/08/98 14:30:10 */

#ifndef _PXFSTRUCT_H
#define _PXFSTRUCT_H

struct pxfhandle {
	void *pxfstructptr;
	int pxftype;
};

#define PXF_FLOCK 1
#define PXF_UTIMBUF 2
#define PXF_UNAMBUF 3
#define PXF_STATBUF 4
#define PXF_TMSBUF 5
#define PXF_GROUP 6
#define PXF_PASSWD 7
#define PXF_DIRENT 8
#define PXF_SIGSET 9
#define PXF_SIGACTION 10
#define PXF_TERMIOS 11
#define PXF_SUBHAND 12

#endif
