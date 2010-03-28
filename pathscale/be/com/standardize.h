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




#ifndef standardize_INCLUDED
#define standardize_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------
// NAME: WN_Upper_Bound_Standardize
// FUNCTION: For the do loop 'doloop' with upper bound 'ub', put that
//   bound in standard form, if possible.  If 'ok_to_fail', return
//   FALSE if we can't standardize the upper bound, otherwise assert.
//   Return TRUE if we can standardize.
//---------------------------------------------------------------------*/

extern BOOL WN_Upper_Bound_Standardize(WN *doloop,
                                       WN *ub,
                                       BOOL ok_to_fail);

#ifdef __cplusplus
}
#endif
#endif /* standardize_INCLUDED */
