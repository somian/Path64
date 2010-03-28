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


#include "defs.h"
#include "erglob.h"
#include "err_host.h"
#include "tracing.h"
#include "config_targ.h"
#include "mempool.h"
#include "mempool_allocator.h"
#include "config.h"
#include "opcode.h"
#include "symtab.h"
#include "strtab.h"
#include "stab.h"


#include "targ_sim.h"

#include "config_host.h"
#include "irbdata.h"
#include "irbdata_defs.h"
#include "language.h"
#include "srcpos.h"
#include "const.h"
#include "targ_const.h"
#include "wintrinsic.h"
#include "wio.h"
#include "wn.h"
#include "wn_map.h"
#include "wn_pragmas.h"
#include "wn_simp.h"
#include "wutil.h"
#include "cxx_memory.h"
#include "errors.h"
