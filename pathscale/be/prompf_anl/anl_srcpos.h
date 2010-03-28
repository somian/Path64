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
#ifndef anl_srcpos_INCLUDED
#define anl_srcpos_INCLUDED

// ==============================================================
// ==============================================================
//
// Module: anl_srcpos.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// Description:
//
//    An object oriented interface to srcpos utilities.  Note that
//    all relational comparison operators return FALSE for different
//    Filenum(), since we assume no order between files.
//
// ==============================================================
// ==============================================================

#include "srcpos.h"

class ANL_SRCPOS
{
private:

   USRCPOS _usrcpos;

public:

   // ============== Constructors & Destructors ==============

   ANL_SRCPOS()              {USRCPOS_srcpos(_usrcpos) = 0;}
   ANL_SRCPOS(SRCPOS srcpos) {USRCPOS_srcpos(_usrcpos) = srcpos;}
   ANL_SRCPOS(WN *stmt)      
   {
      if (stmt == NULL)
	 USRCPOS_srcpos(_usrcpos) = 0;
      else
	 USRCPOS_srcpos(_usrcpos) = WN_linenum(stmt);
   }


   // =============== Accessors ===============

   INT32 Filenum() const {return USRCPOS_filenum(_usrcpos);}
   INT32 Linenum() const {return USRCPOS_linenum(_usrcpos);}
   INT32 Column()  const {return USRCPOS_column(_usrcpos);}


   // =============== Writing to string buffer ===============

   void Write(ANL_CBUF *cbuf);
   void Write_Filenum(ANL_CBUF *cbuf) const {cbuf->Write_Int(Filenum());}
   void Write_Linenum(ANL_CBUF *cbuf) const {cbuf->Write_Int(Linenum());}
   void Write_Column(ANL_CBUF *cbuf)  const {cbuf->Write_Int(Column());}


   // =============== Assignment Operators ===============

   ANL_SRCPOS &operator=(const ANL_SRCPOS &s)
   {
      _usrcpos.srcpos = s._usrcpos.srcpos;
      return *this;
   }
   ANL_SRCPOS &operator+=(INT32 lineincr) 
   {
      USRCPOS_linenum(_usrcpos) += lineincr;
      return *this;
   }
   ANL_SRCPOS &operator-=(INT32 linedecr) 
   {
      USRCPOS_linenum(_usrcpos) -= linedecr;
      return *this;
   }

   // =============== Comparison Operators ===============

   
   BOOL operator==(const ANL_SRCPOS &s)
   {
      return s._usrcpos.srcpos == _usrcpos.srcpos;
   }
   BOOL operator!=(const ANL_SRCPOS &s)
   {
      return s._usrcpos.srcpos == _usrcpos.srcpos;
   }
   BOOL operator<(const ANL_SRCPOS &s);
   BOOL operator<=(const ANL_SRCPOS &s);
   BOOL operator>(const ANL_SRCPOS &s);
   BOOL operator>=(const ANL_SRCPOS &s);

}; // class ANL_SRCPOS

#endif // anl_srcpos_INCLUDED
