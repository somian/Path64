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


#ifndef __IOMODE_H__
#define __IOMODE_H__
#define WR_OP	1		/* last operation was a write */
#define RD_ONLY	0		/* the file is read-only */
#define WR_ONLY	1		/* the file is write-only */
#define RW_FILE	2		/* file opened for both read&write */
#define WR_READY 3		/* writable file in write mode */
#endif /* !__IOMODE_H__ */
