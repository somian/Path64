/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

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
