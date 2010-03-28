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


/* =======================================================================
 * =======================================================================
 *
 *  Module: x_list.c
 *  $Revision$
 *  $Date$
 *  $Author$
 *  $Source$
 *
 *  Revision comments:
 *
 *  13-Dec-1991 - Initial version
 *  27-May-1993 - Changed to MEM_POOL style
 *
 *  Description:
 *  ============
 *
 *  Generalised lisp implementation (Common lisp style.)
 *
 * =======================================================================
 * =======================================================================
 */

static const char *source_file = __FILE__;
static const char *rcs_id = "$Source$ $Revision$";

#define _X_first_(x)    ((x)->first)
#define _X_rest_(x)     ((x)->rest)

/* =======================================================================
 *
 *  _X_PUSH_
 *
 *  See interface description.
 *
 * =======================================================================
 */
_X_LIST_TYPE_ *
_X_PUSH_(
  _X_LIST_LOCAL_BASE_TYPE_  x,
  _X_LIST_TYPE_            *list,
  MEM_POOL                 *pool
)
{
  _X_LIST_TYPE_ *result = TYPE_MEM_POOL_ALLOC(_X_LIST_TYPE_,pool);

  _X_first_(result) = x;
  _X_rest_(result) = list;
  return result;
}


/* =======================================================================
 *
 *  _X_DELETE_
 *
 *  See interface description.
 *
 * =======================================================================
 */
_X_LIST_TYPE_ *
_X_DELETE_(
    _X_LIST_LOCAL_BASE_TYPE_ x,
    _X_LIST_TYPE_           *xl
)
{
  /* Oh, for a little tail recursion...
   */

  _X_LIST_TYPE_  sentinal;
  _X_LIST_TYPE_ *l;
  _X_LIST_TYPE_ *last = &sentinal;

  _X_rest_(last) = NULL;
  for ( l = xl; l; l = _X_rest_(l) ) {
    if (x != _X_first_(l)) {
      _X_rest_(last) = l;
      last = l;
    }
  }
  _X_rest_(last) = NULL;
  return _X_rest_(&sentinal);
}
