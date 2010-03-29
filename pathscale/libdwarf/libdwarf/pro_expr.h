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


#define	MAXIMUM_LOC_EXPR_LENGTH		20

struct Dwarf_P_Expr_s {
    Dwarf_Small ex_byte_stream[MAXIMUM_LOC_EXPR_LENGTH];
    Dwarf_P_Debug ex_dbg;
    Dwarf_Unsigned ex_next_byte_offset;
    Dwarf_Unsigned ex_reloc_sym_index;
    Dwarf_Unsigned ex_reloc_offset;
};
