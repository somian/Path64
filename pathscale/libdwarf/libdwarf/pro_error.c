/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000,2002,2004 Silicon Graphics, Inc.  All Rights Reserved.

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



#include "config.h"
#include "libdwarfdefs.h"
#ifdef HAVE_ELF_H
#include <elf.h>
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include "pro_incl.h"

extern char *_dwarf_errmsgs[];

/* 
    This function performs error handling as described in the 
    libdwarf consumer document section 3.  Dbg is the Dwarf_P_debug
    structure being processed.  Error is a pointer to the pointer
    to the error descriptor that will be returned.  Errval is an
    error code listed in dwarf_error.h.
*/
void
_dwarf_p_error(Dwarf_P_Debug dbg,
	       Dwarf_Error * error, Dwarf_Word errval)
{
    Dwarf_Error errptr;

    /* Allow NULL dbg on entry, since sometimes that can happen and we
       want to report the upper-level error, not this one. */
    if ((Dwarf_Sword) errval < 0)
	printf("ERROR VALUE: %ld - %s\n",
	       (long) errval, _dwarf_errmsgs[-errval - 1]);
    if (error != NULL) {
	errptr = (Dwarf_Error)
	    _dwarf_p_get_alloc(dbg, sizeof(struct Dwarf_Error_s));
	if (errptr == NULL) {
	    fprintf(stderr,
		    "Could not allocate Dwarf_Error structure\n");
	    abort();
	}
	errptr->er_errval = (Dwarf_Sword) errval;
	*error = errptr;
	return;
    }

    if (dbg != NULL && dbg->de_errhand != NULL) {
	errptr = (Dwarf_Error)
	    _dwarf_p_get_alloc(dbg, sizeof(struct Dwarf_Error_s));
	if (errptr == NULL) {
	    fprintf(stderr,
		    "Could not allocate Dwarf_Error structure\n");
	    abort();
	}
	errptr->er_errval = (Dwarf_Sword) errval;
	dbg->de_errhand(errptr, dbg->de_errarg);
	return;
    }

    abort();
}
