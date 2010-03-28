/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

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



#pragma ident "@(#) libf/fio/oconvtab.c	92.2	06/23/99 10:34:31"
#include <cray/dopevec.h>
#include <cray/fmtconv.h>
#include <cray/format.h>
#include <cray/mtlock.h>	/* For SHARED_CC_BUG_WORKAROUND */

/* Local Output Conversion Routines */

extern oc_func	_S2UB;
extern oc_func	_S2UL;
extern oc_func	_S2UO;
extern oc_func	_S2UZ;

/*
 *	_oconvtab[]	Table containing pointers to the output
 *			conversion functions for each data edit-
 *			descriptor, indexed by edit-descriptor.
 */

oc_func * const
_oconvtab[LAST_DATA_ED + 1] = {
	 NULL,		/* none */
	 NULL,		/* A_ED	*/
	_S2UB,		/* B_ED	*/
	_sd2udee,	/* D_ED	*/
	_sd2udee,	/* E_ED	*/
	_sd2uene,	/* EN_ED*/
	_sd2uese,	/* ES_ED*/
	_sd2uf,		/* F_ED	*/
	_sd2uge,	/* G_ED	*/
	_s2ui,		/* I_ED	*/
	_S2UL,		/* L_ED	*/
	_S2UO,		/* O_ED	*/
	 NULL,		/* R_ED */	/* CRI extension */
	_S2UZ,		/* Z_ED	*/
	 NULL		/* Q_ED */	/* MIPSpro extension */
};

/*
 *	_oldotab	Table containing numeric conversion functions,
 *			indexed by data type.  For use with list-directed
 *			output.
 */

oc_func * const
_oldotab[DVTYPE_NTYPES] = {
	NULL,		/* DVTYPE_UNUSED	Unused		*/
	_s2uo,		/* DVTYPE_TYPELESS	Typeless	*/
	_s2ui,		/* DVTYPE_INTEGER	Integer		*/
	_sd2uge,	/* DVTYPE_REAL		Real		*/
	_sd2uge,	/* DVTYPE_COMPLEX	Complex		*/
	NULL,		/* DVTYPE_LOGICAL	Logical		*/
	NULL,		/* DVTYPE_ASCII		Character	*/
	NULL,		/* DVTYPE_DERIVEDBYTE	Unused		*/
	NULL		/* DVTYPE_DERIVEDWORD	Unused		*/
};

/*
 *	_odedtab	Table containing data edit-descriptors for each
 *			data type, indexed by data type.  For use with
 *			the G edit-descriptor.
 */

short const
_odedtab [DVTYPE_NTYPES] = {
	-1,	/* DVTYPE_UNUSED	Unused		*/
	O_ED,	/* DVTYPE_TYPELESS	Typeless	*/
	I_ED,	/* DVTYPE_INTEGER	Integer		*/
	G_ED,	/* DVTYPE_REAL		Real		*/
	G_ED,	/* DVTYPE_COMPLEX	Complex		*/
	L_ED,	/* DVTYPE_LOGICAL	Logical		*/
	A_ED,	/* DVTYPE_ASCII		Character	*/
	-1,	/* DVTYPE_DERIVEDBYTE	Unused		*/
	-1	/* DVTYPE_DERIVEDWORD	Unused		*/
};

/*
 *	_o_sup_flg_tab	Table containing flags indicating whether or not
 *			variables of that type (default KIND) are eligible
 *			to be suppressed in edit-directed formatted output
 *			(i.e., the FSUP or ISUP routines).
 */

short
_o_sup_flg_tab [DVTYPE_NTYPES] = {
	0,	/* DVTYPE_UNUSED	Unused		*/
	0,	/* DVTYPE_TYPELESS	Typeless	*/
	0,	/* DVTYPE_INTEGER	Integer		*/
	0,	/* DVTYPE_REAL		Real		*/
	0,	/* DVTYPE_COMPLEX	Complex		*/
	0,	/* DVTYPE_LOGICAL	Logical		*/
	0,	/* DVTYPE_ASCII		Character	*/
	0,	/* DVTYPE_DERIVEDBYTE	Unused		*/
	0	/* DVTYPE_DERIVEDWORD	Unused		*/
};

/*
 *	_o_sup_val_tab	Table containing values of each type (default KIND)
 *			which are to be suppressed in edit-directed formatted
 *			output.
 */

long
_o_sup_val_tab [DVTYPE_NTYPES] = {
	0,	/* DVTYPE_UNUSED	Unused		*/
	0,	/* DVTYPE_TYPELESS	Typeless	*/
	0,	/* DVTYPE_INTEGER	Integer		*/
	0,	/* DVTYPE_REAL		Real		*/
	0,	/* DVTYPE_COMPLEX	Complex		*/
	0,	/* DVTYPE_LOGICAL	Logical		*/
	0,	/* DVTYPE_ASCII		Character	*/
	0,	/* DVTYPE_DERIVEDBYTE	Unused		*/
	0	/* DVTYPE_DERIVEDWORD	Unused		*/
};

SHARED_CC_BUG_WORKAROUND(_oconvtab_kludge_func)
