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


/*
	If ag_end_symbol_index is zero, 
	ag_length must be known and non-zero.


	Deals with length being known costant or fr
	assembler output, not known.

*/

struct Dwarf_P_Arange_s {
    Dwarf_Addr ag_begin_address;	/* known address or for
					   symbolic assem output,
					   offset of symbol */
    Dwarf_Addr ag_length;	/* zero or address or offset */
    Dwarf_Unsigned ag_symbol_index;

    Dwarf_P_Arange ag_next;

    Dwarf_Unsigned ag_end_symbol_index;	/* zero or index/id of end
					   symbol */
    Dwarf_Addr ag_end_symbol_offset;	/* known address or for
					   symbolic assem output,
					   offset of end symbol */

};
