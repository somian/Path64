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



#pragma ident "@(#) libf/fio/globals.c	92.2	06/23/99 16:08:16"

/*
 *	globals.c	This module contains global data and tables with
 *			dynamic data.
 */

#include <fortran.h>
#include "fio.h"
#include <cray/mtlock.h>

/************************************************************************
 *
 *	Shared global data.
 *
 ***********************************************************************/

plock_t	_openlock;			/* Unit tables lock.  Lock this 
					 * whenever connecting or disconnecting
					 * a unit.   This lock must be obtained
					 * before the unit lock to prevent 
					 * deadlock. */
plock_t	_ioblock;			/* __iob table lock	*/
plock_t	_parselock;			/* Format parsing lock	*/
plock_t _stdin_lock;			/* Stdin lock			*/
plock_t _stdout_lock;			/* Stdout lock			*/
plock_t _stderr_lock;			/* Stderr lock			*/
int	_f_rcsz		= RECMAX;	/* Default sequential formatted RECL */
int	_f_ldsz		= RECMAXLDO;	/* Default list-directed output RECL */

unit_htable _fort_unit[HASH_SIZE];	/* hash table of unit pointers */

unit	*_fort_internal_unit;		/* pointer to unit for internal files */

#ifdef	_F_REAL4
int	_dreal4;			/* decimal digits for real (KIND=4) */
#endif
int	_dreal8;			/* decimal digits for real (KIND=8) */
int	_dreal16;			/* decimal digits for real (KIND=16) */

long	_zero_entity	= 0;		/* Used as addr for zero length	*/
					/* entity for PRESENT function	*/


/************************************************************************
 *
 *	Private global data.
 *
 *	The _tsk_fiostate structure is taskcommon, so the C compiler kindly
 *	replicates it for every active task.  
 *
 ***********************************************************************/
#ifdef	_CRAY1
#pragma _CRI taskcommon _tsk_fiostate
#endif
struct fiostate _tsk_fiostate;

SHARED_CC_BUG_WORKAROUND(_globals_kludge_func)
