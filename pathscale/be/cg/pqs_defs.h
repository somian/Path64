/* -*- c++ -*-
 *
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


#ifndef PQS_DEFS_included
#define PQS_DEFS_included

#include <stdio.h>
#include <set>
#include <algorithm>
#include <memory>

// Forward references
class PQS_MANAGER;
struct op;
struct tn;
typedef struct tn TN;
typedef struct op OP;

// Aliases
typedef OP * PQS_OP;
typedef TN * PQS_TN;

// Instruction classes. All predicate result generating things fit into one of these
// classes.
enum PQS_ITYPE {
   PQS_ITYPE_INVALID,
   PQS_ITYPE_NOPREDICATES,
   PQS_ITYPE_NORM,
   PQS_ITYPE_UNC,
   PQS_ITYPE_OR,
   PQS_ITYPE_AND,
   PQS_ITYPE_ORANDCM,
   PQS_ITYPE_ORCM,
   PQS_ITYPE_ANDCM,
   PQS_ITYPE_ANDORCM,
   PQS_ITYPE_DIVSQRT,
   PQS_ITYPE_LAST
};


// Relational operators on the compares
enum PQS_RELOPTYPE {
   PQS_RELOPTYPE_OTHER,
   PQS_RELOPTYPE_EQ,
   PQS_RELOPTYPE_NE
};


typedef INT32 PQS_NODE_FLAGS;

#define PQS_FLAG_CONDITION_TRUE  1       // The compare condition is always TRUE
#define PQS_FLAG_CONDITION_FALSE 2       // The compare condition is always FALSE

// Should be large enough to never be reused during a compilation
typedef INT32 PQS_MARKER_TYPE;

// Indices into PQS information arrays
typedef INT32 PQS_NODE_IDX;

#define PQS_IDX_NONE 0
#define PQS_IDX_TRUE -1
#define PQS_IDX_FALSE -2
#define PQS_IDX_INVALID -3
#define PQS_Is_Real_Idx(x) ((x)>0)

// enum for possible compare types
enum PQS_TRUTH {
   PQS_TRUTH_NEVER,
   PQS_TRUTH_ALWAYS,
   PQS_TRUTH_POSSIBLE,
   PQS_TRUTH_UNKNOWN
};


// The TN map. 
// The BOOLS are two bits to shut the compiler up.
struct PQS_TN_MAP_TYPE {
  PQS_TN        tn_to_use;            // The TN used in the PQS data structures. Allows copies to be used. 
                                      // If NULL, use the TN this map is attached to. 
  PQS_NODE_IDX  last_def;           // PQS_NODE_IDX of the last definition of the TN
  BOOL          used_as_qual_pred;  // has this been seen as a qualifying predicate?
  BOOL          no_query;           // Does this pass the rules required for sucessful use in the PQS?
  
  inline PQS_TN_MAP_TYPE() {
    last_def = PQS_IDX_INVALID;
    used_as_qual_pred = FALSE;
    no_query = FALSE;
    tn_to_use = NULL;
  }
};

//
// Set class template. Based on the STL set with some additional syntactic to make it 
// A little easier to use. 
//
template <class T, class C = std::less<T> >
class PQS_SET {
public:
#ifdef PQS_USE_MEMPOOLS
   typedef mempool_allocator<T> set_allocator_type;
#else
   typedef std::allocator<T> set_allocator_type; // for gcc 3.2
#endif
   typedef std::set<T,C,set_allocator_type> set_type;
   typedef typename set_type::iterator set_iterator_type;
   set_type _set;

   PQS_SET<T,C>() 
#ifdef PQS_USE_MEMPOOLS
      : _set(set_type::key_compare(),set_allocator_type(PQS_mem_pool))
#endif	
   {}
   
   inline static PQS_SET<T,C> Intersection(PQS_SET<T,C> &A,PQS_SET<T,C> &B)
   {
      PQS_SET<T,C> result;
      set_intersection(A._set.begin(),A._set.end(),B._set.begin(),B._set.end(),
		       inserter(result._set,result._set.begin()));
      return result;
   }
   
   inline static PQS_SET<T,C> Union(PQS_SET<T,C> &A,PQS_SET<T,C> &B)
   {
      PQS_SET<T,C> result;
      set_union(A._set.begin(),A._set.end(),B._set.begin(),B._set.end(),
		inserter(result._set,result._set.begin()));
      return result;
   }
   
   inline static PQS_SET<T,C> Diff(PQS_SET<T,C> &A,PQS_SET<T,C> &B)
   {
      PQS_SET<T,C> result;
      set_difference(A._set.begin(),A._set.end(),B._set.begin(),B._set.end(),
		     inserter(result._set,result._set.begin()));
      return result;
   }
   
   inline static BOOL Is_Empty(PQS_SET<T,C> &A) {
      return (A._set.empty());
   }

   inline BOOL Is_Subset(PQS_SET<T,C> &B) {
      return includes(_set.begin(),_set.end(),B._set.begin(),B._set.end());
   }

   inline BOOL Is_Subset(const T &B) {
      return (_set.count(B) != 0);
   }

   inline void Insert(const T &tn) {
      _set.insert(tn);
   }

   inline void Clear() {
      _set.erase(_set.begin(),_set.end());
   }

   inline void Clear(const T &tn) {
      _set.erase(tn);
   }

   inline INT32 Size(void) const {
      return _set.size();
   }

   inline set_iterator_type begin() const {return _set.begin();}
   inline set_iterator_type end() const {return _set.end();}

   void Print(FILE *f=stdout,BOOL newline=TRUE);
};


#endif

