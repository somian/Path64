/*
 * Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
 */

/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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
 * Module: targ_const_private.h
 * $Revision: 1.5 $
 * $Date: 05/02/25 12:23:10-08:00 $
 * $Author: kannann@iridot.keyresearch $
 * $Source: common/com/x8664/SCCS/s.targ_const_private.h $
 *
 * Revision history:
 *  10-Feb-95 - Original Version
 *
 * Description:
 *
 * Field access macros for type TCON
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef targ_const_private_INCLUDED
#define targ_const_private_INCLUDED

#undef  TCON_ty
#define TCON_ty(c)	((c).ty)

#define TCON_v0(c)	((c).vals.ival.v0)
#define TCON_v1(c)	((c).vals.ival.v1)
#define TCON_v2(c)	((c).vals.ival.v2)
#define TCON_v3(c)	((c).vals.ival.v3)
#define TCON_iv0(c)	((c).cmplxval.ival.v0)
#define TCON_iv1(c)	((c).cmplxval.ival.v1)
#define TCON_iv2(c)	((c).cmplxval.ival.v2)
#define TCON_iv3(c)	((c).cmplxval.ival.v3)
#define TCON_word0(c)	((c).vals.word0)
#define TCON_u0(c)	((c).vals.uval.u0)
#define TCON_u1(c)	((c).vals.uval.u1)
#define TCON_u2(c)	((c).vals.uval.u2)
#define TCON_u3(c)	((c).vals.uval.u3)
#define TCON_I4(c)	TCON_v0(c)
#define TCON_U4(c)	TCON_u0(c)
#define TCON_I8(c)	((c).vals.i0)
#define TCON_U8(c)	((c).vals.k0)
#define TCON_ll0(c)     ((c).vals.llval.ll0)
#define TCON_ll1(c)     ((c).vals.llval.ll1)

/* special TCON accessors for simplifier:
	I8I4CVT and U8I4CVT are nops for MIPS III and above and are removed.
	It is possible for kids to have a types I8, U8, I4, U4.	*/
#define TCON_I8U8I4U4(c)   (MTYPE_is_signed(TCON_ty(c)) ? \
		    ((TCON_ty(c) == MTYPE_I8) ? TCON_I8(c) : TCON_I4(c)) : \
		    ((TCON_ty(c) == MTYPE_U8) ? TCON_U8(c) : TCON_U4(c)))

#define TCON_R8(c)	((c).vals.dval)
#define TCON_R16(c)	((c).vals.qval)
#define TCON_F16(c)	((c).vals.f16val)

#define TCON_IR8(c)	((c).cmplxval.dival)
#define TCON_IR16(c)	((c).cmplxval.qival)
#define TCON_IF16(c)	((c).cmplxval.f16ival)
/* The internal representation of 32-bit floats may be double: */
#ifdef TCON_R4_IS_DOUBLE
#define TCON_R4(c)	TCON_R8(c)
#define TCON_IR4(c)	TCON_IR8(c)
#else
#define TCON_R4(c)	((c).vals.fval)
#define TCON_IR4(c)	((c).cmplxval.fival)
#endif
/* Complex imaginary parts */



#define Set_TCON_R4(c,v)	(TCON_R4(c) = v)
#define Set_TCON_R8(c,v)	(TCON_R8(c) = v)
#define Set_TCON_R16(c,v)	(TCON_R16(c) = v)
#define Set_TCON_F16(c,v)	(TCON_F16(c) = v)
#define Set_TCON_IR4(c,v)	(TCON_IR4(c) = v)
#define Set_TCON_IR8(c,v)	(TCON_IR8(c) = v)
#define Set_TCON_IR16(c,v)	(TCON_IR16(c) = v)
#define Set_TCON_IF16(c,v)	(TCON_IF16(c) = v)
/* WARNING:  We haven't carefully checked the implications of using
 * doubles for 32-bit floats -- must be done if turned on.
 */

#define TCON_cp(c)	((c).vals.sval.cp)
#define TCON_len(c)	((c).vals.sval.len)

#ifdef __cplusplus
extern "C" {
#endif
extern char *Targ_Append_To_Dbuf (char *, char);
#ifdef __cplusplus
}
#endif

#endif /* targ_const_private_INCLUDED */
