//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_rviwn.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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
//
// ====================================================================
//
// Description:
//
// WN interface for (RVI) Register-Variable Identification
//
// ====================================================================
// ====================================================================


#ifndef opt_rviwn_INCLUDED
#define opt_rviwn_INCLUDED "opt_rviwn.h"
#ifdef _KEEP_RCS_ID
static char *opt_rviwnrcs_id = opt_rviwn_INCLUDED"$ $Revision$";
#endif /* _KEEP_RCS_ID */

#ifndef stab_INCLUDED
#include "stab.h"
#endif /* stab_INCLUDED */
#ifndef opt_rvitab_INCLUDED
#include "opt_rvitab.h"
#endif  // opt_rvitab_INCLUDED

// class to hold annotation information
//
class RVI_ANN : public SLIST_NODE {
  DECLARE_SLIST_NODE_CLASS(RVI_ANN);
private:
  ST		*_preg_st;	// the preg's st
  mINT32	 _preg;		// the preg to use
  RVI_NODE	*_rvi_node;	// the node this is for

  RVI_ANN( void );		// not used
  RVI_ANN(const RVI_ANN&);	// not used
  RVI_ANN& operator = (const RVI_ANN&);
public:
  RVI_ANN( ST *st, INT32 preg, RVI_NODE *rvi_node ) :
		_preg_st(st), _preg(preg), _rvi_node(rvi_node)
		{}
  ~RVI_ANN( void ) {}

  // access methods
  ST *Preg_st( void ) const
		{ return _preg_st; }
  INT32 Preg( void ) const
		{ return _preg; }
  RVI_NODE *Rvi_node( void ) const
		{ return _rvi_node; }
  IDX_32 Bitpos( void ) const
		{ return _rvi_node->Bitpos(); }

  // Get a TY_IDX associated with this preg
  TY_IDX Preg_ty( void ) const
    { return MTYPE_To_TY(TY_mtype(ST_type(Preg_st()))); }
  // create a new LDID of this preg
  WN *New_ldid( ALIAS_MANAGER *alias_mgr ) const;

  // print out an annotation
  void Print( FILE *fp = stderr ) const;
};


// singly linked list of RVI_ANNs
//
class RVI_ANN_LIST : public SLIST {
  DECLARE_SLIST_CLASS(RVI_ANN_LIST,RVI_ANN)
private:
  RVI_ANN_LIST(const RVI_ANN_LIST&);
  RVI_ANN_LIST& operator = (const RVI_ANN_LIST&);
public:
  ~RVI_ANN_LIST(void) {}	// destructor, use mempool

  // locate a bitpos in this list
  RVI_ANN *Find( const IDX_32 bitpos );

  // print out the annotation list
  void Print( FILE *fp = stderr );

}; // end of class RVI_ANN_LIST;


// class for iterating through lists of RVI_ANNs
//
class RVI_ANN_ITER : public SLIST_ITER {
  DECLARE_SLIST_ITER_CLASS(RVI_ANN_ITER, RVI_ANN, RVI_ANN_LIST)
public:
  ~RVI_ANN_ITER(void)	{}
};



#endif  // opt_rviwn_INCLUDED
