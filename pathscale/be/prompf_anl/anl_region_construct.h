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
#ifndef anl_region_construct_INCLUDED
#define anl_region_construct_INCLUDED

// ==============================================================
// ==============================================================
//
// Module: anl_region_construct.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Description:
//
//    This handles constructs represented by a region.  There is
//    only one such region at the moment (PARALLEL) region, although
//    we here implement a class of object that can handle various
//    kinds.
//
// ==============================================================
// ==============================================================


class ANL_FUNC_ENTRY;
class ANL_PRAGMA_ATTRIBUTE;


class ANL_REGION_CONSTRUCT
{
private:

   enum ANL_REGION_KIND
   {
      ANL_UNKNOWN_REGION,
      ANL_PARALLEL_REGION,
      ANL_PSECTION_REGION,
      ANL_SINGLE_PROCESS_REGION,
      ANL_MASTER_PROCESS_REGION
   };

   INT64            _id;
   ANL_REGION_KIND  _region_kind;
   WN              *_region;
   INT32            _construct_level;
   BOOL             _is_nowait;
   BOOL             _is_omp;
   ANL_FUNC_ENTRY  *_func_entry;
   MEM_POOL        *_pool;

   WN *_First_Region_Stmt();
   WN *_Last_Region_Stmt();

   void _Region_Srcpos_Range(ANL_SRCPOS *min, ANL_SRCPOS *max);
   void _Region_Body_Srcpos_Range(ANL_SRCPOS *min, ANL_SRCPOS *max);

   WN* _userEndParallel();

   void _Write_Region_Directive(ANL_CBUF *cbuf);

public:

   // =================== Class Utilities ====================

   static BOOL Is_ProMpf_Region_Construct(WN *stmt);
   static BOOL Is_Valid_Dir(ANL_PRAGMA_ATTRIBUTE *dir,
			    INT32                 construct_level);


   // ============== Constructors & Destructors ==============

   ANL_REGION_CONSTRUCT(WN             *region,
			INT32           construct_level,
			ANL_FUNC_ENTRY *func_entry,
			MEM_POOL       *pool);


   // ============== Analysis and Output =============

   WN  *Next_Stmt();

   void Write(ANL_CBUF *cbuf);


}; // class ANL_REGION_CONSTRUCT

#endif /* anl_region_construct_INCLUDED */
