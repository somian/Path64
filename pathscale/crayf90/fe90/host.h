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



/* USMID:  "\n@(#)5.0_pl/headers/host.h	5.5	10/14/99 12:53:57\n" */

/* This module is for host specific information.  All uses of _HOST32 and */
/* _HOST64 and any other host defines should only be referenced in this   */
/* module.                                                                */


# if defined(_HOST_OS_LINUX) || defined(_HOST_OS_DARWIN)
   typedef	long double		ldouble;
# elif defined(_HOST_OS_SOLARIS) || defined(_HOST_OS_IRIX)
   typedef	double			ldouble;
# else
   typedef	long double		ldouble;
# endif


# if defined(_HOST32) && defined(_TARGET64)
  typedef       long long               long_type;
  typedef       double                  float_type;
# else
#   if defined(_HOST_OS_MAX)
      typedef   double                  float_type;
#   else
      typedef   float                   float_type;
#   endif
  typedef       long                    long_type;
# endif

/* Define the host machines 64 bit integer type */

# if defined(_HOST32)
  typedef       long long               long64;
# else
  typedef       long                    long64;
# endif


typedef unsigned int                    boolean;
typedef unsigned int                    Uint;
typedef unsigned long                   Ulong;


# include <stdlib.h>            /* Needed for getenv.      */
# include <ctype.h>
# include <stdio.h>
# include <string.h>
# include <limits.h>
# include <unistd.h>

# if defined(_HOST_OS_UNICOS) || defined(_HOST_OS_MAX) || (defined(_HOST_OS_IRIX) || defined(_HOST_OS_LINUX) || defined(_HOST_OS_DARWIN))
# include <math.h>
# include <sys/param.h>

# if !(defined(_HOST_OS_IRIX) || defined(_HOST_OS_LINUX) || defined(_HOST_OS_DARWIN)) && defined(_DEBUG)
# include <malloc.h>             /* Needed for malloc_check */
# endif

# endif

