
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

#if defined(__linux__) || defined(BUILD_OS_DARWIN)

/* 
 * On linux these functions are already declared in string.h, and we
 * want to avoid conflicting declarations.
 */ 
#include <string.h>

#else /* __linux__ */

#ifndef __BSTRING_H__
#define __BSTRING_H__
#ifdef __cplusplus
extern "C" {
#endif


#ident "$Revision$"

#if !defined(_SIZE_T) && !defined(_SIZE_T_)
#define _SIZE_T
#if (_MIPS_SZLONG == 32)
typedef unsigned int	size_t;
#endif
#if (_MIPS_SZLONG == 64)
typedef unsigned long	size_t;
#endif
#endif

extern void	bcopy(const void *, void *, size_t);
extern int	bcmp(const void *, const void *, size_t);
extern void	bzero(void *, size_t);
extern void	blkclr(void *, size_t);

#ifdef __cplusplus
}
#endif
#endif /* !__BSTRING_H__ */

#endif /* __linux__ */
