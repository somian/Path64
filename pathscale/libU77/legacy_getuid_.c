/*
Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.

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
#ifdef KEY /* Bug 1683 */

#include "pathf90_libU_intrin.h"

/* Provide this for backward compatibility, and in case somebody declares
 * it "external" in Fortran but expects to get it from the library instead
 * of defining it themselves.
 */
pathf90_i4 getuid_(void) { return pathf90_getuid(); }

#endif /* KEY Bug 1683 */
