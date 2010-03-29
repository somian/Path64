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




/*
    This struct holds information about a abbreviation.
    It is put in the hash table for abbreviations for
    a compile-unit.
*/
struct Dwarf_Abbrev_List_s {

    Dwarf_Word ab_code;
    Dwarf_Half ab_tag;
    Dwarf_Half ab_has_child;

    /* 
       Points to start of attribute and form pairs in the .debug_abbrev 
       section for the abbrev. */
    Dwarf_Byte_Ptr ab_abbrev_ptr;

    struct Dwarf_Abbrev_List_s *ab_next;
};
