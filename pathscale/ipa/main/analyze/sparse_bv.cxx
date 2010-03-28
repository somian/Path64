/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


#include "defs.h"
#include "mempool.h"
#include "cxx_memory.h"
#include "errors.h"

#include "sparse_bv.h"

void
SPARSE_BV::Set_Bit (UINT32 index)
{
    if (buffer == NULL) {
	buffer = CXX_NEW (SPARSE_BV_BUFFER, Malloc_Mem_Pool);
	data = CXX_NEW (SPARSE_BV_DATA, Malloc_Mem_Pool);
	data->push_back (0LL);		// reserve 0th entry
    }

    if (index >= buffer->size () * mUINT64_BIT) {
	INT n = ((index + mUINT64_BIT) / mUINT64_BIT) - buffer->size ();
	buffer->insert (buffer->end (), n, 0);
    }

    UINT32 idx = index / mUINT64_BIT;

    Is_True (idx < buffer->size (), ("Bit vector index out of range"));
    
    if ((*buffer)[idx] == 0 ) {
	(*buffer)[idx] = data->size();
	data->push_back (1LL << (index % mUINT64_BIT));
    } else 
	(*data)[(*buffer)[idx]] |= 1LL << (index % mUINT64_BIT);

#ifdef Is_True_On
    //    Verify ();
#endif
} // SPARSE_BV::Set_Bit


void
SPARSE_BV::Union_2_Core (const SPARSE_BV& vec, BUFFER_ITER vec_iter,
			 BUFFER_ITER iter)
{
    while (vec_iter != vec.buffer->end ()) {
	Is_True (iter != buffer->end (), ("bit vector buffer too short"));

	if (*vec_iter != 0) {
	    if (*iter == 0) {
		// copy over this entry
		*iter = data->size ();
		data->push_back ((*vec.data)[*vec_iter]);
	    } else {
		(*data)[*iter] |= (*vec.data)[*vec_iter];
	    }
	}

	++vec_iter;
	++iter;
    }

#ifdef Is_True_On
    //    Verify ();
#endif
} // Union_2_Core


void
SPARSE_BV::Union_2 (const SPARSE_BV& vec)
{
    if (vec.buffer == NULL)
	return;

    if (buffer == NULL) {
	buffer = CXX_NEW (SPARSE_BV_BUFFER, Malloc_Mem_Pool);
	buffer->reserve (vec.buffer->size ());
	data = CXX_NEW (SPARSE_BV_DATA, Malloc_Mem_Pool);
	data->reserve (vec.data->size ());
	data->push_back (0LL);
    }

    // check if need to extend the vector at the end
    if (vec.buffer->size () > buffer->size ()) {
	buffer->insert (buffer->end (),
			vec.buffer->size() - buffer->size (), 0);
    }
    
    Union_2_Core (vec, vec.buffer->begin (), buffer->begin ());

#ifdef Is_True_On
    //    Verify ();
#endif
    
} // Union_2


BOOL
SPARSE_BV::Union_2_Diff_Core (const SPARSE_BV& vec)
{
    BOOL modified = FALSE;

    // check if need to extend the vector at the end
    if (vec.buffer->size () > buffer->size ()) {
	buffer->insert (buffer->end (),
			vec.buffer->size() - buffer->size (), 0);
	modified = TRUE;
    }

    BUFFER_ITER vec_iter (vec.buffer->begin ());
    BUFFER_ITER iter (buffer->begin ());
    
    if (!modified) {
	// check for differences before modifying the vector
	
	while (!modified && vec_iter != vec.buffer->end ()) {
	    Is_True (iter != buffer->end (),
		     ("Inconsisten bit vector comparison"));
	    if (*vec_iter != 0) {
		if (*iter == 0) {
		    modified = TRUE;
		    break;
		}
		mUINT64& data_word = (*data)[*iter];
		mUINT64 xword = data_word | (*vec.data)[*vec_iter];
		if (xword != data_word) {
		    modified = TRUE;
		    data_word = xword;
		}
	    }

	    ++vec_iter;
	    ++iter;
	}

    }

    if (modified)
	Union_2_Core (vec, vec_iter, iter);

    return modified;
    
} // Union_2_Diff_Core


#ifdef Is_True_On

void
SPARSE_BV::Verify () const
{
    if (buffer == NULL)
	return;

    Is_True (buffer->size () != 0, ("SPARSE_BV: empty map"));
    Is_True (buffer->back () != 0, ("SPARSE_BV: empty trailing slot"));

    for (BUFFER_ITER first (buffer->begin ()); first != buffer->end ();
	 ++first) {

	if (*first != 0) {
	    Is_True (*first < data->size (), ("SPARSE_BV: invalid index"));
	    Is_True ((*data)[*first] != 0LL, ("SPARSE_BV:  empty buffer"));
	}
    }
    
}
#endif // Is_True_On
