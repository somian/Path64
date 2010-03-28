/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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
    lebdecodeu32.c

    $Revision$	
    $Date$
*/


#ifndef _LP64
#include <sgidefs.h>
#endif /* _LP64 */
#include <cmplrs/leb128.h>

/*
    This routine takes a pointer to an encoded unsigned leb128
    number, and returns the number of bytes in the encoding.
    In other words, it returns the length of the leb128 number.
    It also returns the number itself in *value.  The details
    of the leb128 encoding are in the Dwarf document.
*/
int
_leb128_unsigned_decode32(char *data, __uint32_t *value)
{
    __uint32_t lvalue;
    unsigned char byte;

    byte = *data;
    if ((byte & 0x80) == 0) {
	*value = byte;
	return 1;
    }
    else if ((*(data + 1) & 0x80) == 0) {
	lvalue = byte & 0x7f;
	lvalue |= (*(data + 1) & 0x7f) << 7;
	*value = lvalue;
	return 2;
    }
    else if ((*(data + 2) & 0x80) == 0) {
	lvalue = byte & 0x7f;
	lvalue |= (*(data + 1) & 0x7f) << 7;
	lvalue |= (*(data + 2) & 0x7f) << 14;

	*value = lvalue;
	return 3;
    }
    else if ((*(data + 3) & 0x80) == 0) {
	lvalue = byte & 0x7f;
	lvalue |= (*(data + 1) & 0x7f) << 7;
	lvalue |= (*(data + 2) & 0x7f) << 14;
	lvalue |= (*(data + 3) & 0x7f) << 21;

	*value = lvalue;
	return 4;
    }
    {
	lvalue = byte & 0x7f;
	lvalue |= (*(data + 1) & 0x7f) << 7;
	lvalue |= (*(data + 2) & 0x7f) << 14;
	lvalue |= (*(data + 3) & 0x7f) << 21;
	lvalue |= (*(data + 4) & 0x7f) << 28;

	*value = lvalue;
	return 5;
    }
}

