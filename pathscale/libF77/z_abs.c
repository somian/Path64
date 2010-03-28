/*
 * Copyright (C) 2009. PathScale, LLC. All Rights Reserved.
 */
/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
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


/* $Header: /home/bos/bk/kpro64-pending/libF77/z_abs.c 1.8 04/12/21 14:58:05-08:00 bos@eng-25.internal.keyresearch.com $ */

#include <math.h>
#include "cmplx.h"
#include "cmplrs/host.h"
#include "defalias.h"
#include "moremath.h"

#ifdef KEY
double __zabs(double zdreal, double zdimag)
#else
double __zabs(double zdreal, double_t zdimag)
#endif
{
  return(hypot(zdreal, zdimag));
}

double z_abs__(dcomplex *z)
{
  return(hypot(z->dreal, z->dimag));
}

#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
double z_abs_(dcomplex *z) { return z_abs__(z); }
double z_abs(dcomplex *z) { return z_abs__(z); }
#pragma weak z_abs
#else /* defined(BUILD_OS_DARWIN) */
defalias(z_abs__, z_abs_);
#ifdef KEY /* Bug 15104 */
weakalias(z_abs__, z_abs);
#else /* KEY Bug 15104 */
defalias(z_abs__, z_abs);
#endif /* KEY Bug 15104 */
#endif /* defined(BUILD_OS_DARWIN) */
