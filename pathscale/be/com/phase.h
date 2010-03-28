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
 * Module: phase.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Revision history:
 *  17-Feb-95 - Original Version
 *
 * Description:
 *   Phase-specific functions and data structures that the common driver
 *   need to know.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef phase_INCLUDED
#define phase_INCLUDED

typedef enum {
    PHASE_COMMON,		    /* non-phase specific */
    PHASE_LNO,			    /* loop-nest optimizer */
    PHASE_WOPT,			    /* global optimizer  */
    PHASE_CG,			    /* code generator */
    PHASE_W2C,	                    /* whirl2c */
    PHASE_W2F,	                    /* whirl2f */
    PHASE_PURPLE,	            /* purple instrumenter */
    PHASE_IPL,			    /* ipl */
    PHASE_PROMPF,	            /* writing a prompf analysis file */
    PHASE_COUNT
} BE_PHASES;

typedef struct {
    const char *group_name;	    /* option group name */
    INT group_name_length;	    /* string length of group_name */
    BE_PHASES phase;		    /* the phase where this group belongs */
} PHASE_SPECIFIC_OPTION_GROUP;

extern PHASE_SPECIFIC_OPTION_GROUP phase_ogroup_table[];

#endif /* phase_INCLUDED */
