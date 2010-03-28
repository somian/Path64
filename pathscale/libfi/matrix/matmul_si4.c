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


#pragma ident "@(#) libfi/matrix/matmul_si4.c	92.1	07/09/99 15:18:08"

#include "matmul.h"

/*
 * Name of this entry point
 */
#define NAME _MATMUL_SI4
/*
 * Name of routine called do computation (if any)
 */
#if defined(UNICOS) || defined(UNIX_NAMING_CONVENTION)
#define SUBNAME _I4SGEMMX
#elif defined(BUILD_COMPILER_GNU) && defined(BUILD_OS_DARWIN)
#define SUBNAME underscore_i4sgemmx__
#else
#define SUBNAME _i4sgemmx__
#endif

/*
 * Type of constants alpha and beta
 */
#define RESULTTYPE	_f_real8

void
NAME(DopeVectorType *RESULT, DopeVectorType *MATRIX_A, 
	DopeVectorType *MATRIX_B)
{
    void    SUBNAME();
    MatrixDimenType matdimdata, *MATDIM;

    const RESULTTYPE   one =  (RESULTTYPE) 1.0;
    const RESULTTYPE   zero = (RESULTTYPE) 0.0;

        MATDIM = (MatrixDimenType *) &matdimdata;

    /*
     * Parse dope vectors, and perform error checking.
     */

    _premult(RESULT, MATRIX_A, MATRIX_B, MATDIM);

    /*
     * Do the transposed matrix multiplication C' = B'*A'
     */

    SUBNAME(&MATDIM->n, &MATDIM->m, &MATDIM->k, &one, MATDIM->B, &MATDIM->inc2b,
    	    &MATDIM->inc1b, MATDIM->A, &MATDIM->inc2a, &MATDIM->inc1a,
	    &zero, MATDIM->C, &MATDIM->inc2c, &MATDIM->inc1c);
    return;
}
