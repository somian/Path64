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
	This struct holds the abbreviation table, before they are written 
	on disk. Holds a linked list of abbreviations, each consisting of
	a bitmap for attributes and a bitmap for forms
*/
typedef struct Dwarf_P_Abbrev_s *Dwarf_P_Abbrev;

struct Dwarf_P_Abbrev_s {
    Dwarf_Unsigned abb_idx;	/* index of abbreviation */
    Dwarf_Tag abb_tag;		/* tag of die */
    Dwarf_Ubyte abb_children;	/* if children are present */
    Dwarf_ufixed *abb_attrs;	/* holds names of attrs */
    Dwarf_ufixed *abb_forms;	/* forms of attributes */
    int abb_n_attr;		/* num of attrs = # of forms */
    Dwarf_P_Abbrev abb_next;
};

/* used in pro_section.c */

int _dwarf_pro_add_AT_fde(Dwarf_P_Debug dbg, Dwarf_P_Die die,
			  Dwarf_Unsigned offset, Dwarf_Error * error);

int _dwarf_pro_add_AT_stmt_list(Dwarf_P_Debug dbg,
				Dwarf_P_Die first_die,
				Dwarf_Error * error);

int _dwarf_pro_add_AT_macro_info(Dwarf_P_Debug dbg,
				 Dwarf_P_Die first_die,
				 Dwarf_Unsigned offset,
				 Dwarf_Error * error);
