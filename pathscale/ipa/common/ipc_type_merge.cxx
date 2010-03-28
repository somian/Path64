/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "defs.h"
#include "symtab.h"

#include "ipc_symtab_merge.h"		// for IPC_GLOBAL_TABS, etc.
#include "ipc_ty_hash.h"		// for merging types hash tables
#include "ipc_type_merge.h"

// Algorithm:
//
// 1) for leaf nodes, we simple insert_unqiue
//
// 2) for non leaf node, we start an loop detection phase
//    a) if no loop is found, we insert_unqiue from the leaf up.
//    b) if a loop is found, we start an validation phase
//
// 3) validation phase:
//    a) find a list of all merged TY that match the current node except
//       the indices
//    b) for each of such TY, set the mapped idx temporarily and follow the 
//       chain to verify
//    c) if validation is ok, commit the current node


static inline BOOL
ARB_equal (const ARB_HANDLE merged_arb, const ARB& new_arb)
{
    const UINT64* p1 =  reinterpret_cast<const UINT64*> (merged_arb.Entry ());
    const UINT64* p2 =  reinterpret_cast<const UINT64*> (&new_arb);

    return (p1[0] == p2[0] &&
	    p1[1] == p2[1] &&
	    p1[2] == p2[2] &&
	    p1[3] == p2[3]);
}

// ======================================================================
// function objects to provide uniform access to different TY kinds
// ======================================================================

static const IPC_GLOBAL_TABS* file_tables;

struct array_access
{
    TY_KIND kind () const { return KIND_ARRAY; }

    UINT kid_count () const { return 1; }
	
    TY_IDX kid (const TY& ty, UINT) const {
	return TY_etype (ty);
    }
    
    // validate the array-specific fields
    BOOL validate (const TY& merged_ty, const TY& new_ty) const {
	return Partial_Compare_Arb (TY_arb (merged_ty), 
				    file_tables->arb_tab + new_ty.Arb ());
    }

}; // array_access


struct struct_access
{
    const TY& new_ty;
    const TY& merged_ty;
	
    struct_access (const TY& _new_ty, const TY& _merged_ty) :
	new_ty (_new_ty), merged_ty (_merged_ty) {}
	
    TY_KIND kind () const { return KIND_STRUCT; }

    UINT kid_count () const {
	if (new_ty.Fld () == 0)
	    return 0;
	UINT count = 0;
	const FLD* fld = file_tables->fld_tab + new_ty.Fld () ;
	do {
	    ++count;
	} while ((fld++->flags & FLD_LAST_FIELD) == 0);
	return count;
    }

    TY_IDX kid (const TY& ty, UINT i) const {
	if (&ty == &new_ty) {
	    const FLD* fld = file_tables->fld_tab + new_ty.Fld ();
	    return fld[i].type;
	} else {
	    FLD_HANDLE fld (TY_fld (merged_ty).Idx () + i);
	    return FLD_type (fld);
	}
    }

    BOOL validate (const TY& merged_ty, const TY& new_ty) const {
	if (new_ty.Fld () == 0 || merged_ty.Fld () == 0)
	    return new_ty.Fld () == merged_ty.Fld ();
	return Partial_Compare_Fld (TY_fld (merged_ty),
				    file_tables->fld_tab + new_ty.Fld ());
    }
    
}; // struct_access


struct pointer_access
{
    TY_KIND kind () const { return KIND_POINTER; }

    UINT kid_count () const { return 1; }

    TY_IDX kid (const TY& ty, UINT) const {
	return TY_pointed (ty);
    }

    BOOL validate (const TY&, const TY&) const {
	return TRUE;
    }
}; // pointer_access


struct function_access
{
    const TY& new_ty;
    const TY& merged_ty;
	
    function_access (const TY& _new_ty, const TY& _merged_ty) :
	new_ty (_new_ty), merged_ty (_merged_ty) {}

    TY_KIND kind () const { return KIND_FUNCTION; }

    UINT kid_count () const {
	UINT count = 0;
	const TYLIST* tylist = file_tables->tylist_tab + TY_tylist (new_ty);
	while (*tylist != 0) {
	    ++count;
	    ++tylist;
	}
	return count;
    }

