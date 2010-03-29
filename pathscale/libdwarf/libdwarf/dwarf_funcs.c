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
#include "dwarf_funcs.h"
#include "dwarf_global.h"

int
dwarf_get_funcs(Dwarf_Debug dbg,
		Dwarf_Func ** funcs,
		Dwarf_Signed * ret_func_count, Dwarf_Error * error)
{
    int res;

    res =
	_dwarf_load_section(dbg,
			    dbg->de_debug_funcnames_index,
			    &dbg->de_debug_funcnames,
			    error);
    if (res != DW_DLV_OK) {
	return res;
    }

    return _dwarf_internal_get_pubnames_like_data(dbg, dbg->de_debug_funcnames, dbg->de_debug_funcnames_size, (Dwarf_Global **) funcs,	/* type 
																	   punning,
																	   Dwarf_Type 
																	   is never
																	   a
																	   completed 
																	   type */
						  ret_func_count,
						  error,
						  DW_DLA_FUNC_CONTEXT,
						  DW_DLE_DEBUG_FUNCNAMES_LENGTH_BAD,
						  DW_DLE_DEBUG_FUNCNAMES_VERSION_ERROR);

}


int
dwarf_funcname(Dwarf_Func func_in, char **ret_name, Dwarf_Error * error)
{
    Dwarf_Global func = (Dwarf_Global) func_in;

    if (func == NULL) {
	_dwarf_error(NULL, error, DW_DLE_FUNC_NULL);
	return (DW_DLV_ERROR);
    }

    *ret_name = (char *) (func->gl_name);
    return DW_DLV_OK;
}

int
dwarf_func_die_offset(Dwarf_Func func_in,
		      Dwarf_Off * return_offset, Dwarf_Error * error)
{
    Dwarf_Global func = (Dwarf_Global) func_in;

    return dwarf_global_die_offset(func, return_offset, error);
}


int
dwarf_func_cu_offset(Dwarf_Func func_in,
		     Dwarf_Off * return_offset, Dwarf_Error * error)
{
    Dwarf_Global func = (Dwarf_Global) func_in;

    return dwarf_global_cu_offset(func, return_offset, error);
}


int
dwarf_func_name_offsets(Dwarf_Func func_in,
			char **ret_func_name,
			Dwarf_Off * die_offset,
			Dwarf_Off * cu_die_offset, Dwarf_Error * error)
{
    Dwarf_Global func = (Dwarf_Global) func_in;

    return dwarf_global_name_offsets(func,
				     ret_func_name,
				     die_offset, cu_die_offset, error);
}
