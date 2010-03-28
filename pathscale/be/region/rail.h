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
// Module: rail.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Revision history:
//  24-AUG-95 dahl - Original Version
//
// Description:
//	Regions Around Inner Loops (RAIL)
//	Finds inner loops and places regions around them.
//
//======================================================================
//======================================================================

#ifndef rail_INCLUDED
#define rail_INCLUDED "rail.h"
#ifdef _KEEP_RCS_ID
static char *railrcs_id = rail_INCLUDED"$ $Revision$";
#endif /* _KEEP_RCS_ID */

class RAIL {

private:
  BOOL _trace_flag;			// -ttTP_REGION(41)
  WN *_func_entry;			// WN func_entry

public:
  RAIL(void);				// default constructor sets trace flag
  ~RAIL(void)				{ }

  BOOL Trace(void)			{ return _trace_flag;	}
  void Set_fe(WN *wn)			{ _func_entry = wn;	}
  WN *Get_fe(void)			{ return _func_entry;	}
  void Next_fe(void)			{ _func_entry = WN_next(_func_entry); }

  void Process_func_entry(WN *);	// go through all functions
  BOOL Process_block(WN *, INT32);	// TRUE if found loop, FALSE otherwise
  WN *Add_region_around_loop(WN *, INT32); // add region node

};	// end of class RAIL

#endif /* ifdef rail_INCLUDED */
