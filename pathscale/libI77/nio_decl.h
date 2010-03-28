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



typedef struct {
   int             ndims;	/* # of dimensions */
   XINT_TYPE             nels;	/* # of elements */
   XINT_TYPE             baseoff;	/* how to get to (0,...,0) element */
   XINT_TYPE             span[7];	/* span of each dimension +1+ */
}               NAMEDims;		/* dimension descriptor */

 /* +1+ note: only # of dimensions applies here, i.e. actual array is
  * between 0 and 7 elements based on ndims */


typedef struct {
   char            varname[36];	/* name of variable */
   Pointer         varaddr;	/* where it is */
   int             type;	/* its type */
   NAMEDims           *dimp;	/* dimension descriptor */
}               NAMENlentry;	/* namelist entry: 1 for each var */


typedef struct {
   char            nlname[36];	/* name of namelist */
   NAMENlentry         nlvnames[1];	/* array of variable descriptors +2+ */
}               NAMENamelist;

 /* +2+ note: this array is not bounded but is terminated by an entry
  * with a null varname */

