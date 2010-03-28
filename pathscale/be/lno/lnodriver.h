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
 * Module: lnodriver.h
 * $Revision$
 * $Date$
 * $Author$
 * $Source$
 *
 * Description:
 *  exported functions from lno.so
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef lnodriver_INCLUDED
#define lnodriver_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

class IPA_LNO_READ_FILE;
extern IPA_LNO_READ_FILE* IPA_LNO_File;

extern void lno_main (INT argc, char **argv, INT, char **);

extern void Lno_Init (void);

extern void Lno_Fini (void);

extern WN *Perform_Loop_Nest_Optimization (PU_Info*, WN *, WN *, BOOL);

#ifdef __cplusplus
}
#endif
#endif /* lnodriver_INCLUDED */
