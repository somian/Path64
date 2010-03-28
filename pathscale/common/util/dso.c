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


#include <alloca.h>
#include <stdio.h>		    /* for fprintf() */
#include <string.h>
#include <dlfcn.h>		    /* for sgidladd(), dlerror() */
#include <cmplrs/rcodes.h>

#include "defs.h"
#include "dso.h"

/*
 * WARNING:  dso is used outside of main mongoose compiler (via targ_info)
 * and should not depend on other mongoose-specific files 
 * (e.g. this is why verbose is a parameter rather than a global variable).
 */

/*
 * Assume that LD_LIBRARY_PATH has already been set up correctly.
 */
void
load_so (const char *soname, char *path, BOOL verbose)
{
    register char *full_path;

    if (path != 0) {
	full_path = (char *) alloca (strlen(path)+strlen(soname)+2);
	strcpy (full_path, path);
	strcat (full_path, "/");
	strcat (full_path, soname);

        if (verbose) {
	    fprintf (stderr, "\nReplacing default %s with %s\n", soname, full_path);
        }
    } else {
        full_path = (char *)soname;
    }

#if ! (defined(linux) || defined(BUILD_OS_DARWIN))
    if (sgidladd (full_path, RTLD_LAZY) == NULL)
#else
    if (dlopen (full_path, RTLD_NOW | RTLD_GLOBAL) == NULL)
#endif
    {
	fprintf (stderr, "Error loading %s: %s\n", full_path, dlerror());
	exit (RC_SYSTEM_ERROR);
    }
} /* load_so */
