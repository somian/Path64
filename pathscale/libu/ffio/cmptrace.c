/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation version 2.1

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


#pragma ident "@(#) libu/ffio/cmptrace.c	92.2	06/29/99 13:16:47"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ffio.h>
#include <liberrno.h>
#include "cmpio.h"
 
static FILE *TraceFPtr  = NULL;		/* Trace file pointer */
static char *TraceFName = NULL;		/* Trace file name    */
static int   TraceCnt   = 0;		/* Opened files using */
					/* compression layer  */
static int   DbgOpenCnt = 0;		/* Trace file ID      */

/*
 * trace printing stuff
 */
 
int
_OpenTraceFile(stat, cinfo)
	struct ffsw	*stat;
	cmp_lyr		*cinfo;
{
	/*
	 * Assign an ID to this opened file. This is used to know 
	 * which debug message belongs to which file when looking 
	 * at the trace output.
	 */
	cinfo->DbgOpenCnt = DbgOpenCnt;
	DbgOpenCnt++;
	
	if (TraceFPtr != NULL) {
		TraceCnt++;
		TRC_LOG("Trace File '%s' already opened (%d)", 
			TraceFName, TraceCnt);
		return(0);
	}

	/*
	 * Check if the FF_CMP_TRACE_FILE environment variable is
	 * set, indicating the location of the trace file.
	 */
	if ((TraceFName = getenv("FF_CMP_TRACE_FILE")) != NULL) {
		if (strcmp(TraceFName, "stderr") == 0) {
			TraceFPtr = stderr;
		} else if (strcmp(TraceFName, "stdout") == 0) {
			TraceFPtr = stdout;
		} else if ((TraceFPtr = fopen(TraceFName, "w")) == NULL) {
			ERETURN(stat, FDC_ERR_TRACE_FILE, 0);
		}

		TraceCnt++;

		TRC_LOG("Opening Trace File '%s' (%d)", 
			TraceFName, TraceCnt);
	} else if (cinfo->DebugLvl > DEBUG_LVL1) {
		/*
		 * No trace file environment variable was given but we are
		 * running in debug mode. Then use standard out to print 
		 * out all of the trace messages.
		 */
		TraceFPtr = stdout;
		TraceFName = "stdout";
	}

	return(0);
}

void
_CloseTraceFile(cinfo)
	cmp_lyr	*cinfo;
{
	if ((TraceFPtr != stdout) && 
	    (TraceFPtr != stderr) && (TraceFPtr != NULL)) {
		TraceCnt--;

		/*
		 * Check to see how many file using this compression
		 * layer still remain open by looking at the value in
		 * 'TraceCnt'. As long as there is somebody open we
		 * want to keep the trace file open.
		 */
		if (TraceCnt <= 0) {
			TRC_LOG("Closing Trace File %s (%d)", 
				TraceFName, TraceCnt);
			fclose(TraceFPtr);
		} else {
			TRC_LOG("Decreased TraceCnt to %d", TraceCnt);
		}
	}
}

void
_TraceLeave(char *rtnName, cmp_lyr *cinfo)
{
	fprintf(TraceFPtr, "%d> %s: Leaving\n", 
		cinfo->DbgOpenCnt, rtnName);
	fflush(TraceFPtr);
}

void
_TraceEnter(char *rtnName, cmp_lyr *cinfo)
{
	fprintf(TraceFPtr, "%d> %s: Entering\n", 
		cinfo->DbgOpenCnt, rtnName);
	fflush(TraceFPtr);
}

void
_TraceLog(char *fmt, ...)
{
	va_list	vaPtr;	
	
	va_start(vaPtr, fmt);
	va_end(vaPtr);
	vfprintf(TraceFPtr, fmt, vaPtr);
	fprintf(TraceFPtr, "\n");
	fflush(TraceFPtr);
}
