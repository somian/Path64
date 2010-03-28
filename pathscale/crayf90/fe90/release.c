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



static char USMID[] = "@(#)5.0_pl/sources/release.c	5.16	10/22/99 08:51:40";

# include "defines.h"          /* Machine dependent ifdefs */

# include "globals.m"
#include "stamp.h"

# if defined(_TARGET_OS_LINUX) || defined(_TARGET_OS_DARWIN)

char	release_level[RELEASE_LEVEL_LEN] = INCLUDE_STAMP; 

# elif defined(_TARGET_OS_IRIX)

char	release_level[RELEASE_LEVEL_LEN] = INCLUDE_STAMP; 

# elif ! defined(_TARGET_OS_SOLARIS)

char	release_level[RELEASE_LEVEL_LEN] = "5.x.x.x "; 

# endif


char	frontend_version[3]	   = "14";
