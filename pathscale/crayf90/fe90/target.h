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



/* USMID:  "\n@(#)5.0_pl/headers/target.h	5.4	06/09/99 17:21:16\n" */

/* This module is for target specific information.                        */   

enum    target_values  {Target_Unicos,		Target_Max,
			Target_Solaris,		Target_Sunos,
			Target_Acset,		
			Target_Dpe,		Target_Irix,
			Target_Linux,		Target_Sv2,
#ifdef KEY /* Mac port */
			Target_Darwin
#endif /* KEY Mac port */
			};
