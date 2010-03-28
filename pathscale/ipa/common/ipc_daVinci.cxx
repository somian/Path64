/*
 * Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
 */
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
 * Module: ipc_daVinci.cxx
 * $Revision: 1.3 $
 * $Date: 04/12/21 14:57:36-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/ipa/common/SCCS/s.ipc_daVinci.cxx $
 *
 * Description:
 *	interface to daVinci, a visualization system for displaying
 *	directed graphs.
 *
 * ====================================================================
 * ====================================================================
 */

#if defined(BUILD_OS_DARWIN)
#include <darwin_elf.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <elf.h>		    // for ipa_cg.h (Elf64_Word)
#endif /* defined(BUILD_OS_DARWIN) */
#include <sys/types.h>		    // for pid_t
#include <unistd.h>		    // for fork(), pipe(), etc.
#include <signal.h>		    // for SIGINT
#include <wait.h>		    // for waitpid()

#define USE_STANDARD_TYPES          /* override unwanted defines in defs.h */
#include "defs.h"
#include "errors.h"		    // for FmtAssert
#include "ip_graph.h"		    // for NODE_ITER and GRAPH
#include "ip_binding.h"		    // for ipc_file.h
#include "strtab.h"		    // for ipc_file.h
#include "stab.h"		    // for ipc_file.h
#include "const.h"		    // for ipc_file.h
#include "irbdata.h"		    // for ipc_file.h
#define USE_DST_INTERNALS
#include "dwarf_DST_mem.h"	    // for ipc_file.h
#include "ipc_file.h"		    // for ipa_cg.h
#include "pu_info.h"		    // for ipa_cg.h
#include "ipa_cg.h"		    // for IPA_CALL_GRAPH
#include "ipa_inline.h"		    // for IPA_NODE
#include "ipa_option.h"		    // for IPA_Enable_daVinci
#include "ipc_daVinci.h"

daVinci *cg_display = 0;

daVinci::daVinci (GRAPH *_g, MEM_POOL *_m)
{
    g = _g;
    m = _m;
    IPA_Enable_daVinci = display_ok = FALSE;
    to_display = from_display = NULL;

    if (isatty (1) == 0 && isatty (2) == 0)
	return;			    // none of stdout and stderr is a tty

    /* now connect to daVinci */
    int read_pipe[2], write_pipe[2];

    if (pipe (read_pipe) == -1 || pipe (write_pipe) == -1)
	return;

    from_display = fdopen (read_pipe[0], "r");
    setbuf (from_display, NULL);
    to_display = fdopen (write_pipe[1], "w");
    setbuf (to_display, NULL);

    switch (pid = fork ()) {
    case -1:			    // can't fork
	close (read_pipe[0]);
	close (read_pipe[1]);
	close (write_pipe[0]);
	close (write_pipe[1]);
	return;

    case 0:			    // child
	dup2 (write_pipe[0], 0);    // reset stdin, stdout, and stderr
	dup2 (read_pipe[1], 1);
	dup2 (read_pipe[1], 2);

	close (write_pipe[0]);	    // close unused pipes.
	close (read_pipe[1]);	    // leave the other pair opened so
				    // that daVinci does not die when
				    // ipa exits.

	execlp ("daVinci", "daVinci", "-pipe", 0);
	exit (1);		    // exec fail, just exit

    default:			    // parent
	close (read_pipe[1]);
	close (write_pipe[0]);
	wait_for ();
    }

    IPA_Enable_daVinci = TRUE;

} // daVinci::daVinci


/* wait for the "ok" message from daVinci */
void
daVinci::wait_for (const char *str)
{
    char buf[512];

    while (fgets (buf, 512, from_display) != NULL) {
	if (strcmp (buf, "ok\n") == 0) {
	    display_ok = TRUE;
	    return;
	} else {
	    switch (buf[0]) {
	    case 'n':		    // node* or no_*
	    case 'e':		    // edge*
	    case 'm':		    // menu*
	    case 'a':		    // answer*
		if (strncmp (buf, str, strlen(str)) == 0) {
		    display_ok = TRUE;
		    return;
		}
		break;
	    default:
		fputs (buf, stderr);
		cleanup ();
		return;
	    }
	}
    }

    cleanup ();
    return;
} // daVinci::wait_for


