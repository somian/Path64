/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

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



#ifndef _CMPLRS_LEB128_H
#define _CMPLRS_LEB128_H
/*

	LEB128.H

	reading and writing leb128 numbers.
	See the DWARF documentation for further information on leb numbers.
	No error indication is returned.

	The supplied buffer must be long enough to hold the leb number.
	For encode, 5 bytes is sufficient for 32 bit values and 10 bytes
	is sufficient for 64 bit values.

	The value returned is the number of bytes in the leb encoding.

	For improperly coded leb number (or one having more than
	the allowed number of leb digits with the most significant
	bit on in the input buffer) the
	leb buffer decode scan stops at 5 characters for 32 bit decodes 
	and stops at 10 characters for 64 bit decodes.  
	For such numbers the returned length will be 5  or 10 
	respectively, and no error indication is supplied.
	
	These routines are in libelfutil.a

*/
 


#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNUC__

extern int _leb128_unsigned_encode64(__uint64_t /*number*/, char* /*buffer*/);
extern int _leb128_signed_encode64(__int64_t /*number*/, char* /*buffer*/);

extern int _leb128_unsigned_encode32(__uint32_t /*number*/, char* /*buffer*/);
extern int _leb128_signed_encode32(__int32_t /*number*/, char* /*buffer*/);

extern int _leb128_unsigned_decode64(char* /*data*/, __uint64_t* /*value*/);
extern int _leb128_signed_decode64(char* /*data*/, __int64_t* /*value*/);

extern int _leb128_unsigned_decode32(char* /*data*/, __uint32_t* /*value*/);
extern int _leb128_signed_decode32(char* /*data*/, __int32_t* /*value*/);

#else

#include <sys/types.h>

extern int _leb128_unsigned_encode64(u_int64_t /*number*/, char* /*buffer*/);
extern int _leb128_signed_encode64(int64_t /*number*/, char* /*buffer*/);

extern int _leb128_unsigned_encode32(u_int32_t /*number*/, char* /*buffer*/);
extern int _leb128_signed_encode32(int32_t /*number*/, char* /*buffer*/);

extern int _leb128_unsigned_decode64(char* /*data*/, u_int64_t* /*value*/);
extern int _leb128_signed_decode64(char* /*data*/, int64_t* /*value*/);

extern int _leb128_unsigned_decode32(char* /*data*/, u_int32_t* /*value*/);
extern int _leb128_signed_decode32(char* /*data*/, int32_t* /*value*/);

#endif

#ifdef __cplusplus
}
#endif
#endif
