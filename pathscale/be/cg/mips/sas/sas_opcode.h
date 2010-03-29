/*
 * Copyright (C) 2005, 2007 PathScale Inc.  All Rights Reserved.
 *

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

 */

#ifndef _OPCODE_H
#define _OPCODE_H

#include <stdint.h>

typedef enum {
#define NO_OPX
#define op(a,b,c,d,e,f,g) a=b,
#include "sas_op.h"
} STOP;


extern const char* Opcode_Name( const STOP );
extern STOP Find_Opcode( const char* );

extern const uint64_t OPR_ATTR_flags[];
extern const uint8_t OPR_throughput[];

#define ATTR            0x0ULL

#define ATTR_load       0x1ULL
#define OPR_is_load(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_load)
#define ATTR_store      0x2ULL
#define OPR_is_store(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_store)
#define OPR_is_memory(o) ( OPR_is_load(o) || OPR_is_store(o) )
#define ATTR_cfc1       0x4ULL
#define OPR_is_cfc1(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_cfc1)
#define ATTR_cmp        0x8ULL
#define OPR_is_cmp(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_cmp)
#define ATTR_op64       0x10ULL
#define OPR_is_op64(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_op64)
#define ATTR_mf         0x20ULL
#define OPR_is_mf(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_mf)
#define ATTR_mt         0x40ULL
#define OPR_is_mt(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_mt)
#define ATTR_mul        0x80ULL
#define OPR_is_mul(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_mult)
#define ATTR_mult       0x100ULL
#define OPR_is_mult(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_mult)
#define OPR_is_dmult(o)	 ( OPR_is_mult(o) && OPR_is_op64(o) )
#define ATTR_div        0x200ULL
#define OPR_is_div(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_div)
#define OPR_is_ddiv(o)	 ( OPR_is_div(o) && OPR_is_op64(o) )
#define ATTR_madd       0x400ULL
#define OPR_is_madd(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_madd)
#define ATTR_mtc1       0x800ULL
#define OPR_is_mtc1(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_mtc1)
#define ATTR_fdiv       0x1000ULL
#define OPR_is_fdiv(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_fdiv)
#define ATTR_fmult      0x2000ULL
#define OPR_is_fmult(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_fmult)
#define ATTR_xfer       0x4000ULL
#define OPR_is_xfer(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_xfer)
#define ATTR_ndual      0x8000ULL
#define OPR_is_ndual(o)	 (OPR_ATTR_flags[OPR_opcode(o)] & ATTR_ndual)


#endif
