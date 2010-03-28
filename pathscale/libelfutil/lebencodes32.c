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
    lebencodes32.c

    $Revision$  
    $Date$
*/

#ifndef _LP64
#include <sgidefs.h>
#endif /* _LP64 */
#include <cmplrs/leb128.h>


/* 
    This routine encodes the given signed number in leb128 mode,
    and stores the bytes at address buffer.  It returns the number 
    of bytes in the encoding.  It is the user's responsibility to
    make sure there is enough storage at buffer.  The details of
    leb128 encoding are in the Dwarf document.

    This relies on right shifting of negative values preserving
    the sign.  Implementation defined according to ISO C.

    5 bytes in buffer are all that is needed.
*/
int
_leb128_signed_encode32(__int32_t number, char *buffer)
{
    int		sign;
    char	*bufPtr;
    char	byte;

    sign = (number < 0) ? -1 : 0;
    bufPtr = buffer;

    for(;;) {
	byte = (number & 0x7f);
	number >>= 7;

	if (number == sign && (byte & 0x40) == ((char)sign & 0x40)) {
	    *bufPtr = byte;
	    bufPtr++;
    	    return bufPtr - buffer;
	}
	byte |= 0x80;
	*bufPtr = byte;
	bufPtr++;
    }

}
