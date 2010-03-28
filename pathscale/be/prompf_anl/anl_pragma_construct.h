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
#ifndef anl_pragma_construct_INCLUDED
#define anl_pragma_construct_INCLUDED

// ==============================================================
// ==============================================================
//
// Module: anl_pragma_construct.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Description:
//
//
// ==============================================================
// ==============================================================


class ANL_FUNC_ENTRY;
class ANL_PRAGMA_ATTRIBUTE;

class ANL_PRAGMA_CONSTRUCT
{
private:

   enum ANL_PRAGMA_KIND
   {
      ANL_UNKNOWN_PRAGMA,
      ANL_SECTION_PRAGMA,
      ANL_BARRIER_PRAGMA,
      ANL_ORDERED_PRAGMA,
      ANL_ATOMIC_PRAGMA,
      ANL_CRITICAL_SECTION_PRAGMA
   };

   INT64            _id;
   ANL_PRAGMA_KIND  _pragma_kind;
   WN              *_pragma_begin;
   WN              *_pragma_end;           // If applicable; otherwise NULL
   BOOL             _pr_end_part_of_body ; // is pragma_end last part of construct body
   BOOL             _is_omp ;
   INT32            _construct_level;
   ANL_FUNC_ENTRY  *_func_entry;
   MEM_POOL        *_pool;

   BOOL _End_Is_Part_Of_Construct(BOOL for_id);
   void _Find_Pragma_End_Stmt();
   void _Get_Max_Construct_Srcpos(ANL_SRCPOS *max);
   void _Write_Pragma_Directives(ANL_CBUF *cbuf);

public:

   // =================== Class Utilities ====================

   static BOOL Is_ProMpf_Pragma_Construct(WN *stmt);
   static BOOL Is_Valid_Dir(ANL_PRAGMA_ATTRIBUTE *dir,
			    INT32                 construct_level);


   // ============== Constructors & Destructors ==============

   ANL_PRAGMA_CONSTRUCT(WN             *pragma_begin,
			INT32           construct_level,
			ANL_FUNC_ENTRY *func_entry,
			MEM_POOL       *pool);


   // ============== Analysis and Output =============

   WN *Next_Stmt();

   void Write(ANL_CBUF *cbuf);


}; // class ANL_PRAGMA_CONSTRUCT

#endif /* anl_pragma_construct_INCLUDED */
