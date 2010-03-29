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


/* libdwarfdefs.h
*/

#ifndef LIBDWARFDEFS_H
#define LIBDWARFDEFS_H

/* We want __uint32_t and __uint64_t and __int32_t __int64_t
   properly defined but not duplicated, since duplicate typedefs
   are not legal C.
*/
/*
 HAVE___UINT32_T
 HAVE___UINT64_T will be set by configure if
 our 4 types are predefined in compiler
*/


#if (!defined(HAVE___UINT32_T)) && defined(HAVE___UINT32_T_IN_SGIDEFS_H)
#include <sgidefs.h>		/* sgidefs.h defines them */
#define HAVE___UINT32_T 1
#endif

#if (!defined(HAVE___UINT64_T)) && defined(HAVE___UINT64_T_IN_SGIDEFS_H)
#include <sgidefs.h>		/* sgidefs.h defines them */
#define HAVE___UINT64_T 1
#endif


#if (!defined(HAVE___UINT32_T)) &&   \
	defined(HAVE_SYS_TYPES_H) &&   \
	defined(HAVE___UINT32_T_IN_SYS_TYPES_H)
#  include <sys/types.h>
#define HAVE___UINT32_T 1
#endif

#if (!defined(HAVE___UINT64_T)) &&   \
	defined(HAVE_SYS_TYPES_H) &&   \
	defined(HAVE___UINT64_T_IN_SYS_TYPES_H)
#  include <sys/types.h>
#define HAVE___UINT64_T 1
#endif

#ifndef HAVE___UINT32_T
typedef int __int32_t;
typedef unsigned __uint32_t;
#define HAVE___UINT32_T 1
#endif

#ifndef HAVE___UINT64_T
typedef long long __int64_t;
typedef unsigned long long __uint64_t;
#define HAVE___UINT64_T 1
#endif

#endif /* LIBDWARFDEFS_H */