    TY_IDX kid (const TY& ty, UINT i) const {
	if (&ty == &new_ty) {
	    return file_tables->tylist_tab[TY_tylist (ty) + i];
	} else {
	    TYLIST_IDX idx = TY_tylist (merged_ty) + i;
	    return Tylist_Table[idx];
	}
    }

    BOOL validate (const TY& merged_ty, const TY& new_ty) const {
	return new_ty.Pu_flags () == merged_ty.Pu_flags ();
    }
}; // function_access
	



// ======================================================================
// validation state for Validate_Recursive_Type ()
// ======================================================================
enum VALIDATION_STATE
{
    VALIDATE_OK		= 1,		// no error found
    VALIDATE_FAIL	= 2,		
    VALIDATE_COMMIT	= 3		// no error found + already
					// inserted into hash table
};


static VALIDATION_STATE
Validate_Recursive_Type (TY_INDEX index, TY_IDX_MAP& ty_map,
			 const SYMSTR_IDX_MAP& str_map, TY_IDX merged_ty_idx);

namespace
{
    
    template <class ACCESS>
    VALIDATION_STATE
    Partial_Match (const TY& merged_ty, const TY& new_ty,
		   TY_IDX_MAP& ty_map,
		   const SYMSTR_IDX_MAP& str_map,
		   const ACCESS& ty_node)
    {
	Is_True (TY_kind (new_ty) == ty_node.kind (),
		 ("Invalid type attributes for array"));

	if (TY_size (merged_ty) != TY_size (new_ty) ||
	    TY_kind (merged_ty) != TY_kind (new_ty) ||
	    TY_mtype (merged_ty) != TY_mtype (new_ty) ||
	    TY_flags (merged_ty) != TY_flags (new_ty) ||
	    TY_name_idx (merged_ty) != str_map[TY_name_idx (new_ty)])
	    return VALIDATE_FAIL;

	UINT kid_count = ty_node.kid_count ();
	UINT i;
	for (i = 0; i < kid_count; ++i) {
	    if (TY_IDX_Attributes (ty_node.kid (merged_ty, i)) !=
		TY_IDX_Attributes (ty_node.kid (new_ty, i)))
		return VALIDATE_FAIL;
	}

	if (! ty_node.validate (merged_ty, new_ty))
	    return VALIDATE_FAIL;

	BOOL all_kids_committed = TRUE;
	// everything matches so far, now look at the kids
	for (i = 0; i < kid_count; ++i) {
	    TY_INDEX kid_index = TY_IDX_index (ty_node.kid (new_ty, i));
	    TY_IDX kid_mapped_idx = ty_map.map_[kid_index];

	    if (TY_Inserted (kid_mapped_idx))
		return VALIDATE_FAIL;

	    if (Is_TY_Temp_Idx (kid_mapped_idx)) {
		all_kids_committed = FALSE;
		Clean_TY_IDX (kid_mapped_idx);
	    }
	    
	    if (Valid_TY_IDX (kid_mapped_idx)) {
		if (TY_IDX_index (ty_node.kid (merged_ty, i)) !=
		    TY_IDX_index (kid_mapped_idx))
		    return VALIDATE_FAIL;
	    } else {
		VALIDATION_STATE v_state =
		    Validate_Recursive_Type (kid_index, ty_map, str_map, 
					     ty_node.kid (merged_ty, i));
		
		if (v_state == VALIDATE_FAIL)
		    return v_state;
		else if (v_state == VALIDATE_OK)
		    all_kids_committed = FALSE;
	    }
	}

	// all kids' types match
	if (all_kids_committed) {
	    return VALIDATE_COMMIT;
	} else
	    return VALIDATE_OK;
    } // Partial_Match
}


