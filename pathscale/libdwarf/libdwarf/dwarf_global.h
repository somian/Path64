/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2004 Silicon Graphics, Inc.  All Rights Reserved.

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




typedef struct Dwarf_Global_Context_s *Dwarf_Global_Context;

/* 
    This struct contains header information for a set of pubnames.
    Essentially, they contain the context for a set of pubnames 
    belonging to a compilation-unit.


    This is also used for the sgi-specific
    weaknames, typenames, varnames, funcnames data:
    the structs for those are incomplete and
    instances of this are used instead.

*/
struct Dwarf_Global_Context_s {

    /* 
       Length in .debug_pubnames of a set of pubnames for a
       compilation-unit. Dwarf_Word pu_length; The value is not made
       available outside libdwarf and not used inside, so no need to
       record it. */

    /* for this context, size of a length. 4 or 8 */
    unsigned char pu_length_size;
    /* for this CU, size of the extension 0 except for dwarf2 extension 
       64bit, in which case is 4. */
    unsigned char pu_extension_size;

    /* 
       Offset into .debug_info of the compilation-unit header (not DIE) 
       for this set of pubnames. */
    Dwarf_Off pu_offset_of_cu_header;

    /* Size of compilation-unit that these pubnames are in. */
    Dwarf_Unsigned pu_info_length;

    Dwarf_Debug pu_dbg;
};


/* This struct contains information for a single pubname. */
struct Dwarf_Global_s {

    /* 
       Offset from the start of the corresponding compilation-unit of
       the DIE for the given pubname CU. */
    Dwarf_Off gl_named_die_offset_within_cu;

    /* Points to the given pubname. */
    Dwarf_Small *gl_name;

    /* Context for this pubname. */
    Dwarf_Global_Context gl_context;
};

int _dwarf_internal_get_pubnames_like_data(Dwarf_Debug dbg,
					   Dwarf_Small *
					   section_data_ptr,
					   Dwarf_Unsigned
					   section_length,
					   Dwarf_Global ** globals,
					   Dwarf_Signed * return_count,
					   Dwarf_Error * error,
					   int allocation_code,
					   int length_err_num,
					   int version_err_num);
