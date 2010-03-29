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
#include "dwarf_vars.h"
#include "dwarf_global.h"

int
dwarf_get_vars(Dwarf_Debug dbg,
	       Dwarf_Var ** vars,
	       Dwarf_Signed * ret_var_count, Dwarf_Error * error)
{
    int res;

    res =
       _dwarf_load_section(dbg,
		           dbg->de_debug_varnames_index,
			   &dbg->de_debug_varnames,
		           error);
    if (res != DW_DLV_OK) {
	return res;
    }

    return _dwarf_internal_get_pubnames_like_data(dbg, dbg->de_debug_varnames, dbg->de_debug_varnames_size, (Dwarf_Global **) vars,	/* type 
																	   punning,
																	   Dwarf_Type 
																	   is never
																	   a
																	   completed 
																	   type */
						  ret_var_count,
						  error,
						  DW_DLA_VAR_CONTEXT,
						  DW_DLE_DEBUG_VARNAMES_LENGTH_BAD,
						  DW_DLE_DEBUG_VARNAMES_VERSION_ERROR);
}


int
dwarf_varname(Dwarf_Var var_in, char **ret_varname, Dwarf_Error * error)
{
    Dwarf_Global var = (Dwarf_Global) var_in;

    if (var == NULL) {
	_dwarf_error(NULL, error, DW_DLE_VAR_NULL);
	return (DW_DLV_ERROR);
    }

    *ret_varname = (char *) (var->gl_name);
    return DW_DLV_OK;
}


int
dwarf_var_die_offset(Dwarf_Var var_in,
		     Dwarf_Off * returned_offset, Dwarf_Error * error)
{
    Dwarf_Global var = (Dwarf_Global) var_in;

    return dwarf_global_die_offset(var, returned_offset, error);

}


int
dwarf_var_cu_offset(Dwarf_Var var_in,
		    Dwarf_Off * returned_offset, Dwarf_Error * error)
{
    Dwarf_Global var = (Dwarf_Global) var_in;

    return dwarf_global_cu_offset(var, returned_offset, error);
}


int
dwarf_var_name_offsets(Dwarf_Var var_in,
		       char **returned_name,
		       Dwarf_Off * die_offset,
		       Dwarf_Off * cu_offset, Dwarf_Error * error)
{
    Dwarf_Global var = (Dwarf_Global) var_in;

    return
	dwarf_global_name_offsets(var,
				  returned_name, die_offset, cu_offset,
				  error);
}
