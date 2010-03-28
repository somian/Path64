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


// -*-C++-*-

/**
*** The x in this file's name indicates that it's private to SNL.
*** None of this stuff can be seen even by those who include "snl.h".
*** It contains the bounds as summarized by a system of equations,
*** and some conditions that need to be true to make the nest transformable.
*** What all that is, exactly, you don't need to know.
**/

/** $Revision$
*** $Date$
*** $Author$
*** $Source$
**/

#ifndef snl_xbounds_INCLUDED
#define snl_xbounds_INCLUDED "snl_xbounds.h"

#ifndef soe_INCLUDED
#include "soe.h"
#endif

#ifdef _KEEP_RCS_ID
static char *snl_xbounds_rcs_id = snl_xbounds_INCLUDED "$Revision$";
#endif /* _KEEP_RCS_ID */

// Portions of these classes are public, but see the interface file for
// what you are really supposed to see.

//-----------------------------------------------------------------------
// SNL_BOUNDS_SYMBOL_LIST
//-----------------------------------------------------------------------

// a list of symbols multiplied together
class SNL_BOUNDS_SYMBOL_NODE : public SLIST_NODE {
  DECLARE_SLIST_NODE_CLASS(SNL_BOUNDS_SYMBOL_NODE);
 public:
  SYMBOL		Symbol;
  WN*			Alias_Wn;       // a read of this symbol

  void Print(FILE *fp) const;
  SNL_BOUNDS_SYMBOL_NODE(const SNL_BOUNDS_SYMBOL_NODE *sl) :
    Symbol(sl->Symbol), Alias_Wn(sl->Alias_Wn) {}
  SNL_BOUNDS_SYMBOL_NODE(const SYMBOL& symbol, WN* alias_wn)  :
    Symbol(symbol), Alias_Wn(alias_wn) {}
  ~SNL_BOUNDS_SYMBOL_NODE() {};
};

class SNL_BOUNDS_SYMBOL_LIST : public SLIST {
  DECLARE_SLIST_CLASS(SNL_BOUNDS_SYMBOL_LIST, SNL_BOUNDS_SYMBOL_NODE);
  MEM_POOL* _pool;
 public:
  SNL_BOUNDS_SYMBOL_LIST(MEM_POOL* pool) : _pool(pool), SLIST() {}
  void Init(const SNL_BOUNDS_SYMBOL_LIST *sl);
  void Print(FILE *fp) const;
  ~SNL_BOUNDS_SYMBOL_LIST();
};

class SNL_BOUNDS_SYMBOL_ITER:public SLIST_ITER {
  DECLARE_SLIST_ITER_CLASS(SNL_BOUNDS_SYMBOL_ITER, SNL_BOUNDS_SYMBOL_NODE, SNL_BOUNDS_SYMBOL_LIST )
 public:
  ~SNL_BOUNDS_SYMBOL_ITER() {};
};

class SNL_BOUNDS_SYMBOL_CONST_ITER : public SLIST_ITER {
  DECLARE_SLIST_CONST_ITER_CLASS(SNL_BOUNDS_SYMBOL_CONST_ITER, SNL_BOUNDS_SYMBOL_NODE, SNL_BOUNDS_SYMBOL_LIST);
 public:
  ~SNL_BOUNDS_SYMBOL_CONST_ITER() {};
};

//-----------------------------------------------------------------------
// SNL_BOUNDS_INFO
//-----------------------------------------------------------------------


class SNL_BOUNDS_INFO {

 public:

  SNL_BOUNDS_INFO(MEM_POOL*);
  SNL_BOUNDS_INFO(const SNL_BOUNDS_INFO*, MEM_POOL* =0);

  ~SNL_BOUNDS_INFO();

  void				Collect_Outer_Info(WN* wn);
  void				Collect_Do_Info(WN* wn_do);
  void				Collect_If_Info(WN* wn_if, BOOL);
  void				Collect_DoWhile_Info(WN* wn_dowhile);
  void				Collect_WhileDo_Info(WN* wn_whiledo);

  // all information known about the bounds.  This includes the outer
  // loops and conditionals.  All facts.  This is correct, but
  // does row_echelon generate unnecessary extra code because of the
  // extra facts?  No, since it projects.  OK.
  SYSTEM_OF_EQUATIONS&		Bounds() {return _bounds;}
  const SYSTEM_OF_EQUATIONS&	Bounds() const {return _bounds;}

  // the conditionals, what additional assumptions are required to make
  // the nest transformable (e.g. N>0).
  SYSTEM_OF_EQUATIONS&		Conditionals() {return _conditionals;}
  const SYSTEM_OF_EQUATIONS&	Conditionals() const {return _conditionals;}

  // note that ST* is NULL for loop variables, offset is nest depth,
  // where 0 is very outermost (not just in this nest but in the function.
  // After Canonicize, indices from Outermost_Depth() are in the front of
  // the list, and therefore the first columns as Bounds() and
  // Conditionals().
  SNL_BOUNDS_SYMBOL_LIST&	Var_Info() {return _var_info;}
  const SNL_BOUNDS_SYMBOL_LIST&	Var_Info() const {return _var_info;}

  MEM_POOL*			Pool() const {return _pool;}

  void		Canonicize(INT depth, DOLOOP_STACK* stack, INT stk_first);

  // Given the access array or vector, add the equations expressed.
  // The second parameter is whether to add to the conditionals as well.
  // These return the number of added equations.

  INT		Add_Access(ACCESS_VECTOR* av, BOOL);
  INT		Add_Access(ACCESS_ARRAY* ai, BOOL);

  void		Reset_Bounds_To(INT, INT, INT);
  void		Reset_Conditionals_To(INT, INT, INT);

  INT		Outermost_Depth() const {return _outermost_depth;}
  INT&		Outermost_Depth() {return _outermost_depth;}

  void		Exclude_Outer_Loops(INT how_many);

  void		Print(FILE*) const;

 private:

  //Add_Access helper function
  INT		Lookup_Entry(SYMBOL, WN*);

  void Reset_Varcount_To(INT cols);

  // undefined
  SNL_BOUNDS_INFO&		operator = (const SNL_BOUNDS_INFO&);
  SNL_BOUNDS_INFO();
  SNL_BOUNDS_INFO(const SNL_BOUNDS_INFO&);

  INT				_outermost_depth;
  SYSTEM_OF_EQUATIONS		_bounds;
  SYSTEM_OF_EQUATIONS		_conditionals;
  SNL_BOUNDS_SYMBOL_LIST	_var_info;
  MEM_POOL*			_pool;
};

#endif