void
daVinci::cleanup (void)
{
    int stat;
    
    display_ok = FALSE;
    kill (pid, SIGINT);
    waitpid (pid, &stat, WNOHANG);  // capture any SIGCHLD so not to
				    // confuse ipacom.
    fclose (to_display);
    fclose (from_display);
} // daVinci::cleanup


// translate the subgraph into Term representation (use preorder traversal)
void
daVinci::Graph_To_Term (NODE_INDEX v, mUINT8 *visit)
{
    IPA_NODE *node = (IPA_NODE *) g->Node_User(v);
    static const char attr[] = "a(\"COLOR\",\"lightblue\")";

    visit[v] = 1;
    fprintf (to_display, "l(\"%d\",n(\"\",[a(\"OBJECT\",\"%s\"),%s],[",
	     v, node->Name(), attr); 
    
    NODE_ITER vitr(g, v);

    for (NODE_INDEX vi = vitr.First_Succ(); vi != -1; vi = vitr.Next_Succ()) {

	fprintf(to_display, "l(\"%d\",e(\"\",[],", vitr.Current_Edge_Index());
	if (visit[vi] == 0)
	    Graph_To_Term (vi, visit);
	else 
	    fprintf (to_display, "r(\"%d\")", vi);
	fprintf (to_display, ")),");
    }

    fprintf (to_display, "]))");
} // daVinci::Graph_To_Term


void
daVinci::Translate_Call_Graph (void)
{
    if (!display_ok)
	return;

    mUINT8 *visit = CXX_NEW_ARRAY (mUINT8, GRAPH_vmax (g), m);
    bzero (visit, sizeof(mUINT8)*GRAPH_vmax(g));

    fprintf (to_display, "new_term([");
    NODE_ITER vitr(g, GRAPH_root(g));
    for (NODE_INDEX vi = vitr.First_Succ(); vi != -1; vi = vitr.Next_Succ()) {
	if (visit[vi] == 0) {
	    Graph_To_Term (vi, visit);
	    fprintf (to_display, ",");
	}
    }
    fprintf (to_display, "])\n");

    CXX_DELETE (visit, m);

    wait_for ();
} // daVinci::Translate_Call_Graph 


void
daVinci::Mark_Used (NODE_INDEX v)
{
    
    if (!display_ok)
	return;
    
    fprintf (to_display, "change_node_color(\"%d\",\"red\")\n", v);
    wait_for ();
} // daVinci::Mark_Used (NODE_INDEX v)


void
daVinci::Mark_Deleted (NODE_INDEX v)
{
    
    if (!display_ok)
	return;
    
    fprintf (to_display, "change_node_color(\"%d\",\"lightgray\")\n", v);
    wait_for ();
} // daVinci::Mark_Deleted (NODE_INDEX v)


void
daVinci::Mark_Inlined_Caller (NODE_INDEX v)
{
    if (!display_ok)
	return;
    
    fprintf (to_display, "change_node_color(\"%d\",\"violet\")\n", v);
    wait_for ();
} // daVinci::Mark_Inlined_Caller


void
daVinci::Mark_Inlined_Callee (NODE_INDEX v)
{
    if (!display_ok)
	return;
    
    fprintf (to_display, "change_node_color(\"%d\",\"lightred\")\n", v);
    wait_for ();
} // daVinci::Mark_Inlined_Callee


void
daVinci::Mark_Inlined_Deleted (NODE_INDEX v)
{
    if (!display_ok)
	return;
    fprintf (to_display, "select_nodes_labels([\"%d\"])\n", v);
    wait_for ("node_selections_labels");
    fprintf (to_display, "hide_or_show_edges\n");
    wait_for ();
} // daVinci::Mark_Inlined_Deleted