static VALIDATION_STATE
Validate_Recursive_Type (TY_INDEX index, TY_IDX_MAP& ty_map,
			 const SYMSTR_IDX_MAP& str_map, TY_IDX merged_ty_idx)
{
    TY_IDX& my_mapped_idx = ty_map.map_[index];

    Is_True (! Valid_TY_IDX (my_mapped_idx),
	     ("Invalid call to Validate_Recursive_Type"));

    const TY& new_ty = file_tables->ty_tab[index];
    const TY& merged_ty = Ty_Table[merged_ty_idx];

    if (TY_kind (new_ty) != TY_kind (merged_ty))
	return VALIDATE_FAIL;

    VALIDATION_STATE v_state;
    
    switch (TY_kind (new_ty)) {
	
    case KIND_SCALAR:
    case KIND_VOID:
	ty_map.set_map (index, Insert_Unique_Ty (new_ty));
	if (ty_map.map_[index] == merged_ty_idx) {
	    return VALIDATE_COMMIT;
	} else
	    return VALIDATE_FAIL;

    case KIND_ARRAY:
	Set_TY_Temp_Idx (my_mapped_idx, merged_ty_idx);
	v_state = Partial_Match (merged_ty, new_ty, ty_map, str_map,
				 array_access ());
	break;

    case KIND_STRUCT:
	Set_TY_Temp_Idx (my_mapped_idx, merged_ty_idx);
	v_state = Partial_Match (merged_ty, new_ty, ty_map, str_map,
				 struct_access (new_ty, merged_ty));
	break;
	
    case KIND_POINTER:
	Set_TY_Temp_Idx (my_mapped_idx, merged_ty_idx);
	v_state = Partial_Match (merged_ty, new_ty, ty_map, str_map,
				 pointer_access ());
	break;

    case KIND_FUNCTION:
	Set_TY_Temp_Idx (my_mapped_idx, merged_ty_idx);
	v_state = Partial_Match (merged_ty, new_ty, ty_map, str_map,
				 function_access (new_ty, merged_ty));
	break;
    }
    
    if (v_state == VALIDATE_COMMIT)
	ty_map.set_map (index, Insert_Unique_Ty (new_ty));
    return v_state;
} // Validate_Recursive_Type

static void
Clear_All_Temp_Idx (TY_INDEX index, TY_IDX_MAP& ty_map);

namespace {

    template <class ACCESS>
    void
    Clear_Temp_Idx_Specific (const TY& new_ty, TY_IDX_MAP& ty_map,
			const ACCESS& ty_node)
    {
	UINT kid_count = ty_node.kid_count ();
	for (UINT i = 0; i < kid_count; ++i) {
	    TY_INDEX kid_index = TY_IDX_index (ty_node.kid (new_ty, i));
	    TY_IDX mapped_kid_idx = ty_map.map_[kid_index];
	    if (Is_TY_Temp_Idx (mapped_kid_idx))
		Clear_All_Temp_Idx (kid_index, ty_map);
	}
    }
}


// Clear all temp_idx that have been set
static void
Clear_All_Temp_Idx (TY_INDEX index, TY_IDX_MAP& ty_map)
{
    TY_IDX& my_mapped_idx = ty_map.map_[index];

    if (! Is_TY_Temp_Idx (my_mapped_idx))
	return;

    Clear_TY_Temp_Idx (my_mapped_idx);

    const TY& new_ty = file_tables->ty_tab[index];

    switch (TY_kind (new_ty)) {
    case KIND_SCALAR:
    case KIND_VOID:
	return;

    case KIND_ARRAY:
	Clear_Temp_Idx_Specific (new_ty, ty_map, array_access ());
	break;

    case KIND_STRUCT:
	Clear_Temp_Idx_Specific (new_ty, ty_map, struct_access (new_ty,
								new_ty));  
	break;

    case KIND_POINTER:
	Clear_Temp_Idx_Specific (new_ty, ty_map, pointer_access ()); 
	break;
	
    case KIND_FUNCTION:
	Clear_Temp_Idx_Specific (new_ty, ty_map, function_access (new_ty,
								  new_ty)); 
	
	break;
    }
    
} // Clear_All_Temp_Idx 


// Given a recursive type, search in the merged symtab for a matching one.
static TY_IDX
Find_Recursive_Type (TY_INDEX index, TY_IDX_MAP& ty_map,
		     const SYMSTR_IDX_MAP& str_map, const TY& ty)
{
    // Get a list of all types in the merged symtab that match "ty" except
    // for the indices
    TY_IDX_VEC ty_list;
    Find_Matching_Ty (ty, ty_list);
    
    for (TY_IDX_VEC::const_iterator iter (ty_list.begin ());
	 iter != ty_list.end (); ++iter) {
	VALIDATION_STATE state =
	    Validate_Recursive_Type (index, ty_map, str_map, *iter);
	
	if (state == VALIDATE_OK) {
	    return *iter;
	}
	
	Is_True (state != VALIDATE_COMMIT, ("Inconsistent cycles in types"));

	Clear_All_Temp_Idx (index, ty_map);
    }

    return 0;
} // Find_Recursive_Type


