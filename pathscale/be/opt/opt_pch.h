//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_pch.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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
//
// ====================================================================
// ====================================================================


// All system headers included directly from OPT

#include <alloca.h>

#include "defs.h"
#include "config_targ.h"
#include "errors.h"
#include "erglob.h"
#include "region_util.h"
#include "symtab.h"
#include "tracing.h"
#include "wn.h"

#include "cxx_base.h"
#include "cxx_memory.h"
#include "cxx_template.h"

#include "opt_defs.h"
#include "opt_array.h"
#include "opt_base.h"
#include "opt_config.h"
#include "opt_htable.h"
#include "opt_sym.h"
#include "opt_util.h"
#include "opt_wn.h"

#include "opt_bb.h"
#include "opt_cfg.h"
#include "opt_points_to.h"
#include "opt_ssa.h"
#include "opt_cvtl_rule.h"
