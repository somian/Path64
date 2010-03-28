/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


/* USMID @(#) libu/ffio/locklyr.h	92.1	10/07/99 22:14:06 */

#include <cray/portdefs.h>
#include <cray/mtlock.h>
#include "fflock.h"


#ifdef KEY /* Bug 6003 */
#define LYR_LOCK(_x)	MEM_LOCK((_x)->lyr_info);
#define LYR_UNLOCK(_x)	MEM_UNLOCK((_x)->lyr_info);
#define COND_LYR_UNLOCK(_x)	{if ((_x)->lyr_info != NULL) LYR_UNLOCK(_x) }
#else
#if	defined(__mips) || defined(_LITTLE_ENDIAN)
	/* mips only handles reg_lock now */
#define LYR_LOCK(_x)	MEM_LOCK((unsigned long *)_x->lyr_info);
#define LYR_UNLOCK(_x)	MEM_UNLOCK((unsigned long *)_x->lyr_info);
#define COND_LYR_UNLOCK(_x)	{if (_x->lyr_info != NULL) LYR_UNLOCK(_x) }
#else
#define LYR_LOCK(_x)	{if (_x->reg_lock) \
				{LMEM_LOCK(_x->lyr_info)}\
			 else\
				_nlockon(_x->lyr_info);\
			}

#define LYR_UNLOCK(_x)	{if (_x->reg_lock) \
				{LMEM_UNLOCK(_x->lyr_info)}\
			else\
				_nlockoff(_x->lyr_info);\
			}
#define COND_LYR_UNLOCK(_x)	{if (_x->lyr_info != NULL) LYR_UNLOCK(_x) }
#endif
#endif /* Key Bug 6003 */
