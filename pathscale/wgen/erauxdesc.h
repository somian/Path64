/*
 * Copyright (C) 2006. QLogic Corporation. All Rights Reserved.
 */

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
 *
 * Module: erauxdesc.h 
 * $Revision: 1.5 $
 * $Date: 04/12/21 14:57:52-08:00 $
 * $Author: bos@eng-25.internal.keyresearch.com $
 * $Source: /home/bos/bk/kpro64-pending/kgccfe/SCCS/s.erauxdesc.h $
 *
 * Revision history:
 *  19-Mar-92 - Original Version
 *
 * Description:
 *
 * Interface definition for the process program-specific portions of the
 * error handling package in util/errors.c.  Contains:
 *
 * 	1. includes of the back-end process specific error codes
 *
 * ====================================================================
 * ====================================================================
 */


#ifndef erauxdesc_INCLUDED
#define erauxdesc_INCLUDED

static char *eraux_desc_rcs_id = "$Source: /home/bos/bk/kpro64-pending/kgccfe/SCCS/s.erauxdesc.h $ $Revision: 1.5 $";

#include "erfe.desc"		/* Front end error codes */

#endif

