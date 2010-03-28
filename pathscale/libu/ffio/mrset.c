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


#pragma ident "@(#) libu/ffio/mrset.c	92.1	06/29/99 13:16:47"


#include <ffio.h>
#include <cray/mtlock.h>
#include "fssio.h"
#include <memory.h>

/*
 * _mr_locsw_set(mr_info, usersw, count) finds an unused entry in the
 *	structure describing outstanding asynchronous I/O
 *	requests, and initializes its values.
 * Parameters:  usersw - address of the status word passed in from the
 *			upper level. 
 *		count - number of BITS already completed for this request.
 * 
 * Returns: pointer to the _lociosw structure to be used.
 *          NULL if error occurred
 */
_lociosw *
_mr_locsw_set(
	struct mr_f *mr_info, 
	struct ffsw *usersw, 
	int count)
{
_lociosw *locptr;
int i;
struct _loclink *loclink;

	MEM_LOCK(&(mr_info->locsw_lock)); 
	if (mr_info->loclist == NULL) {
		mr_info->loclist = (struct _loclink *)calloc(1, sizeof(struct _loclink)); 
		if (mr_info->loclist == NULL)   {
			goto nomem;
		} 
	} 
	loclink = mr_info->loclist;
	locptr = loclink->loc_first;
	for (;;) {
		for (i = 0; i < _FSSASYNUM ; i++) {
       			if (locptr->user_sw == NULL) {
				goto found ;
			}
			else
				locptr++;
		}
		if (loclink->loc_nxt == NULL) {
			loclink->loc_nxt =  (struct _loclink *)calloc(1, sizeof(struct _loclink));
			if (loclink->loc_nxt == NULL)   {
				goto nomem;
			}
		}
		loclink = loclink->loc_nxt;
		locptr = loclink->loc_first;
	}
found:
	locptr->user_sw = usersw;
	locptr->sw_count = count;
	CLRFFSTAT(*usersw);
	memset(&locptr->local_sw , 0, sizeof(struct ffsw));
#if     defined(_ADDR64) || defined(__mips)
	usersw->sw_sptr = (void *)locptr;
#else
	usersw->sw_sptr = (int)locptr;
#endif
	MEM_UNLOCK(&(mr_info->locsw_lock));
	return(locptr);
nomem:
	MEM_UNLOCK(&(mr_info->locsw_lock));
	return(NULL);
}

/*
 * Clear the entry pointed to by locptr.
 * The calling routine is responsible for locking.
 */
void
_mr_locsw_clear(
	struct mr_f *mr_info,
	 _lociosw *locptr)
{
	locptr->user_sw = 0;
	locptr->sw_count = 0;
	memset(&(locptr->local_sw), 0, sizeof(struct ffsw));
	return;
}
