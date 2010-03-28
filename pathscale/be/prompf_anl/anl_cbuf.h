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
#ifndef anl_cbuf_INCLUDED
#define anl_cbuf_INCLUDED

// ==============================================================
// ==============================================================
//
// Module: anl_cbuf.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Description:
//
//    A class used for buffering and manipulating dynamically
//    allocated character strings.  The dynamic allocation scheme
//    is hidden from the user of this class.
//
//    The memory to buffer characters is allocated in multiples
//    of fixed-sized chunks.  Hence, the constructor with an
//    initial allocation size will allocate the number of "chunks"
//    into which the given size will fit.
//
//    Note that any call to Write_Char() or Write_String() may
//    cause the dynamically allocated buffer to be reallocated,
//    and as such may invalidate any earlier results from Chars().
//
// ==============================================================
// ==============================================================


#define CBUF_SMALLEST_ALLOC_SIZE 128

class ANL_CBUF
{
private:

   const char *_splitchars;     // Chars where it is legal to split a line
   const char *_continuation;   // Prefix for the continuation of a line
   UINT32      _max_linelength; // Maximum number of chars we put on one line
   UINT32      _linelength;     // Current line-length
   UINT32      _chunk_size;     // Size of chunks allocated to this buffer
   UINT        _size;           // Size of buffer
   UINT        _next;           // Next available char within buffer
   char       *_buf;            // buffer
   MEM_POOL   *_pool;

   UINT _A_Number_Of_Chunks(UINT size)
   {
      return (size <= 0? 0 : (((size/_chunk_size) + 1) * _chunk_size));
   }

   char *_Alloc(UINT size)
   {
      char *buf;

      if (size > 0)
      {
	 buf = CXX_NEW_ARRAY(char, size, _pool);
	 buf[0] = '\0';
      }
      else
	 buf = NULL;
      return buf;
   }

   BOOL _Is_Splitc(char splitc);

   void _Split();

public:

   // ********* Constructors **********

   ANL_CBUF(MEM_POOL *pool):
      _splitchars(""),
      _continuation(""),
      _max_linelength(UINT32_MAX),
      _linelength(0),
      _chunk_size(CBUF_SMALLEST_ALLOC_SIZE),
      _size(0),
      _next(0),
      _buf(NULL),
      _pool(pool)
   {}

   ANL_CBUF(MEM_POOL *pool, UINT chunk_size, UINT size): 
      _splitchars(""),
      _continuation(""),
      _linelength(0),
      _max_linelength(UINT32_MAX),
      _next(0),
      _pool(pool)
   {
      _chunk_size = (chunk_size > CBUF_SMALLEST_ALLOC_SIZE?
		     chunk_size : CBUF_SMALLEST_ALLOC_SIZE);
      _size = _A_Number_Of_Chunks(size);
      _buf = _Alloc(_size);
   }

   ANL_CBUF(MEM_POOL *pool, const char *s);


   // ********* Destructors **********

   ~ANL_CBUF()
   {
      if (_size > 0)
	 CXX_DELETE_ARRAY(_buf, _pool);
   }

   // ********* Modifications **********

   void Set_Linesplit(const char *continuation,
		      const char *splitchars,
		      mUINT32     max_linelength)
   {
      // Split lines at splitchars to enforce a line-length
      // of less than or equal to the given max-length.  Insert
      // the continuation char-sequence at split-points.
      //
      _splitchars = splitchars;
      _continuation = continuation; 
      _max_linelength = max_linelength;
   }

   void Reset_Linesplit()
   {
      _splitchars = "";
      _continuation = ""; 
      _max_linelength = UINT32_MAX;
   }

   void Reset() 
   {
      _next = 0;
   }

   void Reduce_Size(UINT by_amount)
   {
      _next = (by_amount > _next? 0 : (_next - by_amount));
      _buf[_next] = '\0';
   }

   void Write_Char(char c);

   void Write_String(const char *s);

   void Write_Int(INT64 i)
   {
      char s[32];

      sprintf(s, "%1lld", i);
      Write_String(s);
   }

   void Append_Pragma_Preamble(BOOL is_omp, BOOL lower_case);

   // ********* Inquiries **********

   UINT        Size() const {return _next;}
   const char *Chars() const {return _buf;} // Always '\0' terminated!

}; // ANL_CBUF


#endif /* anl_cbuf_INCLUDED */
