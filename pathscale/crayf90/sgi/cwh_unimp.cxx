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


#include <stdio.h>
#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define FAIL () { } 

#define QUOTE(x) x
#define ASSERT_UNIMPLEMENTED 1
#undef ASSERT_UNIMPLEMENTED 
#ifndef ASSERT_UNIMPLEMENTED
#define UNIMPLEMENTED(fname) void fname() {printf("%3d %s\n",__LINE__,# fname);}
#else
#define UNIMPLEMENTED(fname) void fname() {printf("Unimplemented function :: %s \n",# fname); exit(1);}
#endif

#define NOTNEEDED(fname) void fname() {}

/*SGI junk */
#include "defs.h"
#include "errors.h"
#include "err_host.h"
#include "config.h"

void Cleanup_Files FAIL
/* int  Op_Name FAIL */
int  trace_verbose = 0 ;

/* Known to be not needed */
NOTNEEDED(process_v_dbg_flags)
NOTNEEDED(PDGCS_debug_init)
NOTNEEDED(PDGCS_end_comp_unit)
NOTNEEDED(fei_readsm)
/* NOTNEEDED(fei_field_dot) */

/* Not implemented yet */

UNIMPLEMENTED(fei_task_wait)
UNIMPLEMENTED(fei_task_send)
UNIMPLEMENTED(fei_next_symbol)
UNIMPLEMENTED(npex_constant)
UNIMPLEMENTED(npex_to_expr)
UNIMPLEMENTED(fei_loc_cmr)
UNIMPLEMENTED(fei_wmb)
UNIMPLEMENTED(fei_rem_cmr)
UNIMPLEMENTED(fei_ssd_alloc)
UNIMPLEMENTED(fei_mpp_symmetric_alloc)
UNIMPLEMENTED(fei_ssd_free)
UNIMPLEMENTED(fei_mpp_symmetric_free)
UNIMPLEMENTED(fei_copyin)
UNIMPLEMENTED(fei_copyout)
UNIMPLEMENTED(fei_argchk)
UNIMPLEMENTED(fei_argloc)
UNIMPLEMENTED(npex_mult)
UNIMPLEMENTED(npex_div)
UNIMPLEMENTED(npex_plus)
UNIMPLEMENTED(npex_minus)
UNIMPLEMENTED(npex_uminus)
UNIMPLEMENTED(npex_mod)
UNIMPLEMENTED(npex_shiftr)
UNIMPLEMENTED(npex_shiftl)
UNIMPLEMENTED(fei_smt_actual_to_generic)
UNIMPLEMENTED(fei_smt_original_to_qualified)
UNIMPLEMENTED(npex_end)
UNIMPLEMENTED(fei_module_file)
UNIMPLEMENTED(fei_numcpus)

/* Elemental operations */

UNIMPLEMENTED(fei_my_pe)
UNIMPLEMENTED(fei_numargs)
UNIMPLEMENTED(fei_ridiv)

/* Obsolete and unsupported */
UNIMPLEMENTED(fei_i24m)

/* I/O related */
UNIMPLEMENTED(fei_getpos)


/* Cray hardware specific ? */
UNIMPLEMENTED(fei_mbclr)
UNIMPLEMENTED(fei_mbld)
UNIMPLEMENTED(fei_mbmx)
UNIMPLEMENTED(fei_mbmxl)
UNIMPLEMENTED(fei_mbul)

/* Things we don't ever expect to see */
UNIMPLEMENTED(fei_reshape)
UNIMPLEMENTED(fei_dshiftl)
UNIMPLEMENTED(fei_dshiftr)
#ifdef __cplusplus
}
#endif /* __cplusplus */

