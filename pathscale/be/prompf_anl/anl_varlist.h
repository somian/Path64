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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */
#ifndef anl_varlist_INCLUDED
#define anl_varlist_INCLUDED

// ==============================================================
// ==============================================================
//
// Module: anl_varlist.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Description:
//
//    An object to represent the attributes we accumulate for 
//    variable references, and an object to represent a set of 
//    such variables with their attributes (sorted on ST_idx()s).
//
// ==============================================================
// ==============================================================

#include "array_set.h" // For ARRAY_SET

class ANL_FUNC_ENTRY; // Defined in "anl_func_entry.h"

#define ANL_VAR_READ    0x00000001
#define ANL_VAR_WRITTEN 0x00000002


class ANL_VAR
{
private:
   ST      *_st;
   ANL_VAR *_alias; // A circular list of name aliases
   UINT32  _status;

public:

   // ============== Constructors & Destructors ==============

   ANL_VAR(): _st(NULL), _alias(this), _status(0) {}
   ANL_VAR(ST *st): _st(st), _alias(this), _status(0) {}


   // =================== Inquiries ===================

   INT32 Id() const {return (_st==NULL? 0 : ST_st_idx(_st));}
   BOOL operator==(const ANL_VAR &v) const {return Id() == v.Id();}
   BOOL operator!=(const ANL_VAR &v) const {return Id() != v.Id();}
   BOOL operator<(const ANL_VAR &v) const {return Id() < v.Id();}
   BOOL operator<=(const ANL_VAR &v) const {return Id() <= v.Id();}
   BOOL operator>(const ANL_VAR &v) const {return Id() > v.Id();}
   BOOL operator>=(const ANL_VAR &v) const {return Id() >= v.Id();}

   BOOL Is_Read() {return ((_status & ANL_VAR_READ) != 0);}
   BOOL Is_Written() {return ((_status & ANL_VAR_WRITTEN) != 0);}


   // =================== Modification ==================

   ANL_VAR &operator=(const ANL_VAR &v) 
   {
      _st = v._st;
      _status = v._status;
      return *this;
   }

   void Set_Name_Alias(ANL_VAR *var);

   void Set_Read() {_status = (_status | ANL_VAR_READ);}
   void Set_Written() {_status = (_status | ANL_VAR_WRITTEN);}

   void Reset_References(); // For this and all aliases


   // ================ Printing Variable Info ===============

   void Write(ANL_CBUF *cbuf, ANL_FUNC_ENTRY *_func_entry); // this and aliases

}; // class ANL_VAR


class ANL_VARLIST
{
private:

   ARRAY_SET<ANL_VAR*> _vlist;  // List of ANL_VARs sorted by ST_idx()s.
   ANL_FUNC_ENTRY     *_func_entry;
   MEM_POOL           *_pool;

   mUINT32 _Binary_Search(INT32 id, mUINT32 from, mUINT32 till);

   UINT32 _Get_Io_Item_Lda_Access_Status(WN *io_item);

   UINT32 _Get_Lda_Access_Status(WN *lda);

public:

   // ============== Constructors & Destructors ==============

   ANL_VARLIST(MEM_POOL *pool, ANL_FUNC_ENTRY *func_entry):
   _vlist(pool), 
   _pool(pool),
   _func_entry(func_entry)
   {}

   ~ANL_VARLIST()
   {
      for (INT i = 0; i < _vlist.Size(); i++)
	 CXX_DELETE(_vlist.Indexed_Get(i), _pool);
   }


   // =================== Search ===================

   ANL_VAR *Find(ST *st);            // NULL if not found
   ANL_VAR *Find_or_Insert(ST *st);  // Never NULL


   // =================== Initialization ===================

   void Insert_Var_Refs(WN *subtree);


   // ============== Printing Variable-list Info =============

   void Write(ANL_CBUF *cbuf, INT64 construct_id);


}; // class ANL_VARLIST

#endif /* anl_varlist_INCLUDED */
