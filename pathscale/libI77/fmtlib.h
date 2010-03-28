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


#ifndef __FMTLIB_H__
#define __FMTLIB_H__
/* $Header$ */
extern char *icvt(int, int *, int *, char *);
extern char *llcvt(ftnll , int *, int *, char *);
extern char *ozcvt(unsigned char *, int , int *, int, char * );
#ifdef I90
extern char *bcvt(unsigned char *, int , int *, char * );
#endif

#define MAXOCTLENGTH (32*8+2)/3+1	/* allow 32-byte items */

#endif /* !__FMTLIB_H__ */
