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




/* Handle error passing in the name of the Dwarf_P_Debug
   User must supply {} around the macro.
   Putting the {} here leads to macro uses that don't look like C.
   The error argument to dwarf_error is hard coded here as 'error'
*/
#define DWARF_P_DBG_ERROR(dbg,errval,retval) \
     _dwarf_p_error(dbg,error,errval); return(retval);

struct Dwarf_Error_s {
    Dwarf_Sword er_errval;
};

void _dwarf_p_error(Dwarf_P_Debug dbg, Dwarf_Error * error,
		    Dwarf_Word errval);
