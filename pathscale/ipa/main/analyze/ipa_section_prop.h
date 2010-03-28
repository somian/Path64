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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */

#ifndef cxx_ipa_section_prop_INCLUDED
#define cxx_ipa_section_prop_INCLUDED

extern MEM_POOL IPA_array_prop_pool;
extern BOOL Trace_IPA_Sections;

//------------------------------------------------------------
// DESCR:  perform scalar EUSE and KILL analysis
//------------------------------------------------------------
class IPA_SCALAR_MUST_DF_FLOW : public IPA_DATA_FLOW
{
protected:
  virtual void  InitializeNode(void* n);
  virtual void* Meet(void* in, void* vertex, INT *change);
  virtual void* Trans(void* in, void* out, void* vertex, INT *change);
  virtual void  PostProcessIO(void* n);

public:
  IPA_SCALAR_MUST_DF_FLOW(IPA_CALL_GRAPH *cg, DF_DIRECTION ddf, MEM_POOL *m);
  virtual void Print_entry(FILE* fp, void*, void* n);
};

//------------------------------------------------------------
// DESCR: perform array MOD/REF analysis
//------------------------------------------------------------
class IPA_ARRAY_DF_FLOW: public IPA_DATA_FLOW
{
protected:
  virtual void  InitializeNode(void* n);
  virtual void* Meet(void* in, void* vertex, INT* change);
  virtual void* Trans(void* in, void* out, void* vertex, INT* change);
  virtual void  PostProcessIO(void* n);

public:
  IPA_ARRAY_DF_FLOW(IPA_CALL_GRAPH* cg, DF_DIRECTION ddf, MEM_POOL* m);
  virtual void Print_entry(FILE* fp, void*, void* n);
};

//--------------------------------------------------------------------
// Propagate formals that are used as symbolic terms in array sections
//--------------------------------------------------------------------
class IPA_FORMALS_IN_ARRAY_SECTION_DF: public IPA_DATA_FLOW
{
protected:
  virtual void* Meet(void* in, void* vertex, INT* change);
  virtual void* Trans(void* in, void* out, void* vertex, INT* change);

public:
  IPA_FORMALS_IN_ARRAY_SECTION_DF(IPA_CALL_GRAPH* cg, 
                                  DF_DIRECTION ddf, 
                                  MEM_POOL* m);
  virtual void Print_entry(FILE* fp, void* out, void* vertex);
};

#endif // cxx_ipa_section_INCLUDED



