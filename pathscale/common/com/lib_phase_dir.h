/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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

/* 
 * PHASEPATH:    Location for *NON* general purpose, non GNU phases(e.g be).
 * GNUPHASEPATH: Location for *NON* general purpose, GNU phases(e.g collect2)
 *
 * BINPATH:      Location for general purpose phases (e.g m4) 
 * ALTBINPATH:   Another location for general purpose phases (e.g m4 of NUE)
 *
 * LIBPATH:      Location for compiler's specific lib.
 * ALTLIBPATH:   Specify the location where native lib reside.
 */

#ifndef lib_phase_dir_INCLUDED
#define lib_phase_dir_INCLUDED

#include <stamp.h>

#include "pathscale_defs.h"
/* TODO : This really should be a --prefix option during configure */
#if defined(VENDOR_PSC)
/*
#define NAMEPREFIX	""
#define BINPATH		PSC_INSTALL_PREFIX "/bin"
#define ALTBINPATH	BINPATH
#define LIBPATH		PSC_INSTALL_PREFIX "/lib"
#define ALTLIBPATH	LIBPATH
#define PHASEPATH	PSC_INSTALL_PREFIX "/lib"
#define GNUPHASEPATH	PSC_INSTALL_PREFIX "/lib/gcc-lib/" PSC_TARGET "/" PSC_GCC_VERSION
*/
/*
#define PHASEPATH	"/usr/lib/" PSC_NAME_PREFIX "cc-lib"
#define GNUPHASEPATH    PHASEPATH
#define ALTBINPATH	BINPATH
#define ALTLIBPATH	LIBPATH
*/
#else /*defined(VENDOR_XXX)*/
/*
#define NAMEPREFIX	""
#define BINPATH		"/usr/bin"
#define LIBPATH		"/usr/lib"
#define ALTBINPATH	BINPATH
#define ALTLIBPATH	LIBPATH
*/
#endif /*defined(VENDOR_XXX)*/

#endif /* lib_phase_dir_INCLUDED */
