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


#ifndef __IPC_LINK_H__
#define __IPC_LINK_H__

#ifdef __cplusplus
extern "C" {
#endif 

extern void
ipa_init_link_line (int argc, char **argv);

extern void
ipa_add_link_flag (const char *str);

extern void
ipa_add_comma_list (const char* name);

extern void
ipa_compose_comma_list (const char* name);

#ifdef _LD_IPA_INTERFACE

#pragma weak ipa_init_link_line
#pragma weak ipa_add_link_flag
#pragma weak ipa_add_comma_list
#pragma weak ipa_compose_comma_list

#endif /* _LD_IPA_INTERFACE */

#ifdef __cplusplus
}
#endif


#ifndef _LD_IPA_INTERFACE

/* These are NOT exported to ld */

#include <vector>

typedef std::vector<const char*> ARGV;

extern void
ipa_insert_whirl_obj_marker ();

extern ARGV *
ipa_link_line_argv (const ARGV* output_files,
                    const char* dir, const char* symtab_file);

// extern void process_cord_obj_list (FILE *);
#endif /* _LD_IPA_INTERFACE */


#endif /* __IPC_LINK_H__ */
