///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007 PathScale, LLC.  All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of version 2 of the GNU General Public License as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it would be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Further, this software is distributed without any warranty that it is
// free of the rightful claim of any third person regarding infringement
// or the like.  Any license provided herein, whether implied or
// otherwise, applies only to this software file.  Patent licenses, if
// any, provided herein do not apply to combinations of this program with
// other software, or any other product whatsoever.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write the Free Software Foundation, Inc., 59
// Temple Place - Suite 330, Boston MA 02111-1307, USA.
///////////////////////////////////////////////////////////////////////////

#if defined(__linux__) || defined(BUILD_OS_DARWIN)

// Work around linux "weak" bug explained in be/com/weak.cxx, so that
// processor specific instruction latencies and other information is
// used from the appropriate .so file.

#include "topcode.h"
#include "ti_si_types.h"

extern const SI * const SI_top_si[];
extern const SI * const * SI_top_si_p;

extern const SI * const SI_ID_si[];
extern const SI * const * SI_ID_si_p;

extern const int SI_ID_count;
extern const int * SI_ID_count_p;

extern const SI_ISSUE_SLOT * const SI_issue_slots[];
extern const SI_ISSUE_SLOT * const * SI_issue_slots_p;

extern const int SI_issue_slot_count;
extern const int * SI_issue_slot_count_p;

extern const SI_RESOURCE * const SI_resources[];
extern const SI_RESOURCE * const * SI_resources_p;

extern const int SI_resource_count;
extern const int * SI_resource_count_p;

extern const SI_RRW SI_RRW_initializer;
extern const SI_RRW * SI_RRW_initializer_p;

extern const SI_RRW SI_RRW_overuse_mask;
extern const SI_RRW * SI_RRW_overuse_mask_p;

extern const char * sanity_check_targ_so_name;
extern const char * sanity_check_targ_so_name_p;

struct TARG_INFO_INIT
{
  TARG_INFO_INIT ()
  {
    SI_top_si_p = SI_top_si;
    SI_ID_si_p = SI_ID_si;
    SI_ID_count_p = &SI_ID_count;
    SI_issue_slots_p = SI_issue_slots;
    SI_issue_slot_count_p = &SI_issue_slot_count;
    SI_resources_p = SI_resources;
    SI_resource_count_p = &SI_resource_count;
    SI_RRW_initializer_p = &SI_RRW_initializer;
    SI_RRW_overuse_mask_p = &SI_RRW_overuse_mask;
    sanity_check_targ_so_name_p = sanity_check_targ_so_name;
  }
} Targ_Info_Initializer;

#endif // __linux__
