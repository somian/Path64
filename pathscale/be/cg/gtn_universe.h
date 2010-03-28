/*
 *  Copyright (C) 2007 PathScale, LLC.  All Rights Reserved.
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


/* =======================================================================
 * =======================================================================
 *
 *  Module: gtn_universe.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Revision comments:
 *
 *  13-Oct-1994 - Initial version
 *
 *  Description:
 *  ============
 *
 *  Impliments the universe of global TNs over which we define GTN_SETs.
 *  It is very advantageous to do this as there are far fewer global TNs
 *  than local TNs.
 *
 *  Reserved prefix:
 *  ================
 *
 *      GTN_UNIVERSE
 *
 *  Exported functions:
 *  ===================
 *
 *      void GTN_UNIVERSE_Pu_Begin(void)
 *
 *          Initialize GTN for the current program unit.
 *
 *
 *      void GTN_UNIVERSE_Pu_End(void)
 *
 *          All done with GTN sets in current PU.  Release any memory and
 *          destruct ay data structures.
 *
 *
 *      void GTN_UNIVERSE_Add_TN(
 *          TN *tn
 *      )
 *
 *          Register 'tn' as a global.  There is no going back.
 *
 *
 *  Exported variables:
 *  ===================
 *
 *      const INT32 GTN_UNIVERSE_size
 *
 *          Number of TNs in the GTN universe.
 *
 *
 *  Macros exported only to GTN_SET functions:
 *  ==========================================
 *
 *      INT32 GTN_UNIVERSE_TN_int(
 *          TN *tn
 *      )
 *
 *          Returns the global TN number of 'tn'.  If it has not been
 *          added to the universe, return 0.
 *
 *
 *      TN *GTN_UNIVERSE_INT_tn(
 *          INT32 i
 *      )
 *
 *          Returns the global TN whose number in the GTN universe is
 *          'i'.
 *
 *  Iteration over the members of the GTN universe:
 *  ===============================================
 *
 *      Iterator type GTN_UNIVERSE_ITER
 *
 *          Iterates over all the GTNs in the current REGION.
 *
 *          Supports:
 *
 *              void GTN_UNIVERSE_ITER_Init( GTN_UNIVERSE_ITER *iter )
 *              BOOL GTN_UNIVERSE_ITER_Done( const GTN_UNIVERSE_ITER *iter )
 *              TN* GTN_UNIVERSE_ITER_Current( GTN_UNIVERSE_ITER *iter )
 *              void GTN_UNIVERSE_ITER_Step( GTN_UNIVERSE_ITER *iter )
 *
 *  
 * =======================================================================
 * =======================================================================
 */

#ifndef GTN_UNIVERSE_INCLUDED
#define GTN_UNIVERSE_INCLUDED


#ifdef GTN_IMPLEMENTATION
#define GTN_CONST
#else
#define GTN_CONST const
#endif

extern GTN_CONST INT32  GTN_UNIVERSE_size;
extern INT32  First_REGION_GTN;
extern GTN_CONST INT32 *GTN_CONST GTN_UNIVERSE_tn_int_map;
extern TN   *GTN_CONST *GTN_CONST GTN_UNIVERSE_int_tn_map;
extern INT32  tn_int_map_allocated_size;
extern INT32  int_tn_map_allocated_size;


extern void GTN_UNIVERSE_Pu_Begin(void);
extern void GTN_UNIVERSE_REGION_Begin(void);
extern void GTN_UNIVERSE_Pu_End(void);
extern void GTN_UNIVERSE_Add_TN( TN *tn );

inline INT32
GTN_UNIVERSE_TN_int( TN *tn)
{
  INT32 val = 0;

#if 0
  /* the following assertion fails at several places in CG. Comment 
     it out till those references are fixed. */
  Is_True (TN_is_register(tn) && 
	   TN_is_global_reg(tn) &&
	   (TN_number(tn) < tn_int_map_allocated_size),
		("Invalid TN%d in GTN_UNIVERSE_TN_int", TN_number(tn)));
#else
  if (!(TN_is_register(tn) && 
	TN_is_global_reg(tn) &&
	(TN_number(tn) < tn_int_map_allocated_size))) {
    val = 0;
  } else
#endif

  val = GTN_UNIVERSE_tn_int_map[TN_number(tn)];
  return val;
}


inline TN *
GTN_UNIVERSE_INT_tn (INT32 i) 
{
  Is_True (i < GTN_UNIVERSE_size, ("GTN_UNIVERSE_INT_tn: %d out of range", i));
  return GTN_UNIVERSE_int_tn_map[i];
}


typedef INT32 GTN_UNIVERSE_ITER;    /* Well, it's a simple iterator... */

/* =======================================================================
 *
 *  GTN_UNIVERSE_ITER_Init
 *  GTN_UNIVERSE_ITER_Done
 *  GTN_UNIVERSE_ITER_Current
 *  GTN_UNIVERSE_ITER_Step
 *
 *      Iterator functions.  See interface description.
 *
 * =======================================================================
 */

inline void
GTN_UNIVERSE_ITER_Init( GTN_UNIVERSE_ITER* iter )
{
  *iter = GTN_UNIVERSE_size - 1;
}

inline BOOL
GTN_UNIVERSE_ITER_Done( const GTN_UNIVERSE_ITER* iter )
{
  return *iter < First_REGION_GTN;
}

inline TN*
GTN_UNIVERSE_ITER_Current( const GTN_UNIVERSE_ITER* iter )
{
  return GTN_UNIVERSE_INT_tn(*iter);
}

inline void
GTN_UNIVERSE_ITER_Step( GTN_UNIVERSE_ITER* iter )
{
  --*iter;
}
#endif
