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



static char USMID[] = "\n@(#)5.0_pl/sources/mpp_main.c	5.1	04/29/99 21:22:31\n";

# include "defines.h"           /* Machine dependent ifdefs */

# ifdef _TARGET_OS_MAX
extern void the_real_main (int, char **);

void main (int  argc,
           char *argv[])
{

     the_real_main (argc, argv);
}
# endif
