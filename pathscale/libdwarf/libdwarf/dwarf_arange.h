/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.

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



/* This structure is used to read an arange into. */
struct Dwarf_Arange_s {

    /* Starting address of the arange, ie low-pc. */
    Dwarf_Addr ar_address;

    /* Length of the arange. */
    Dwarf_Unsigned ar_length;

    /* 
       Offset into .debug_info of the start of the compilation-unit
       containing this set of aranges. */
    Dwarf_Off ar_info_offset;

    /* Corresponding Dwarf_Debug. */
    Dwarf_Debug ar_dbg;
};



int
  _dwarf_get_aranges_addr_offsets(Dwarf_Debug dbg,
				  Dwarf_Addr ** addrs,
				  Dwarf_Off ** offsets,
				  Dwarf_Signed * count,
				  Dwarf_Error * error);
