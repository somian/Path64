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
/* ====================================================================
 * ====================================================================
 *
 * Module: ipa_feedback.h
 *
 * Revision history:
 *  15-Sep-97 - Original Version
 *
 * Description:
 *
 * Define the external interfaces for the IPA static analysis feedback
 * modul.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ipa_feedback_INCLUDED
#define ipa_feedback_INCLUDED


// Main output file(s) for feedback info
extern FILE *IPA_Feedback_exl_fd;
extern FILE *IPA_Feedback_dve_fd;
extern FILE *IPA_Feedback_dfe_fd;
extern FILE *IPA_Feedback_prg_fd;
extern FILE *IPA_Feedback_con_fd;

// for string table + hash table manipulation
#define FBK_HASH_TAB_SIZE 128
#define FBK_STR_POOL_INIT_SIZE 4096



// ====================================================================
//
// IPA_feedback: Master IPA static analysis tool (feedback) classes
//
// ====================================================================

class IPA_FEEDBACK_STRINGS
{
private:

 mINT32 _str_pool_max;        // Maximum (allocated) size
 mINT32 _str_pool_size;       // Current (used) size
 MEM_PTR _str_pool_base;      // Base of string pool
 MEM_POOL *_str_pool;         // memory pool for strings

public:

  // constructor
  IPA_FEEDBACK_STRINGS ( MEM_POOL * str_pool );

  // Emit identifier to the feedback string table.
  INT32 Emit_id_string ( const char *name );

  // Add non-duplicate string to the feedback string table.
  INT32 Add_id_string ( const char *name );

};

class IPA_FEEDBACK_HASHES
{
private:

public:

  // constructor
  IPA_FEEDBACK_HASHES ( MEM_POOL * str_hash_tbl );
};

#endif /* ipa_feedback_INCLUDED */
