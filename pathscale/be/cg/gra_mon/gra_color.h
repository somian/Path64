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

//  GRA register assignment via graph coloring
/////////////////////////////////////
//  
//  Description:
//
//      Interface for actual register assignment.
//
//  Exported functions:
//
//      void GRA_Color(void)
//          Choose registers for all the global LRANGEs in the current PU or
//          region or whatever.
//
/////////////////////////////////////


//  $Revision$
//  $Date$
//  $Author$
//  $Source$


#ifndef GRA_COLOR_INCLUDED
#define GRA_COLOR_INCLUDED
#ifndef GRA_COLOR_RCS_ID
#define GRA_COLOR_RCS_ID
#ifdef _KEEP_RCS_ID
static char *gra_color_rcs_id = "$Source$ $Revision$";
#endif
#endif

extern void
GRA_Color(void);

#endif