// Map the given recursive type to the matching one in the merged symtab
static void
Commit_Recursive_Type (TY_INDEX index, TY_IDX_MAP& ty_map,
		       TY_IDX merged_ty_idx);
namespace
{

    template <class ACCESS>
    void
    Commit_Ty_Specific (const TY& new_ty, TY_IDX_MAP& ty_map,
			TY_IDX merged_ty_idx, const ACCESS& ty_node)
    {
	const TY& merged_ty = Ty_Table[merged_ty_idx];
	Is_True (TY_kind (merged_ty) == ty_node.kind (),
		 ("Invalid type in Commit_Recursive_Type"));
	
	UINT kid_count = ty_node.kid_count ();
	for (UINT i = 0; i < kid_count; ++i) {
	    TY_INDEX kid_index = TY_IDX_index (ty_node.kid (new_ty, i));
	    TY_IDX mapped_kid_idx = ty_map.map_[kid_index];
	    if (! Valid_TY_IDX (mapped_kid_idx))
		Commit_Recursive_Type (kid_index, ty_map,
				       ty_node.kid (merged_ty, i));
	}
    }
}

	   
// Map the given recursive type to the matching one in the merged symtab
static void
Commit_Recursive_Type (TY_INDEX index, TY_IDX_MAP& ty_map,
		       TY_IDX merged_ty_idx)
{
    TY_IDX& my_mapped_idx = ty_map.map_[index];

    Is_True (! Valid_TY_IDX (my_mapped_idx), ("Invalid type index"));

    ty_map.set_map (index, merged_ty_idx);

    const TY& new_ty = file_tables->ty_tab[index];

    switch (TY_kind (new_ty)) {
    case KIND_SCALAR:
    case KIND_VOID:
	return;

    case KIND_ARRAY:
	Commit_Ty_Specific (new_ty, ty_map, merged_ty_idx, array_access ());
	break;

    case KIND_STRUCT:
	Commit_Ty_Specific (new_ty, ty_map, merged_ty_idx,
			    struct_access (new_ty, Ty_Table[merged_ty_idx])); 
	break;

    case KIND_POINTER:
	Commit_Ty_Specific (new_ty, ty_map, merged_ty_idx, pointer_access ()); 
	break;
	
    case KIND_FUNCTION:
	Commit_Ty_Specific (new_ty, ty_map, merged_ty_idx,
			    function_access (new_ty, Ty_Table[merged_ty_idx]));
	break;
    }
} // Commit_Recursive_Type


static TY_IDX
Insert_Ty (TY_INDEX index, TY_IDX_MAP& ty_map, const SYMSTR_IDX_MAP& str_map);

namespace
{

