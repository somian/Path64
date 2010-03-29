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



/*  Bytes needed to encode a number.
    Not a tight bound, just a reasonable bound.
*/
#define ENCODE_SPACE_NEEDED   (2*sizeof(Dwarf_Unsigned))


int _dwarf_pro_encode_leb128_nm(Dwarf_Unsigned val, int *nbytes,
				char *space, int splen);

int _dwarf_pro_encode_signed_leb128_nm(Dwarf_Signed value, int *nbytes,
				       char *space, int splen);
