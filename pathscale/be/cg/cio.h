/*
 * Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
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




#ifndef cio_INCLUDED
#define cio_INCLUDED



// Flags controlling cross iteration copy propagation and
// read/write/CSE removal

extern BOOL  CIO_enable_invariant_copy_removal;
extern BOOL  CIO_enable_copy_removal;
extern BOOL  CIO_enable_read_removal;
extern BOOL  CIO_enable_write_removal;
extern BOOL  CIO_enable_cse_removal;
extern INT32 CIO_rw_max_omega;
extern INT32 CIO_reserve_int;
extern INT32 CIO_reserve_fp;

#endif /* cio_rwtran_INCLUDED */
