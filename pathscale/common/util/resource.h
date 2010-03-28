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


#ifndef resource_INCLUDED
#define resource_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif



#ifdef _KEEP_RCS_ID
static char *resource_rcs_id = "$Source$ $Revision$";
#endif /* _KEEP_RCS_ID */

/* Request codes: */
typedef INT RES_REQUEST;

#define RR_Report_Current	1	/* Report current usage */
#define RR_Report_Delta		2	/* Report delta usage */
#define RR_Clear		3	/* Clear a delta summary */
#define RR_Start		4	/* Start a delta */
#define RR_Stop			5	/* Stop a delta */
#define RR_End			6	/* Transfer a delta to parent */
#define RR_Current_User		7	/* Return current user time */
#define RR_Current_System	8	/* Return current system time */
#define RR_Current_Elapsed	9	/* Return current elapsed time */
#define RR_Current_Memory	10	/* Return current memory usage */
#define RR_Delta_User		11	/* Return delta user time */
#define RR_Delta_System		12	/* Return delta system time */
#define RR_Delta_Elapsed	13	/* Return delta elapsed time */
#define RR_Delta_Memory		14	/* Return delta memory usage */

/* Typedefs */
typedef struct resources RESOURCES;
typedef struct rstate RSTATE;
typedef RSTATE *PSTATE;


/* Time structure: */
typedef struct time_info {
    INT secs;	/* Full seconds */
    INT usecs;	/* Fraction in microseconds */
} TIME_INFO;

/* External routines: */

/* Initialize base timer, etc.: */
extern void Resource_Init ( void );

/* Allocate a resource structure: */
extern RSTATE *Resource_Alloc ( const char *rname, RSTATE *parent );

/* Accumulate resource usage: */
extern void Resource_Accum ( RSTATE *r, RES_REQUEST req );


/* Return a timing record from r for caller use: */
extern TIME_INFO *Get_Time ( RSTATE *r, RES_REQUEST req );

/* Return memory usage from r for caller use: */
extern INT Get_Memory ( RSTATE *r, RES_REQUEST req );

/* Return the name of a timer for caller use: */
extern const char *Get_Timer_Name ( RSTATE *r );

/* Report resource usage from r (if non-NULL) or current usage,
 * according to the request function req and preceded by title:
 */
extern void Resource_Report (
    FILE *file,
    RES_REQUEST func,
    RSTATE *r,
    char *title
);

#ifdef __cplusplus
}
#endif
#endif /* resource_INCLUDED */
