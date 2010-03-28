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
#ifndef anl_loop_construct_INCLUDED
#define anl_loop_construct_INCLUDED

// ==============================================================
// ==============================================================
//
// Module: anl_loop_construct.h
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

class ANL_LOOP_CONSTRUCT
{
private:

   enum ANL_LOOP_KIND
   {
      ANL_DO_LOOP,
      ANL_WHILE_LOOP,
      ANL_WHILE_AS_DO_LOOP,
      ANL_DOACROSS_LOOP,
      ANL_PARALLELDO_LOOP,
      ANL_PDO_LOOP
   };

   INT64           _id;
   ANL_LOOP_KIND   _loop_kind;
   WN             *_loop_region;
   WN             *_loop;
   INT32           _construct_level;
   ANL_FUNC_ENTRY *_func_entry;
   MEM_POOL       *_pool;

   WN *_First_Loop_Stmt();
   WN *_Last_Loop_Stmt();

   static BOOL Is_Valid_Dir(ANL_PRAGMA_ATTRIBUTE *dir, 
			    INT32                 construct_level);
   static BOOL Is_End_Of_Loop_Comment(WN *end_stmt);
   static void Remove_Stmt_In_Block(WN *stmt);

   BOOL _Is_Parallel_Loop() const 
   {
      return (_loop_kind == ANL_DOACROSS_LOOP   ||
	      _loop_kind == ANL_PARALLELDO_LOOP ||
	      _loop_kind == ANL_PDO_LOOP);
   }
   
   void _Loop_Srcpos_Range(ANL_SRCPOS *min, ANL_SRCPOS *max);
   void _Write_Loop_Header(ANL_CBUF *cbuf);
   void _Write_Loop_Directive(ANL_CBUF *cbuf);

public:

   // ============== Constructors & Destructors ==============

   ANL_LOOP_CONSTRUCT(WN             *loop,
		      INT32           construct_level,
		      ANL_FUNC_ENTRY *func_entry,
		      MEM_POOL       *pool);


   // ============== Analysis and Output =============

   WN *Next_Stmt();

   void Write(ANL_CBUF *cbuf);


}; // class ANL_LOOP_CONSTRUCT

#endif /* anl_loop_construct_INCLUDED */
