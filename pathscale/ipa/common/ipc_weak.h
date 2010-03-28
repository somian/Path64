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


/* ====================================================================
 * ====================================================================
 *
 * Module: ipc_weak.h
 * $Source: /home/bos/bk/kpro64-pending/ipa/common/SCCS/s.ipc_weak.h $
 *
 * Revision history:
 *  14-Jun-95 - Original Version
 *  12-Apr-96 - Suppress C++ warnings
 *
 * Description:
 *
 *  Declare symbols defined in ld and referenced in ipa.so weak so that
 *  ipa.so will link cleanly.
 *
 * TODO:  C++ doesn't yet recognize #pragma weak, so we suppress this
 * all for C++ compilations.  Reinstate when C++ supports them.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ipc_weak_INCLUDED
#define ipc_weak_INCLUDED

#ifdef __PASS1_H__
#pragma weak used_gp_area
#endif /* __PASS1_H__ */

#ifdef __IPA_OPTION_H__
#pragma weak ld_ipa_opt
#pragma weak ipacom_flags
#pragma weak WB_flags
#pragma weak Y_flags
#endif /* __OPTION_H__ */

#ifdef __LD_ERROR_H__
#pragma weak msg
#endif /* __LD_ERROR_H__ */

#ifdef __EXT_TBL_H__
#pragma weak merge_ext
#pragma weak enter_mext
#pragma weak slookup_mext
#pragma weak slookup_mext_idx
#pragma weak get_mext
#pragma weak ext_tbl

#pragma weak ld_slookup_mext

#endif /* __EXT_TBL_H__ */

#ifdef __OBJ_FILE_H__
#pragma weak num_ir
#pragma weak get_next_ir
#pragma weak is_archive_member
#endif /* __OBJ_FILE_H__ */

#ifdef __PROCESS_H__
extern int create_tmpdir (int) __attribute__((weak));
extern string create_unique_file (const string, char) __attribute__((weak));
extern string create_tmp_file (const string) __attribute__((weak));
extern void add_to_tmp_file_list (string) __attribute__((weak));
extern string tmpdir __attribute__((weak));
extern string *get_command_line (an_object_file_ptr, string, string, int*) __attribute__((weak));
extern int make_link (const string, const string) __attribute__((weak));
#endif /* __PROCESS_H__ */

#ifdef __LD_UTIL_H__
#pragma weak concat_names
#endif /* __LD_UTIL_H__ */

#ifdef __LD_MAIN_H__
extern int arg_count __attribute__((weak));	// TK debug
#pragma weak arg_vector
#pragma weak environ_vars
#pragma weak max_gpa_size
#endif /* __LD_MAIN_H__ */

#ifdef __READ_H__
#pragma weak read_one_section
#pragma weak read_headers
#pragma weak unread_sections
#pragma weak unread_obj
#pragma weak objs_mapped_total_size
#pragma weak copy_section
#endif /* __READ_H__ */

#ifdef __DEM_H__
#pragma weak always_demangle
#endif /* __DEM_H__ */

#ifdef __ELFHASH_H
#pragma weak elfhash
#endif /* __ELFHASH_H */

#endif /* ipc_weak_INCLUDED */
