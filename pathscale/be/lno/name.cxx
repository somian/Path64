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


#ifdef USE_PCH
#include "lno_pch.h"
#endif // USE_PCH
#pragma hdrstop

#include "name.h"
#include "ff_utils.h"
#include "access_vector.h"
#include "lnoutils.h"

static BOOL operator<(const SYMBOL& symb1, const SYMBOL& symb2) {
  
    if (symb2.St() == NULL) return(FALSE);
    if (symb1.St() == NULL) return(TRUE);
    if (symb1.ST_Base() < symb2.ST_Base()) return(TRUE);
    else if (symb1.ST_Base() > symb2.ST_Base()) return(FALSE);
    else if (symb1.ST_Offset() < symb2.ST_Offset()) return(TRUE);
    else if (symb1.ST_Offset() > symb2.ST_Offset()) return(FALSE);
    else if (symb1.WN_Offset() < symb2.WN_Offset()) return(TRUE);
    else return(FALSE);

}

static BOOL operator<(const ACCESS_VECTOR& a1, const ACCESS_VECTOR& a2)
{
  // TODO, support non linear symbols when they're implemented
  // note that the answer is true for both a1<a2 and a2<a1 if
  // both a1 and a2 are Too_Messy or having Non_Lin_Symb
  if (a1.Too_Messy || a1.Non_Lin_Symb)
      return TRUE;
  if (a2.Too_Messy || a2.Non_Lin_Symb)
      return FALSE;

  if (a2.Too_Messy || a2.Non_Lin_Symb) {
    return FALSE;
  }

  if (a1.Const_Offset < a2.Const_Offset) {
    return TRUE;
  } else if (a2.Const_Offset < a1.Const_Offset) {
    return FALSE;
  }
  if (a1.Nest_Depth() != a2.Nest_Depth()) {
    return (a1.Nest_Depth() < a2.Nest_Depth());
  }
  for (INT i=0; i<a1.Nest_Depth(); i++) {
    if (a1.Loop_Coeff(i) < a2.Loop_Coeff(i)) {
      return TRUE;
    } else if (a2.Loop_Coeff(i) < a1.Loop_Coeff(i)) {
      return FALSE;
    }
  }

  if (a1.Lin_Symb==NULL && a2.Lin_Symb==NULL)
    return FALSE;

  INTSYMB_ITER iter1(a1.Lin_Symb);
  INTSYMB_ITER iter2(a2.Lin_Symb);
  INTSYMB_NODE *node1,*node2;
  for (node1=iter1.First(),node2 = iter2.First();
    !iter1.Is_Empty() && !iter2.Is_Empty();
    node1=iter1.Next(),node2=iter2.Next()) {
    if (node1->Symbol<node2->Symbol) {
      return TRUE;
    } else if (node1->Symbol<node2->Symbol) {
      return FALSE;
    }
    if (node1->Coeff < node2->Coeff) {
      return TRUE;
    } else if (node2->Coeff < node1->Coeff) {
      return FALSE;
    }
  }
  if (!iter2.Is_Empty()) return TRUE;
  else return FALSE;
}

static BOOL operator<(const ACCESS_ARRAY& a1, const ACCESS_ARRAY& a2)
{
  if (a1.Too_Messy) { 
    if (a2.Too_Messy) {
      return (&a1 < &a2);
    } else {
      return TRUE;
    }
  }
  if (a2.Too_Messy) {
    return FALSE;
  }
  if (a1.Num_Vec() != a2.Num_Vec()) {
    return (a1.Num_Vec() < a2.Num_Vec());
  }
  for (INT i=0; i<a1.Num_Vec(); i++) {
    if (*(a1.Dim(i))< *(a2.Dim(i))) {
      return TRUE;
    } else if (*(a2.Dim(i))< *(a1.Dim(i))) {
      return FALSE;
    } 
  }
  return FALSE;
}

BOOL NAME2BIT::operator <(const NAME2BIT &name2bit) const {
  
    if (_symbol<name2bit._symbol) return(TRUE);
    else if (name2bit._symbol<_symbol) return(FALSE);

    // now symbols are equivalent, check the access_arrays
    if (name2bit._access_array == NULL) return(FALSE);
    else if (_access_array == NULL) return(TRUE);
    else if ( *_access_array == *name2bit._access_array) return(FALSE);
    else if ( *_access_array < *name2bit._access_array) return(TRUE);
    else return(FALSE);
}

BOOL NAME2BIT::operator ==(const NAME2BIT &name2bit) const {
    if (_symbol != name2bit._symbol)
      return(FALSE);
    else if (_access_array==NULL && name2bit._access_array==NULL)
      return(TRUE);
    else if (_access_array!=NULL && name2bit._access_array!=NULL)
      return(*_access_array == *name2bit._access_array);
    else return (FALSE);
}

