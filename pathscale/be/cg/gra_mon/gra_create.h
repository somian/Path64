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

/*                    Create Basic GRA Data Structures
 *                    ================================
 *
 *  Exported functions:
 *
 *      void GRA_Create()
 *
 *          Traverse the flow grap building GRA's basic data structures:
 *
 *              1. The GRA_REGIONs that represent the separately preallocated
 *                 parts of the PU and the compliment.
 *
 *              2. The GRA_BBs that encapsulate the BBs.  GRA_bb_map is
 *                 set up to map from BBs to GRA_BBs.
 *
 *              3. The LRANGEs that represent register candidates.
 *                 GRA_tn_lrange_map is set up to map from TNs to LRANGEs.
 *
 *              4. The LUNITs that represent a BB within a particular
 *                 LRANGE
 *
 *      void GRA_Delete()
 *                 
 *          Destroy all the above  when we are done with it.
 */


/*
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 */


#ifndef CREATE_INCLUDED
#define CREATE_INCLUDED

#ifndef CREATE_RCS_ID
#define CREATE_RCS_ID
#ifdef _KEEP_RCS_ID
static char *create_rcs_id = "$Source$ $Revision$";
#endif
#endif

extern void GRA_Create(void);
extern void GRA_Delete(void);

#endif
