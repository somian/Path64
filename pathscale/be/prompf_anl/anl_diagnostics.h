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
#ifndef anl_diagnostics_INCLUDED
#define anl_diagnostics_INCLUDED

// ==============================================================
// ==============================================================
//
// Module: anl_diagnostics.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Description:
//
//    A class for diagnostics handling in prompf_anl.so.  Note
//    that the "diag_file" must be opened externally to this
//    class, and will not be explicitly closed before program
//    termination upon a fatal error.
//
// ==============================================================
// ==============================================================


class ANL_DIAGNOSTICS
{
private:

   BOOL  _error_condition;
   FILE *_diag_file;

public:

   ANL_DIAGNOSTICS(FILE *diag_file):
   _error_condition(FALSE),
   _diag_file(diag_file)
   {}

   void Warning(const char *msg);
   void Error(const char *msg);
   void Fatal(const char *msg);

   BOOL Error_Was_Reported() const {return _error_condition;}
   void Reset_Error_Condition() {_error_condition = FALSE;}

}; // ANL_DIAGNOSTICS


#endif /* anl_diagnostics_INCLUDED */
