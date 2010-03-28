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


#ifdef __cplusplus
extern "C" {
#endif


/* =======================================================================
 * =======================================================================
 *
 *  Module: x_list.h
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Revision comments:
 *
 *  -2-Mar-1992 - Initial version
 *
 *  Description:
 *  ============
 *
 *      Template for generating externally linked list modules.  I've tried
 *      to use Common Lisp like names for the various functions so I didn't
 *      have to invent any names.  The list of supported functions is very
 *      incomplete currently, but it can grow as required.  (It will have
 *      to grow a good bit to take on the BB lists.)
 *  
 *  Instructions for use:
 *
 *      This file is only a template.  See MTP_ROOT/bin/gen_x_list for
 *      instructions on how to create instantiations based on specific
 *      types.
 *  
 *  Prefixes:     X_LIST  -- for type specific lists of Xs
 *      
 *  Interface Description:
 *
 *      Exported types:
 *
 *          typedef struct x_list X_LIST
 *
 *              The "con" cell of an X_LIST.
 *
 *              A structure with the following fields:
 *
 *                  XT  first
 *
 *                      First XT in the list.
 *
 *                  X_LIST *rest
 *
 *                      Rest of the list.
 *
 *      Exported functions:
 *
 *          X_LIST *X_LIST_Push(
 *              XT        first,
 *              X_LIST   *rest,
 *              MEM_POOL *pool
 *              
 *          )
 *
 *              Return a list consisting of 'first' . 'rest'.  Memory
 *              allocated in the given 'pool'.
 *
 *	    X_LIST *X_LIST_Delete(
 *		XT	x,
 *		X_LIST	*list
 *	    )
 *
 *		Destructively remove element <x> from the list. The result
 *		of this function must be used, since the argument 'list'
 *		is no longer valid after this function is called.
 *
 *
 * =======================================================================
 * =======================================================================
 */

typedef struct _X_LIST_TAG_ _X_LIST_TYPE_;

typedef _X_BASE_TYPE_ _X_LIST_LOCAL_BASE_TYPE_;

struct _X_LIST_TAG_ {
  _X_LIST_LOCAL_BASE_TYPE_      first;
  _X_LIST_TYPE_                *rest;
};

extern _X_LIST_TYPE_ *
_X_PUSH_(
  _X_LIST_LOCAL_BASE_TYPE_  x,
  _X_LIST_TYPE_            *xl,
  MEM_POOL                 *pool
);

extern _X_LIST_TYPE_ *
_X_DELETE_(
  _X_LIST_LOCAL_BASE_TYPE_ x,
  _X_LIST_TYPE_	          *xl
);

#ifdef __cplusplus
}
#endif
