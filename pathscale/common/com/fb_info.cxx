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
// ====================================================================
// ====================================================================
//
// Module: fb_info.cxx
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Description:
//
// The file fb_info.cxx defines the string array FB_EDGE_NAMES, and
// implements the procedures FB_EDGE_TYPE_fprintf and
// FB_EDGE_TYPE_sprintf.
//
// ====================================================================
// ====================================================================

#include "fb_info.h"

const char *FB_EDGE_NAMES[]
    = { "------", "INCOMING", "OUTGOING", "ENTRY_OUTGOING",
	"BRANCH_TAKEN", "BRANCH_NOT_TAKEN",
	"LOOP_ZERO", "LOOP_POSITIVE", "LOOP_OUT", "LOOP_BACK",
	"LOOP_EXIT", "LOOP_ITERATE",
	"CIRCUIT_LEFT", "CIRCUIT_RIGHT", "CIRCUIT_NEITHER",
	"CALL_INCOMING", "CALL_OUTGOING", "CALL_INOUTSAME",
	"IO_OUTGOING", "IO_ESCAPE[1]", "IO_ESCAPE[2]", "IO_ESCAPE[3]",
	"SWITCH_DEFAULT" };

void
FB_EDGE_TYPE_fprintf( FILE *fp, const FB_EDGE_TYPE fb_type )\
{
  if ( fb_type < FB_EDGE_SWITCH_BASE ) {
    fprintf( fp, "%s", FB_EDGE_NAMES[fb_type] );
  } else {
    fprintf( fp, "SWITCH[%d]", fb_type - FB_EDGE_SWITCH_BASE );
  }
}

INT
FB_EDGE_TYPE_sprintf( char *buffer, const FB_EDGE_TYPE fb_type )
{
  if ( fb_type < FB_EDGE_SWITCH_BASE ) {
    return sprintf( buffer, "%s", FB_EDGE_NAMES[fb_type] );
  } else {
    return sprintf( buffer, "SWITCH[%d]", fb_type - FB_EDGE_SWITCH_BASE );
  }
}
