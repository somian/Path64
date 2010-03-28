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
#ifndef anl_file_mngr_INCLUDED
#define anl_file_mngr_INCLUDED

// ==============================================================
// ==============================================================
//
// Module: anl_file_mngr.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Description:
//
//    A file handler, which reports error messages using an abstraction
//    named DIAGNOSTICS.  The interface of DIAGNOSTICS is expected to
//    include the following:
//
//         Warning: (char *) -> void
//         Error:   (char *) -> void
//         Fatal:   (char *) -> void
//
//    where a Fatal call is not expected to return, but is expected to
//    terminate execution.  An Error or a Warning will return.  The state
//    after an error may not be as expected.
//
//    This implementation is based on the <stdio.h> facilities and 
//    exports only what we currently need.  The interface can be 
//    extended and generalized to fit any future needs.  Intended 
//    use for reading from a file (similar for writing to a file):
//
//       ANL_FILE_MNGR file_mngr(diag);
//       file_mngr.Open_Read(filename);
//       while (!file_mngr.Error_Status() && !file_mngr.End_Of_File())
//       {
//          char c = file_mngr.Read_Char();
//          .... c ...
//       }
//       file_mngr.Close_File();
//
//    This abstraction does not buffer output beyond what is provided
//    by default through the <stdio.h> utilities, so any such buffering
//    must occur outside of this abstraction.  Such buffering may be
//    desirable, since this abstraction will perform fairly extensive
//    tests on all IO operations.
//
// ==============================================================
// ==============================================================


class ANL_DIAGNOSTICS;

class ANL_FILE_MNGR
{
private:

   ANL_DIAGNOSTICS *_diag;
   const char      *_name;
   FILE            *_file;
   INT32            _next_ch;

   static const INT _obuf_size = 513;    // Size of output buffer
   INT              _next_obuf;          // Next available char in _obuf
   char             _obuf[_obuf_size+1]; // Output buffer

   static void _Concat(char       *buf,
		       INT         max_chars,
		       const char *string[],
		       INT         num_strings);

   static UINT64 _Get_Decimal_Number(INT ch);
   static UINT64 _Get_Hex_Number(INT ch);
   static BOOL   _Exists(const char *name);

   void _General_Check(BOOL c, const char *proc_name, const char *msg);
   void _Not_Open_Check(const char *proc_name, const char *to_be_opened);
   void _Is_Open_Check(const char *proc_name);
   void _Overwrite_Warning(const char *proc_name, const char *filename);
   void _Write_Obuf();

public:

   ANL_FILE_MNGR(ANL_DIAGNOSTICS *diag):
     _diag(diag),
     _name(NULL),
     _file(NULL),
     _next_ch(EOF),
     _next_obuf(0)
   {}

   ~ANL_FILE_MNGR() { if (_file != NULL) Close_File(); }

   void Open_Read(const char *name);
   void Open_Create(const char *name);
   void Open_Append(const char *name);
   void Close_File();
   void Close_And_Remove_File();

   BOOL   File_Is_Open() {return (_file != NULL);}
   BOOL   End_Of_File() {return (_next_ch == EOF);}
   char   Peek_Char() {return _next_ch;}   // Does not alter stream ptr
   char   Read_Char();   // Returns current char and advances stream ptr
   UINT64 Read_Uint64(BOOL as_hex = FALSE);
   UINT64 Read_Ptr() {return Read_Uint64(TRUE);}
   void   Write_Char(char c);
   void   Write_String(const char *s);
   void   Flush_Write_Buffer() {if (_next_obuf > 0) _Write_Obuf();}

   const char *Name() const {return _name;}
   FILE       *File() {return _file;}
   
}; // ANL_FILE_MNGR


#endif /* anl_file_mngr_INCLUDED */
