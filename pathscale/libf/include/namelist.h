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


/* USMID @(#) libf/include/namelist.h	92.1	06/18/99 10:01:44 */
#ifndef _NAMELIST_H                      /* prevent multiple inclusions */
#define _NAMELIST_H

/*
 * Header file describing the Fortran 90 Namelist structure
 */

#include <fortran.h>
#include <cray/dopevec.h>
#include "f90io.h"

#ifndef NMLIST_VERSION
#define NMLIST_VERSION  1               /* cilist version number */
#endif

/*
 *      A nmlist_scalar describes a scalar namelist group_object_list item.
 *      A dopevector is used for an array namelist group_object_list item.
 *
 *      On some architectures, the fcd is two words.  Therefore, there is
 *	a padding word when there is a noncharacter item.
 */

typedef struct nmlist_scalar {
    f90_type_t		tinfo;		/* type information for variable */
    union {
	void		*ptr;		/* pointer to noncharacter item */
       _fcd		charptr;	/* Fortran character descriptor */
    } scal_addr;
} nmlist_scalar_t;

/*
 *      One nmlist_goli describes one namelist group_object_list_item.
 */
typedef struct nmlist_goli {
    entrycode_t		valtype :8;	/* type of namelist entry */
#if defined(_UNICOS) || defined(__mips) || defined(_LITTLE_ENDIAN)
    unsigned int		:24;	/* pad for first 32 bits	*/
    unsigned int		:32;	/* pad for second 32-bits	*/
#else
    unsigned int		:24;	/* unused */
#endif
    _fcd		goli_name;	/* Fortran character descriptor for */
                                        /* group_object_list_item name. */
    union {
	struct nmlist_scalar	*ptr;	/* ptr to nmlist_scalar struct */
	struct DopeVector 	*dv;	/* ptr to dope vector */
	struct nmlist_struclist *sptr;	/* ptr to structure namelist table */
    } goli_addr;
} nmlist_goli_t;

/*
 *      nmlist_group is the structure of a namelist.   One namelist group
 *      is passed with the single call to a compiler-library interface
 *      routine.
 */

typedef struct {
    unsigned int	version :3;     /* contains NAMELIST_VERSION */
#if defined(_UNICOS) || defined(__mips) || defined(_LITTLE_ENDIAN)
    unsigned int		:29;    /* unused */
    unsigned int		:16;    /* unused */
#else
    unsigned int		:13;    /* unused */
#endif
    unsigned int	icount  :16;    /* Number of group_object_list_items */
                                        /* in the namelist. */
    _fcd		group_name;	/* Fortran character descriptor */
                                        /* for namelist group name */
    struct nmlist_goli	goli[1];	/* at least one or more */
					/* group_object_list_items */
} nmlist_group;

/*
 *      A nmlist_struclist describes a structure namelist group_object_list.
 *      If the structure is a scalar, then a null pointer is the second
 *      word in the structure table.  If the structure is an array, then
 *      the address of the dope vector is in the second word.
 */

typedef struct nmlist_struclist {

#if defined(_UNICOS) || defined(__mips) || defined(_LITTLE_ENDIAN)
    unsigned int		  :32;	/* unused */
    unsigned int		  :16;	/* unused */
#else
    unsigned int		  :16;	/* unused */
#endif
    unsigned int	structlen :16;	/* number of entries in structure. */
    union {
    	struct DopeVector   *dv;	/* ptr to dope vector */
	void		    *v;		/* ptr to byte address */
	_f_int		    *wa;	/* ptr to word address */
    } struc_addr;
    struct nmlist_goli	goli[1];	/* group_object_list_items */
} nmlist_struclist_t;

/*
 *      MIN
 *              Prevent compiler warnings by removing definitions of MIN
 *              previously added by other header files.
 */
#undef MIN
#define MIN(a,b)        ((a) < (b) ? (a) : (b))


extern int _FRN(ControlListType *cilist, nmlist_group *nmlist, void *stck);
extern int _FWN(ControlListType *cilist, nmlist_group *nmlist, void *stck);

#endif /* !_NAMELIST_H */
