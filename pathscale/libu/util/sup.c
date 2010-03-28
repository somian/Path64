/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

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


#pragma ident "@(#) libu/util/sup.c	92.1	07/07/99 13:18:33"

#include <fortran.h>
#include <cray/dopevec.h>

extern	short	_o_sup_flg_tab[DVTYPE_NTYPES];
extern	long	_o_sup_val_tab[DVTYPE_NTYPES];

#define	F_INDX	DVTYPE_REAL
#define	I_INDX	DVTYPE_INTEGER

void
FSUP(long *fvalue)
{
	_o_sup_val_tab[F_INDX]	= *fvalue;
	_o_sup_flg_tab[F_INDX]	= 1;

	return;
}

void
FSUPC(void)
{
	_o_sup_flg_tab[F_INDX]	= 0;

	return;
}

void
ISUP(long *ivalue)
{
	_o_sup_val_tab[I_INDX]	= *ivalue;
	_o_sup_flg_tab[I_INDX]	= 1;

	return;
}

void
ISUPC(void)
{
	_o_sup_flg_tab[I_INDX]	= 0;

	return;
}
