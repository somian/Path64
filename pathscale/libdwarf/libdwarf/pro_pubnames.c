/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000,2004 Silicon Graphics, Inc.  All Rights Reserved.

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
#include "libdwarfdefs.h"
#include <stdio.h>
#include <string.h>
#ifdef HAVE_ELFACCESS_H
#include <elfaccess.h>
#endif
#include "pro_incl.h"
#include "pro_section.h"


/*
    This function adds another public name to the 
    list of public names for the given Dwarf_P_Debug.  
    It returns 0 on error, and 1 otherwise.
*/

Dwarf_Unsigned
dwarf_add_pubname(Dwarf_P_Debug dbg,
		  Dwarf_P_Die die,
		  char *pubname_name, Dwarf_Error * error)
{
    return
	_dwarf_add_simple_name_entry(dbg, die, pubname_name,
				     dwarf_snk_pubname, error);
}
