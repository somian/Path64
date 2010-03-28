/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


#ifndef ercg_INCLUDED
#define ercg_INCLUDED

/* ====================================================================
 * ====================================================================
 *
 * Module: ercg.h
 * $Revision: 1.3 $
 * $Date: 05/08/30 18:35:56-07:00 $
 * $Author: anmol@iridot.internal.keyresearch.com $
 * $Source: be/cg/SCCS/s.ercg.h $
 *
 * Revision history:
 *  02-Nov-89 - Original Version
 *  01-Feb-91 - Copied for TP/Muse
 *
 * Description:
 *
 * Define the Muse code generator error codes for use with the error
 * message handler errors.c.  The associated error descriptors may be
 * found in the file ercg.desc.
 *
 * ====================================================================
 * ====================================================================
 */


#include "errors.h"	/* Always needed */

/* The error codes in each erxxx.h file should start from some multiple
 * of 1000, which is the phase number.
 */
#define EC_BASE_CG	EP_CG*1000


/* Scheduling Preparation: */
#define EC_Ill_Cycle	EC_BASE_CG+1	/* int, str */

/* Register Allocation: */
#define EC_Ill_Reg_Spill1 EC_BASE_CG+2	/* string(register-name) */
#define EC_Ill_Reg_Spill2b EC_BASE_CG+3 /* int (suggested -O level) */

/* asm() processing related. */
#define EC_ASM_Bad_Operand EC_BASE_CG+4 /* int (line number), 
                                           int (bad operand number), 
                                           string (what was expected) */

/* Start all target-specific codes here: */
#define EC_TARGET	EC_BASE_CG+200

#endif /* ercg_INCLUDED */