    template <class ACCESS>
    TY_IDX
    Insert_Ty_Specific (TY_INDEX index, TY_IDX_MAP& ty_map,
			const SYMSTR_IDX_MAP& str_map, const ACCESS& ty_node)
    {
	const TY& ty = file_tables->ty_tab[index];
	TY_IDX& my_mapped_idx = ty_map.map_[index];

	// check if the kid type has been merged
	UINT kid_count = ty_node.kid_count ();
	BOOL new_recursive_type = FALSE;
	UINT i;
	for (i = 0; i < kid_count; ++i ) {
	    TY_IDX kid_idx = ty_node.kid (ty, i);
	    TY_IDX mapped_kid_idx = ty_map.map_[TY_IDX_index (kid_idx)];

	    Is_True (! Is_TY_Temp_Idx (mapped_kid_idx),
		     ("Overlapping insertion and validation phase"));

	    if (mapped_kid_idx == 0) {
		mapped_kid_idx = Insert_Ty (TY_IDX_index (kid_idx), ty_map,
					    str_map);
		if (Valid_TY_IDX (my_mapped_idx)) {
		    // The kid recursively points back to myself AND found a
		    // match for myself in the merged TY table
		    return my_mapped_idx;
		}
	    } else if (TY_Inserted (mapped_kid_idx))
		continue;

	    if (!Valid_TY_IDX (mapped_kid_idx)) {
		TY_IDX original_ty_idx;
		Is_True (TY_Merging (mapped_kid_idx),
			 ("Invalid state in type merging"));

		// store original idx value
		original_ty_idx = my_mapped_idx;

		// this is a recursive type, the kid is pointing back to a
		// previously visited TY.  We now need to search for a
		// matching recursive type in the merged symtab, if we
		// haven't already done so.
		TY_IDX matched_idx = new_recursive_type ? 0 :
		    Find_Recursive_Type (index, ty_map, str_map, ty);

		if (matched_idx == 0) {
			my_mapped_idx = Replace_TY_IDX_index(my_mapped_idx, original_ty_idx);
		}
		
		if (matched_idx == 0) {
		    // No matching cycle is found
		     TY_IDX recursive_ty_idx;
		     (void) New_TY (recursive_ty_idx);
		     Set_TY_Inserted (ty_map.map_[TY_IDX_index (kid_idx)],
				      recursive_ty_idx); 
		     Initialize_New_Recursive_Type (recursive_ty_idx);
		} else {
		    Is_True (Valid_TY_IDX (matched_idx), ("Invalid TY_IDX"));
		    // found a match, we can commit the entire recursive type 
		    // (connected component) and return.
		    Commit_Recursive_Type (index, ty_map, matched_idx); 
		    return matched_idx;
		}
	    }
	}

	// Decided to insert this type into the merged symtab
	if (TY_Inserted (my_mapped_idx)) {
	    // an entry has already been reserved for me
	    Clean_TY_IDX (my_mapped_idx);
	    TY& new_ty = Ty_Table[my_mapped_idx];
	    new_ty = ty;
	    Insert_Allocated_Ty (new_ty, my_mapped_idx);
	    Finalize_New_Recursive_Type ();
	    return my_mapped_idx;
	} else
	    return Insert_Unique_Ty (ty);
    } // Insert_Ty_Specific
} // namespace


static TY_IDX
Insert_Ty (TY_INDEX index, TY_IDX_MAP& ty_map, const SYMSTR_IDX_MAP& str_map)
{
    TY_IDX& mapped_idx = ty_map.map_[index];

    Is_True (mapped_idx == 0, ("Invalid call to Insert_Ty"));

    Set_TY_Merging (mapped_idx);

    const TY& ty = file_tables->ty_tab[index];

    TY_IDX return_idx;

    switch (TY_kind (ty)) {
    case KIND_SCALAR:
    case KIND_VOID:
	return_idx = Insert_Unique_Ty (ty);
	break;
	
    case KIND_ARRAY:
	return_idx = Insert_Ty_Specific (index, ty_map, str_map,
					 array_access ());
	break;
	
    case KIND_STRUCT:
	return_idx = Insert_Ty_Specific (index, ty_map, str_map,
					 struct_access (ty, ty));
	break;

    case KIND_POINTER:
	return_idx = Insert_Ty_Specific (index, ty_map, str_map,
					 pointer_access ());
	break;
	
    case KIND_FUNCTION:
	return_idx = Insert_Ty_Specific (index, ty_map, str_map,
					 function_access (ty, ty));
	break;
	
    default:
	Fail_FmtAssertion ("Invalid TY_kind");
	return 0;
    }
    
    ty_map.set_map (index, return_idx);
    return return_idx;
} // Insert_Ty


void
Merge_All_Types (const IPC_GLOBAL_TABS& original_tabs,
		 IPC_GLOBAL_IDX_MAP& idx_map)
{
    Temporary_Error_Phase ephase ("IPA Type Merging");

    Setup_Type_Merging_Hash_Tables (original_tabs, idx_map);
    
    file_tables = &original_tabs;
    TY_IDX_MAP& ty_map = idx_map.ty;
    const SYMSTR_IDX_MAP& str_map = idx_map.sym_str;
    
    

    for (UINT idx = 1; idx < file_tables->ty_tab_size; ++idx) {
	if (ty_map.map_[idx] == 0)
	    (void) Insert_Ty (idx, ty_map, str_map);
	else
	    Is_True (Valid_TY_IDX (ty_map.map_[idx]), ("Invalid TY_IDX"));
    }
} // Merge_All_Types
