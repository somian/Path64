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
#ifndef anl_func_entry_INCLUDED
#define anl_func_entry_INCLUDED

// ==============================================================
// ==============================================================
//
// Module: anl_func_entry.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Description:
//
//    An object to represent the analysis of a function/subroutine
//    to produce .anl file entries.  Such entries may be original
//    source construct descriptors (and eventually, perhaps,
//    transformed source construct descriptors).
//
// ==============================================================
// ==============================================================


#include <array_set.h>

class ANL_PRAGMA_ATTRIBUTE;
typedef ARRAY_SET<ANL_PRAGMA_ATTRIBUTE*> DIR_ENTRIES;

class ANL_FUNC_ENTRY
{
private:

   INT32            _construct_level;
   WN              *_pu;
   MEM_POOL        *_pool;
   W2CF_TRANSLATOR *_w2cf;
   WN_MAP           _id_map;
   COUNTER         *_next_id;
   DIR_ENTRIES      _dir_entries;

   void _Push_Construct_Level(ANL_CBUF *cbuf);
   void _Pop_Construct_Level();

public:

   // ============== Constructors & Destructors ==============

   ANL_FUNC_ENTRY(WN              *pu, 
		  MEM_POOL        *pool, 
		  W2CF_TRANSLATOR *w2cf, 
		  WN_MAP           id_map,
		  COUNTER         *next_id):
   _construct_level(0),
   _pu(pu),
   _pool(pool),
   _w2cf(w2cf),
   _id_map(id_map),
   _next_id(next_id),
   _dir_entries(pool)
   {}

   ~ANL_FUNC_ENTRY();


   // =================== Inquiries ===================

   W2CF_TRANSLATOR *Pu_Translator()     {return _w2cf;}
   WN              *Pu_Tree()           {return _pu;}
   COUNTER         *Next_Construct_Id() {return _next_id;}
   DIR_ENTRIES     *Dir_Entries()       {return &_dir_entries;}

   void             Get_Pragma_Srcpos_Range(WN        *apragma,
					    ANL_SRCPOS *min,
					    ANL_SRCPOS *max);


   // ============== Analysis and Output =============

   void Set_Construct_Id(WN *construct, INT64 id);

   void Emit_Nested_Original_Constructs(ANL_CBUF *cbuf,
					WN       *from_stmt, 
					WN       *to_stmt);

   void Emit_Dir_Entries(ANL_CBUF *cbuf,
			 INT64     for_construct_id,
			 INT32     for_construct_level,
			 BOOL    (*do_emit)(ANL_PRAGMA_ATTRIBUTE *dir,
					    INT32      construct_level));

   void Emit_Original_Construct(ANL_FILE_MNGR *outp_file);


}; // class ANL_FUNC_ENTRY

#endif /* anl_func_entry_INCLUDED */
