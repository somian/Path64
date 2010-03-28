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


// -*-C++-*-

/**                     INFIN
***                     ----
***
*** Description:
***
***     32 bit integers with infinite.
***
***
***
*** Exported type
***
***     INT32_INFIN
***
***		A data structure that is either a 32 bit integer or
***		plus/minus infinite.  This is represented as a 32 bit
***		integer but the operators are redefined so that its
***		maximum and minium values are treated as +- infinite
***		respectively.  Note: As with C integers, overflow
***		of non-infinite numbers is not checked and can cause 
***		unpredictable results.
***
***	Exported functions
***
***  		INT32_INFIN(const INT32 i)
***			Create a INT32_INFIN with value i
***  		BOOL Is_Infinite() const
***  		void Set_Infinite()
***  		BOOL Is_Neg_Infinite() const
***  		void Set_Neg_Infinite() 
***		INT32 Value() return the value.  Only valid if not +- infinite
***
***  		INT32_INFIN &operator =(const INT32_INFIN )
***  		INT32_INFIN operator +(const INT32_INFIN,const INT32_INFIN);
***  		INT32_INFIN operator -();
***  		INT32_INFIN operator -(const INT32_INFIN,const INT32_INFIN);
***  		INT32_INFIN operator *(const INT32_INFIN,const INT32_INFIN);
***
***  		BOOL operator >(const INT32_INFIN,const INT32_INFIN); 
***  		BOOL operator >=(const INT32_INFIN,const INT32_INFIN);
***  		BOOL operator <(const INT32_INFIN,const INT32_INFIN); 
***  		BOOL operator <=(const INT32_INFIN,const INT32_INFIN);
***  		BOOL operator ==(const INT32_INFIN,const INT32_INFIN);
***  		BOOL operator !=(const INT32_INFIN,const INT32_INFIN);
***
***  		void Print(const FILE *fp);
***			Print an INT32_INFIN
***/

/** $Revision$
*** $Date$
*** $Author$
*** $Source$
**/

#ifndef infin_INCLUDED
#define infin_INCLUDED "infin.h"

#ifdef _KEEP_RCS_ID
static char *infin_rcs_id = infin_INCLUDED "$Revision$";
#endif /* _KEEP_RCS_ID */

#ifndef defs_INCLUDED
#include "defs.h"
#endif
#ifndef errors_INCLUDED
#include "errors.h"
#endif

class INT32_INFIN {
  INT32 _val;
  enum {INFINITE = 2147483647,NEGINFINITE = -2147483647-1};
public:

  INT32_INFIN(const INT32 i) { _val = i; }
  INT32_INFIN() { _val = 0; }
  INT32_INFIN(const INT32_INFIN& i) { _val = i._val; }
  BOOL Is_Infinite() const { return(_val == INFINITE); }
  void Set_Infinite() { _val = INFINITE; }
  BOOL Is_Neg_Infinite() const { return(_val == NEGINFINITE); }
  void Set_Neg_Infinite() { _val = NEGINFINITE; }
  INT32 Value() { return _val; }

  INT32_INFIN &operator =(const INT32_INFIN i) { _val = i._val; return(*this);}
  friend inline INT32_INFIN operator +(const INT32_INFIN ,const INT32_INFIN) ;
  inline INT32_INFIN operator -() const;
  friend inline INT32_INFIN operator -(const INT32_INFIN, const INT32_INFIN) ;
  friend inline INT32_INFIN operator *(const INT32_INFIN, const INT32_INFIN) ;

  friend inline BOOL operator >(const INT32_INFIN, const INT32_INFIN) ;
  friend inline BOOL operator >=(const INT32_INFIN, const INT32_INFIN) ;
  friend inline BOOL operator <(const INT32_INFIN, const INT32_INFIN) ;
  friend inline BOOL operator <=(const INT32_INFIN, const INT32_INFIN) ;
  friend inline BOOL operator ==(const INT32_INFIN, const INT32_INFIN) ;
  friend inline BOOL operator !=(const INT32_INFIN, const INT32_INFIN) ;

  void Print(FILE *fp) const {
    if (this->Is_Infinite()) {
      fprintf(fp," infinite ");
    } else if (this->Is_Neg_Infinite()) {
      fprintf(fp," -infinite ");
    } else {
      fprintf(fp," %d ",_val);
    }
  }

};



inline INT32_INFIN operator +(const INT32_INFIN a, const INT32_INFIN b) 
{

  if (a.Is_Infinite()  || b.Is_Infinite()) {
    return INT32_INFIN::INFINITE; 
  } else if (a.Is_Neg_Infinite()  || b.Is_Neg_Infinite()) {
    return INT32_INFIN::NEGINFINITE; 
  } else {
    return(a._val+b._val);
  }
}

inline INT32_INFIN INT32_INFIN::operator -() const
{

  if (this->Is_Infinite()) { 
    return NEGINFINITE; 
  } else if (this->Is_Neg_Infinite()) {
    return INFINITE; 
  } else {
    return(-_val);
  }
}

inline INT32_INFIN operator -(const INT32_INFIN a, const INT32_INFIN b) 
{
  return(a + -b);
}

inline INT32_INFIN operator *(const INT32_INFIN a, const INT32_INFIN b) 
{
  INT64 prod = (INT64)a._val * (INT64)b._val;
  if (prod >= (INT64) INT32_INFIN::INFINITE) {
    return INT32_INFIN::INFINITE;
  } else if (prod <= (INT64) INT32_INFIN::NEGINFINITE) {
    return INT32_INFIN::NEGINFINITE;
  } else {
    return((INT32) prod);
  }
}

inline BOOL operator >(const INT32_INFIN a, const INT32_INFIN b) 
{
  return(a._val > b._val);
}

inline BOOL operator >=(const INT32_INFIN a, const INT32_INFIN b) 
{
  return(a._val >= b._val);
}

inline BOOL operator <(const INT32_INFIN a, const INT32_INFIN b) 
{
  return(a._val < b._val);
}

inline BOOL operator <=(const INT32_INFIN a, const INT32_INFIN b) 
{
  return(a._val <= b._val);
}

inline BOOL operator ==(const INT32_INFIN a, const INT32_INFIN b) 
{
  return(a._val == b._val);
}

inline BOOL operator !=(const INT32_INFIN a, const INT32_INFIN b) 
{
  return(a._val != b._val);
}


#endif



