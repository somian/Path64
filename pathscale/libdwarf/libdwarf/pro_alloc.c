/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000,2004 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2.1 of the GNU Lesser General Public License 
  as published by the Free Software Foundation.

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Special thanks goes to SGI for their continued support to open source

  Contact information:  Silicon Graphics, Inc., 1500 Crittenden Lane,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/



#include "config.h"
#include "dwarf_incl.h"
#include <stdlib.h>

/*
	The allocator wants to know which region
	this is to be in so it can allocate the new space
	with respect to the right region.
*/
 /*ARGSUSED*/
    Dwarf_Ptr _dwarf_p_get_alloc(Dwarf_P_Debug dbg, Dwarf_Unsigned size)
{
    void *sp;

    sp = malloc(size);
    memset(sp,0, (int) size);
    return sp;
}


 /*ARGSUSED*/ void
dwarf_p_dealloc(void *space, Dwarf_Unsigned typ)
{
    free(space);
    return;
}


/* Essentially a stub for now. */
 /*ARGSUSED*/ void
_dwarf_p_dealloc(Dwarf_P_Debug dbg, Dwarf_Small * ptr)
{
    dwarf_p_dealloc(ptr, DW_DLA_STRING);
}
