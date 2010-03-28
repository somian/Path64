/*
 * Copyright (C) 2007 PathScale, LLC.  All Rights Reserved.
 */
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


#ifdef _KEEP_RCS_ID
static const char source_file[] = __FILE__;
static const char rcs_id[] = "$Source: /home/bos/bk/kpro64-pending/common/targ_info/access/SCCS/s.ti_init.c $ $Revision: 1.5 $";
#endif /* _KEEP_RCS_ID */

#include <alloca.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"
#include "targ_isa_subset.h"
#include "targ_isa_hazards.h"
#include "targ_isa_registers.h"
#include "targ_abi_properties.h"
#include "targ_proc.h"
#include "dso.h"
#include "errors.h"

#include "ti_init.h"

#if defined(__linux__) || defined(BUILD_OS_DARWIN)
const char * sanity_check_targ_so_name_p;
#define sanity_check_targ_so_name sanity_check_targ_so_name_p
#endif

/* ====================================================================
 *
 *  TI_Initialize
 *
 *  See interface description
 *
 * ====================================================================
 */
void
TI_Initialize(ABI_PROPERTIES_ABI tabi, ISA_SUBSET tisa, PROCESSOR tproc, char *tpath)
{
  static BOOL initialized;

  if ( !initialized ) {
    INT                i;
    const char        *targ_name     = PROCESSOR_Name(tproc);
    INT                targ_name_len = strlen(targ_name);
    char              *targ_so_name  = alloca(targ_name_len + sizeof(".so"));

    for (i = 0; i < targ_name_len; i++) {
      targ_so_name[i] = tolower(targ_name[i]);
    }
    strcpy(targ_so_name + targ_name_len, ".so");

    load_so(targ_so_name, tpath, FALSE /*verbose*/);

    ISA_SUBSET_Value = tisa;
    PROCESSOR_Value = tproc;
    ABI_PROPERTIES_ABI_Value = tabi;

    ABI_PROPERTIES_Initialize();
    ISA_HAZARD_Initialize();
    ISA_REGISTER_Initialize();

#if defined(__linux__) || defined(BUILD_OS_DARWIN)
    // For bug 13044, sanity check that we have loaded the proper information.
    FmtAssert (!strcmp(targ_so_name, sanity_check_targ_so_name),
     ("TI_Initialize did not load proper information from %s", targ_so_name));
#endif

    initialized = TRUE;
  }
}

#if defined(__linux__) || defined(BUILD_OS_DARWIN)

#include "ti_si_types.h"

const SI * const * SI_top_si_p;
const SI * const * SI_ID_si_p;
const int * SI_ID_count_p;
const SI_ISSUE_SLOT * const * SI_issue_slots_p;
const int * SI_issue_slot_count_p;
const SI_RESOURCE * const * SI_resources_p;
const int * SI_resource_count_p;
const SI_RRW * SI_RRW_initializer_p;
const SI_RRW * SI_RRW_overuse_mask_p;

#endif
