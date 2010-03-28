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
#ifndef anl_dir_attribute_INCLUDED
#define anl_dir_attribute_INCLUDED

// ==============================================================
// ==============================================================
//
// Module: anl_pragma_attribute.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Description:
//
//    This defines a class representing a directive attribute, which
//    should belong to some enclosing construct (function, loop, etc.).
//
// ==============================================================
// ==============================================================


class ANL_FUNC_ENTRY;
class ANL_SRCPOS;


class ANL_PRAGMA_ATTRIBUTE
{
private:

   enum ANL_PRAGMA_ATTR_KIND
   {
      ANL_UNKNOWN_ATTR,
      ANL_PREFETCH_REF_ATTR,
      ANL_DISTRIBUTE_ATTR,
      ANL_REDISTRIBUTE_ATTR,
      ANL_DISTRIBUTE_RESHAPE_ATTR,
      ANL_DYNAMIC_ATTR,
      ANL_COPYIN_ATTR,
      ANL_NUMTHREADS_ATTR,
      ANL_PAGE_PLACE_ATTR,
      ANL_CONCURRENTIZE,
      ANL_NOCONCURRENTIZE,
      ANL_ASSERT_PERMUTATION,
      ANL_ASSERT_CONCURRENT_CALL,
      ANL_ASSERT_DO,
      ANL_ASSERT_DOPREFER,
      ANL_IVDEP,
      ANL_FLUSH,
      ANL_NUM_ATTRS
   };

   ANL_PRAGMA_ATTR_KIND _pragma_kind;
   WN                  *_apragma;
   ANL_FUNC_ENTRY      *_func_entry;
   INT32                _enclosing_construct_level;
   MEM_POOL            *_pool;
   
   static void _Append_Arg_Numbers(ANL_CBUF *cbuf,
				   INT32     val1,
				   INT32     val2);

   BOOL _Is_Assertion();
   void _Write_Distribution(ANL_CBUF *cbuf, WN **next);
   void _Write_Pragma_Arguments(ANL_CBUF *cbuf);
   void _Write_Pragma(ANL_CBUF *cbuf);

public:

   // =================== Class Utilities ====================

   static BOOL Is_ProMpf_Pragma_Attribute(WN *stmt);


   // ============== Constructors & Destructors ==============

   ANL_PRAGMA_ATTRIBUTE(WN             *apragma,
			INT32           enclosing_construct_level,
			ANL_FUNC_ENTRY *func_entry,
			MEM_POOL       *pool);


   // ======================== Queries ========================
      
   BOOL Is_Pragma_Construct_Attribute(INT32 construct_level);
   BOOL Is_Region_Construct_Attribute(INT32 construct_level);
   BOOL Is_Loop_Construct_Attribute(INT32 construct_level);


   // =================== Analysis and Output =================

   WN *Next_Stmt(); // Stmt after last clause belonging to this directive

   void Write(ANL_CBUF *cbuf, INT32 id);


}; // class ANL_PRAGMA_ATTRIBUTE

#endif /* anl_pragma_attribute_INCLUDED */
