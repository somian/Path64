/* 
   Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
   File modified June 20, 2003 by PathScale, Inc. to update Open64 C/C++ 
   front-ends to GNU 3.2.2 release.
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
 * Module: c_int_model.h
 * $Revision: 1.7 $
 * $Date: 05/12/05 09:00:08-08:00 $
 * $Author: bos@eng-24.pathscale.com $
 * $Source: kg++fe/SCCS/s.c_int_model.h $
 *
 * Revision history:
 *  ??-???-92 - Original Version
 *
 * Description:
 *
 * External interface to C integer model configuration support.
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef c_int_model_INCLUDED
#define c_int_model_INCLUDED

#ifdef _KEEP_RCS_ID
static char *c_int_model_rcs_id = "$Source: kg++fe/SCCS/s.c_int_model.h $ $Revision: 1.7 $";
#endif /* _KEEP_RCS_ID */

#ifndef defs_INCLUDED
#include "defs.h"
#endif
#ifndef mtypes_INCLUDED
#include "mtypes.h"
#endif

/* Define the integer model choices.  WARNING:  There are tables in the
 * associated source file which depend on these value assignments.
 */
typedef enum {
  TARGET_INT_ILP32 = 0,
  TARGET_INT_LP64  = 1,
  TARGET_INT_ILP64 = 2
} TARGET_INT_MODEL;
extern TARGET_INT_MODEL Target_Int_Model;

#ifdef __cplusplus
extern "C" {
#endif

extern void Initialize_C_Int_Model ( void );
extern TYPE_ID FE_Int_Type_To_Mtype(int);
extern TYPE_ID FE_Pointer_Type_To_Mtype(void);
extern TYPE_ID FE_int_To_Mtype(void);
extern void Make_Int_Model_Consistent(void);

#ifdef __cplusplus
}
#endif

#endif /* c_int_model_INCLUDED */
