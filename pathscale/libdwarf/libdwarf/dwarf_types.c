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
#include "dwarf_types.h"
#include "dwarf_global.h"

int
dwarf_get_types(Dwarf_Debug dbg,
		Dwarf_Type ** types,
		Dwarf_Signed * ret_type_count, Dwarf_Error * error)
{
    int res;

    res =
       _dwarf_load_section(dbg,
		           dbg->de_debug_typenames_index,
			   &dbg->de_debug_typenames,
			   error);
    if (res != DW_DLV_OK) {
	return res;
    }

    return _dwarf_internal_get_pubnames_like_data(dbg, dbg->de_debug_typenames, dbg->de_debug_typenames_size, (Dwarf_Global **) types,	/* type 
																	   punning,
																	   Dwarf_Type 
																	   is never
																	   a
																	   completed 
																	   type */
						  ret_type_count,
						  error,
						  DW_DLA_TYPENAME_CONTEXT,
						  DW_DLE_DEBUG_TYPENAMES_LENGTH_BAD,
						  DW_DLE_DEBUG_TYPENAMES_VERSION_ERROR);

}


int
dwarf_typename(Dwarf_Type type_in, char **ret_name, Dwarf_Error * error)
{
    Dwarf_Global type = (Dwarf_Global) type_in;

    if (type == NULL) {
	_dwarf_error(NULL, error, DW_DLE_TYPE_NULL);
	return (DW_DLV_ERROR);
    }

    *ret_name = (char *) (type->gl_name);
    return DW_DLV_OK;
}


int
dwarf_type_die_offset(Dwarf_Type type_in,
		      Dwarf_Off * ret_offset, Dwarf_Error * error)
{
    Dwarf_Global type = (Dwarf_Global) type_in;

    return dwarf_global_die_offset(type, ret_offset, error);
}


int
dwarf_type_cu_offset(Dwarf_Type type_in,
		     Dwarf_Off * ret_offset, Dwarf_Error * error)
{
    Dwarf_Global type = (Dwarf_Global) type_in;

    return dwarf_global_cu_offset(type, ret_offset, error);

}


int
dwarf_type_name_offsets(Dwarf_Type type_in,
			char **returned_name,
			Dwarf_Off * die_offset,
			Dwarf_Off * cu_die_offset, Dwarf_Error * error)
{
    Dwarf_Global type = (Dwarf_Global) type_in;

    return dwarf_global_name_offsets(type,
				     returned_name,
				     die_offset, cu_die_offset, error);
}
