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

int
dwarf_get_str(Dwarf_Debug dbg,
	      Dwarf_Off offset,
	      char **string,
	      Dwarf_Signed * returned_str_len, Dwarf_Error * error)
{
    int res;

    if (dbg == NULL) {
	_dwarf_error(NULL, error, DW_DLE_DBG_NULL);
	return (DW_DLV_ERROR);
    }

    if (offset == dbg->de_debug_str_size) {
	/* Normal (if we've iterated thru the set of strings
	   using dwarf_get_str and are at the end). */
	return DW_DLV_NO_ENTRY;
    }
    if (offset > dbg->de_debug_str_size) {
	_dwarf_error(dbg, error, DW_DLE_DEBUG_STR_OFFSET_BAD);
	return (DW_DLV_ERROR);
    }

    if (string == NULL) {
	_dwarf_error(dbg, error, DW_DLE_STRING_PTR_NULL);
	return (DW_DLV_ERROR);
    }

    res =
       _dwarf_load_section(dbg,
                           dbg->de_debug_str_index,
                           &dbg->de_debug_str,
                           error);
    if (res != DW_DLV_OK) {
        return res;
    }

    *string = (char *) dbg->de_debug_str + offset;

    *returned_str_len = (strlen(*string));
    return DW_DLV_OK;
}
