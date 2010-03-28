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


//-*-c++-*-
//=====================================================================
//
// Module: rbi.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  7-SEP-95 dahl - Original Version
//
// Description:
//	Region Boundary Info (RBI)
//
//======================================================================
//======================================================================

#ifndef rbi_INCLUDED
#define rbi_INCLUDED "rbi.h"
#ifdef _KEEP_RCS_ID
static char *rbircs_id = rbi_INCLUDED"$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include "region_util.h"

const INT32 WN_PROP_USED_IN  = 0x01,
	    WN_PROP_DEF_IN   = 0x02,
	    WN_PROP_LIVE_OUT = 0x04;

class RBI {

private:
  INT32	    _trace_level;		// -ttTP_REGION(41), -ttRGN
  MEM_POOL *_pool;
  WN_MAP    _prop_map;
  WN_MAP    _tag_map;			// WN map, mark nodes with region id

public:
  RBI(MEM_POOL *);			// pool must be supplied to constructor
  ~RBI(void);

  // 0 - no trace
  // 1 - major function trace only (TT_REGION_RBI_DEBUG and other debug bits)
  // 2 - detailed trace (only RBI bit set TT_REGION_RBI_DEBUG)
  BOOL Trace( void ) const		{ return _trace_level > 0; }
  BOOL Trace2( void ) const		{ return _trace_level == 2; }

  MEM_POOL *Mem_Pool( void )            { return _pool; }

  void RBI_Calc_Kids(RID *root, const DU_MANAGER *du_mgr,
		     const ALIAS_MANAGER *am);

  void RBI_Calc_Rgn(RID *const rid_node, const DU_MANAGER *const du_mgr,
		    const ALIAS_MANAGER *const am);

  INT32 Rgn_WN_Tag( WN *const wn ) const
    {
      return WN_MAP32_Get( _tag_map, wn );
    }

  void Set_WN_Cur_Rgn( WN* wn, INT32 rgn_id )
    {
      WN_MAP32_Set( _tag_map, wn, rgn_id );
    }

  BOOL Outside_Cur_Rgn( WN* wn, INT32 rgn_id ) const
    {
      // RETURNs are always considered to lie outside since they
      // amount to exits from the region and anything that could be
      // used by the exited-to code is found on the RETURN's
      // def-list.
      if (WN_opcode(wn) == OPC_RETURN)
	return TRUE;
      else
	return (Rgn_WN_Tag(wn) != rgn_id);
    }

  INT32 Get_WN_Prop( WN *const wn ) const
    {
      return WN_MAP32_Get( _prop_map, wn );
    }

  void Set_WN_Prop( WN *const, const INT32 );

  void Set_PT_SET_All_Aliasable(POINTS_TO_SET *);

  void Add_To_PT_SET(POINTS_TO_SET *, POINTS_TO *);

};	// end of class RBI

extern "C" void Region_Bound_Info(WN *tree,
				  DU_MANAGER *du_mgr,
				  ALIAS_MANAGER *am);

#endif /* ifdef rbi_INCLUDED */
