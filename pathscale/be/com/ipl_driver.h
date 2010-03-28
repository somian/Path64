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


#ifndef ipl_driver_INCLUDED
#define ipl_driver_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

extern void
ipl_main (INT ipl_argc, char **ipl_argv);

extern void
Ipl_Init (void);

extern void 
Ipl_Init_From_Ipa (MEM_POOL* pool);

extern void
Perform_Procedure_Summary_Phase (WN* w, struct DU_MANAGER *du_mgr,
				 struct ALIAS_MANAGER *alias_mgr,
				 void* emitter );

extern void 
Ipl_Fini (void);

extern void 
Ipl_Extra_Output (struct output_file *);


#ifdef __cplusplus
}
#endif
#endif /* ipl_driver_INCLUDED */
