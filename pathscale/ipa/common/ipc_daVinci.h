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
 * Module: ipc_daVinci.h
 * $Revision: 1.3 $
 * $Date: 04/12/21 14:57:36-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/ipa/common/SCCS/s.ipc_daVinci.h $
 *
 * Description:
 *	Class definition for the interface to daVinci.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ipc_daVinci_INCLUDED
#define ipc_daVinci_INCLUDED

class daVinci {

private:
    
    MEM_POOL *m;
    GRAPH *g;
    BOOL display_ok;
    pid_t pid;

    FILE *to_display, *from_display;

    void Graph_To_Term (NODE_INDEX, mUINT8 *);

    void wait_for (const char *str = "ok\n");

    void cleanup ();
    
public:

    daVinci (GRAPH *, MEM_POOL *);
    
    void Translate_Call_Graph (void);

    void Mark_Used (NODE_INDEX);
    void Mark_Deleted (NODE_INDEX);
    void Mark_Inlined_Caller (NODE_INDEX);
    void Mark_Inlined_Callee (NODE_INDEX);
    void Mark_Inlined_Deleted (NODE_INDEX);

}; // daVinci

extern daVinci *cg_display;

#endif /* ipc_daVinci_INCLUDED */
