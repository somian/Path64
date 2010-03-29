/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000,2002,2004 Silicon Graphics, Inc.  All Rights Reserved.

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



#include "config.h"
#include "dwarf_incl.h"
#include <stdio.h>
#include "dwarf_weaks.h"
#include "dwarf_global.h"

int
dwarf_get_weaks(Dwarf_Debug dbg,
		Dwarf_Weak ** weaks,
		Dwarf_Signed * ret_weak_count, Dwarf_Error * error)
{
    int res;

    res =
       _dwarf_load_section(dbg,
		           dbg->de_debug_weaknames_index,
			   &dbg->de_debug_weaknames,
		           error);
    if (res != DW_DLV_OK) {
	return res;
    }

    return _dwarf_internal_get_pubnames_like_data(dbg, dbg->de_debug_weaknames, dbg->de_debug_weaknames_size, (Dwarf_Global **) weaks,	/* type 
																	   punning,
																	   Dwarf_Type 
																	   is never
																	   a
																	   completed 
																	   type */
						  ret_weak_count,
						  error,
						  DW_DLA_WEAK_CONTEXT,
						  DW_DLE_DEBUG_WEAKNAMES_LENGTH_BAD,
						  DW_DLE_DEBUG_WEAKNAMES_VERSION_ERROR);

}



int
dwarf_weakname(Dwarf_Weak weak_in, char **ret_name, Dwarf_Error * error)
{
    Dwarf_Global weak = (Dwarf_Global) weak_in;

    if (weak == NULL) {
	_dwarf_error(NULL, error, DW_DLE_WEAK_NULL);
	return (DW_DLV_ERROR);
    }
    *ret_name = (char *) (weak->gl_name);
    return DW_DLV_OK;
}


int
dwarf_weak_die_offset(Dwarf_Weak weak_in,
		      Dwarf_Off * weak_off, Dwarf_Error * error)
{
    Dwarf_Global weak = (Dwarf_Global) weak_in;

    return dwarf_global_die_offset(weak, weak_off, error);
}


int
dwarf_weak_cu_offset(Dwarf_Weak weak_in,
		     Dwarf_Off * weak_off, Dwarf_Error * error)
{
    Dwarf_Global weak = (Dwarf_Global) weak_in;

    return dwarf_global_cu_offset(weak, weak_off, error);
}


int
dwarf_weak_name_offsets(Dwarf_Weak weak_in,
			char **weak_name,
			Dwarf_Off * die_offset,
			Dwarf_Off * cu_offset, Dwarf_Error * error)
{
    Dwarf_Global weak = (Dwarf_Global) weak_in;

    return dwarf_global_name_offsets(weak,
				     weak_name,
				     die_offset, cu_offset, error);
}
