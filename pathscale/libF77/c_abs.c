/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2.1 of the GNU Lesser General Public License 
  as published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU Lesser General Public 
  License along with this program; if not, write the Free Software 
  Foundation, Inc., 59 Temple Place - Suite 330, Boston MA 02111-1307, 
  USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


/*  $Header: /home/bos/bk/kpro64-pending/libF77/c_abs.c 1.6 04/12/21 14:58:01-08:00 bos@eng-25.internal.keyresearch.com $ */
#include <math.h>
#include "moremath.h"
#include "cmplx.h"
#include "defalias.h"

float __c8abs(float zreal, float zimag)
{
  return(hypotf(zreal, zimag));
}

float c_abs__(float z[2])
{
  return(hypotf(z[0], z[1]));
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
float c_abs_(float z[2]) { return c_abs__(z); }
float c_abs(float z[2]) { return c_abs__(z); }
#else /* defined(BUILD_OS_DARWIN) */
defalias(c_abs__, c_abs_);
defalias(c_abs__, c_abs);
#endif /* defined(BUILD_OS_DARWIN) */