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


/* ====================================================================
 * ====================================================================
 *
 *  Module: ti_init.h
 *  $Revision: 1.5 $
 *  $Date: 04/12/21 14:57:25-08:00 $
 *  $Author: bos@eng-25.internal.keyresearch.com $
 *  $Source: /home/bos/bk/kpro64-pending/common/targ_info/access/SCCS/s.ti_init.h $
 *
 *  Synopsis:
 *
 *	Initialize the targ-info package.
 *
 *  Interface Description:
 *
 *	Misc. functions:
 *
 *	    void TI_Initialize( ABI_PROPERTIES_ABI, ISA_SUBSET, PROCESSOR, target-path)
 *
 *		Initialize the package.  Needs to be called once per run
 *		of the backend, before any other operations in the
 *		targ-info package are used.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ti_init_INCLUDED
#define ti_init_INCLUDED

#ifdef _KEEP_RCS_ID
static const char ti_init_rcs_id[] = "$Source: /home/bos/bk/kpro64-pending/common/targ_info/access/SCCS/s.ti_init.h $ $Revision: 1.5 $";
#endif /* _KEEP_RCS_ID */

#include "targ_abi_properties.h"
#include "targ_isa_subset.h"
#include "targ_proc.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void
TI_Initialize (ABI_PROPERTIES_ABI, ISA_SUBSET, PROCESSOR, char *tpath);

#ifdef __cplusplus
}
#endif
#endif /* ti_init_INCLUDED */
