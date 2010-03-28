/*
 * Copyright (C) 2007 Pathscale, LLC.  All Rights Reserved.
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
 * Module: config_targ_opt.h
 * $Revision: 1.5 $
 * $Date: 05/03/02 15:28:06-08:00 $
 * $Author: bos@eng-24.internal.keyresearch.com $
 * $Source: common/com/mips/SCCS/s.config_targ_opt.h $
 *
 * Revision history:
 *  11-Apr-96 - Original Version
 *
 * Description:
 *
 * External definitions for the -TARG group.
 *
 * Some of these variables are also defined in config.h or
 * MIPS/config_targ.h, for historical reasons.  In order to separate
 * the headers and minimize dependencies on changes to this one, new
 * group members should be defined only here, and their users should
 * explicitly include this file instead of having it indirectly
 * included (e.g. via config.h or MIPS/config_targ.h).  We should also
 * work towards removing most of the definitions from those headers.
 *
 * Exported variables should have names prefixed by "TARG_" to
 * facilitate moving them to a pushable struct later if desired.
 * See config_debug.[hc] for an example.
 *
 * ====================================================================
 * ====================================================================
 */

/* This file was named as config_TARG.h.
 * Change it into config_targ_opt.h because windows can not distinguish
 * between "config_targ.h" and "config_TARG.h". */

#ifndef config_TARG_INCLUDED
#define config_TARG_INCLUDED

#ifdef _KEEP_RCS_ID
/*REFERENCED*/
static char *config_TARG_h_rcs_id = "$Source: common/com/mips/SCCS/s.config_targ_opt.h $ $Revision: 1.5 $";
#endif /* _KEEP_RCS_ID */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ====================================================================
 * List of global variables that are set by the -TARG option group
 * These appear only here, requiring explicit inclusion of this file.
 * ====================================================================
 */

/* General target control: */
extern char *ABI_Name;		/* -TARG:abi=xxx */
extern char *Processor_Name;	/* -TARG:processor=xxx */
extern char *ISA_Name;		/* -TARG:isa=xxx */
extern INT16 Target_FPRs;	/* Number of target FP registers */
extern BOOL Pure_ABI;		/* Avoid non-ABI constructs? */

/* Fault handling: */
extern BOOL Force_FP_Precise_Mode;	/* Force precise FP traps? */
extern BOOL Force_Memory_Dismiss;	/* Force mem fault dismissal? */
extern BOOL Force_Page_Zero;		/* Force mapping page zero? */
extern BOOL Force_SMM;			/* Force sequential memory? */
extern char *FP_Excp_Max;		/* Max FP trap enables */
extern char *FP_Excp_Min;		/* Min FP trap enables */

/* Force calls to be indirect (i.e. use address in register)? */
extern BOOL Force_Jalr;

/* Miscellaneous target instruction features: */
extern BOOL Madd_Allowed;	/* Generate madd instructions? */
extern BOOL SYNC_Allowed;
extern BOOL Slow_CVTDL;
extern BOOL Sext_Allowed;	/* Use sll r2,r1,0 for 32-bit sign extend */
extern char *Endian_Name;	/* -TARG:endian=xxx */

/* Itanium options: */
extern BOOL Itanium_a0_step;	/* a0 version of itanium chip */

/* ====================================================================
 * List of global variables that are set by the -TARG option group
 * These also appear in config_targ.h, and are implicitly included by
 * including config.h.  They should be removed from there when it is
 * convenient.
 * ====================================================================
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /* config_TARG_INCLUDED */
