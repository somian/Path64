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
#ifndef cxx_ipa_cprop_annot_INCLUDED
#define cxx_ipa_cprop_annot_INCLUDED

extern BOOL
Union_Formal_Cprop_Annot (IPA_NODE *callee, IPA_EDGE* e);

extern BOOL
Intra_PU_Formal_Cprop (IPA_NODE *node);

extern BOOL
Intra_PU_Global_Cprop (IPA_NODE* node);

extern void
Map_Global_Constants (IPA_NODE *node);


enum IPA_CLONING_ACTION 
{
  NEEDS_CLONING,
  ANNOTATION_CHANGED,
  NO_CHANGE
};

// Union of cprop annotations for a quasi clone and its origin
extern void 
Union_Quasi_Clone_Cprop_Annot (IPA_NODE* origin, IPA_NODE* clone);

// Union of cprop annotations enhanced by cloning
extern IPA_CLONING_ACTION 
Union_Formal_Cprop_Annot_With_Cloning (IPA_NODE*, IPA_EDGE*);

// Check if two edges are equivalent wrt to their cprop annotations
extern BOOL 
Edges_Have_Equiv_Cprop_Annots (const IPA_EDGE*, const IPA_EDGE*);

#endif // cxx_ipa_cprop_annot_INCLUDED
