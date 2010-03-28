/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "defs.h"  // Basic types, such as INT32 */
#include "cxx_memory.h"    /* CXX_NEW/CXX_DELETE */
#include "anl_cbuf.h"


BOOL
ANL_CBUF::_Is_Splitc(char splitc)
{
   INT i;

   for (i = 0; _splitchars[i] != '\0' && _splitchars[i] != splitc; i++);
   return (_splitchars[i] == splitc);

} // ANL_CBUF::_Is_Splitc


void
ANL_CBUF::_Split()
{
   // Preconditions: _buf must be '\0' terminated
   //                _linelength == _max_linelength
   //
   // Note that we never split a line such that the resultant line,
   // after concatenating in the continuation string, again must be
   // split (since this could lead to an infinite recursion of splits).
   //
   INT splitpt, reduction;

   for (splitpt = _next-1;
	splitpt >= 0 && _buf[splitpt] != '\n' && !_Is_Splitc(_buf[splitpt]);
	splitpt--);

   reduction = _next-(splitpt+1);
   if (strlen(_continuation)+reduction < _max_linelength &&
       _buf[splitpt] != '\n')
   {
      // Do the split immediately after the splitpt; any other 
      // cases are considered impossible to split.
      //
      ANL_CBUF remainder(_pool, &_buf[splitpt+1]);

      _linelength -= reduction; // Start again from split-point
      _next = splitpt+1;
      Write_String(_continuation);      // Insert continuation on a new line
      Write_String(remainder.Chars());  // Insert characters after splitpt
   }
} // ANL_CBUF::_Split


ANL_CBUF::ANL_CBUF(MEM_POOL *pool, const char *s):
   _splitchars(""),
   _continuation(""),
   _linelength(0),
   _max_linelength(UINT32_MAX),
   _chunk_size(CBUF_SMALLEST_ALLOC_SIZE),
   _pool(pool)
{
   if (s == NULL)
   {
      _next = 0;
      _linelength = 0;
      _size = 0;
      _buf = NULL;
   }
   else
   {
      // Allocate a multiple of chunks large enough to hold s plus
      // one additional character.
      //
      _next = strlen(s);
      _size = _A_Number_Of_Chunks(_next+1);
      _buf = _Alloc(_size);
      for (INT i=0; i<_next; i++)
      {
	 _buf[i] = s[i];
	 if (s[i]=='\n')
	    _linelength = 0;
	 else
	    _linelength++;
      }
      _buf[_next] = '\0';
   }
} // ANL_CBUF::ANL_CBUF


void
ANL_CBUF::Write_Char(char c)
{
   if (_size == 0)
   {
      _size = _chunk_size;
      _buf = _Alloc(_size);
   }
   else if (_next+1 >= _size)
   {
      char *reclaim = _buf;

      _size = _size + _chunk_size;
      _buf = strcpy(_Alloc(_size), reclaim);
      CXX_DELETE_ARRAY(reclaim, _pool);
   }
   _buf[_next++] = c;
   _buf[_next] = '\0';

   if (c == '\n')
      _linelength = 0;
   else if (++_linelength >= _max_linelength)
      _Split();

} // ANL_CBUF::Write_Char


void
ANL_CBUF::Write_String(const char *s)
{
   UINT len;

   if (s==NULL)
      return;

   len = strlen(s);
   if (_next+len >= _size)
   {
      // Allocate a multiple of chunks large enough to hold s + '\0'
      //
      _size = _A_Number_Of_Chunks(_next+len+1);
      if (_next > 0)
      {
	 char *reclaim = _buf;

	 _buf = strcpy(_Alloc(_size), reclaim);
	 CXX_DELETE_ARRAY(reclaim, _pool);
      }
      else
	 _buf = _Alloc(_size);
   }
   for (INT i=0; i<len; i++)
   {
      _buf[_next++] = s[i];

      if (s[i] == '\n')
	 _linelength = 0;
      else if (++_linelength >= _max_linelength)
      {
	 _buf[_next] = '\0';
	 _Split();  // NOTE: may call CBUF::Write_String() recursively

	 // May need to reallocate the buffer, so do the rest recursively
	 //
	 if (i+1<len)
	    Write_String(&s[i+1]);
	 i = len;
      }
   }
   _buf[_next] = '\0';
} // ANL_CBUF::Write_String


void
ANL_CBUF:: Append_Pragma_Preamble(BOOL is_omp, BOOL is_lower_case)
{
  // Append the appropriate preamble for OMP pragmas 
  // or just add a space.

   if (is_omp)
     if (is_lower_case)
       Write_String("omp_");
     else
      Write_String(" OMP_");
   else 
     if (!is_lower_case)
       Write_Char(' ');

}  // ANL_CBUF:: Append_Pragma_Preamble


 
