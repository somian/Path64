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


#ifdef _KEEP_RCS_ID
static const char source_file[] = __FILE__;
static const char rcs_id[] = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

#include "ti_errors.h"

#if defined(BUILD_OS_DARWIN)
/* Put this in data section, not common, because Mach-O linker won't import
 * a common symbol from a .a archive to satisfy an undefined symbol in a .o */
char TI_errmsg[TI_ERRMSG_BUFLEN] = { 0 };
#else /* defined(BUILD_OS_DARWIN) */
char TI_errmsg[TI_ERRMSG_BUFLEN];
#endif /* defined(BUILD_OS_DARWIN) */
