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


/* USMID @(#) libfi/matrix/mmx_c.h	92.1	10/28/98 22:50:56 */

/* This is a wrapper file for the matrix multiply routines */
#include <cray/dopevec.h>	/* description of Fortran 90 Dope Vector */
#include "intrin.h"
#include "mmx_dlutil.h"

extern void MMMM(DopeVectorType * R, DopeVectorType * A, DopeVectorType * B, void* blas);
extern void MMMV(DopeVectorType * R, DopeVectorType * A, DopeVectorType * B);
extern void MMVM(DopeVectorType * R, DopeVectorType * A, DopeVectorType * B);

void MMDD(DopeVectorType * R, DopeVectorType * A, DopeVectorType * B) 
{
  int ranka,rankb;

  ranka = A->n_dim;
  rankb = B->n_dim;

  if (ranka == 2 && rankb == 2) {
    __f90_open_blas_lib();
    MMMM(R,A,B,BLASROUTINE);
  } else if (ranka == 2 && rankb == 1) {
    MMMV(R,A,B);
  } else if (ranka == 1 && rankb == 2) {
    MMVM(R,A,B);
  } else {
    ERROR(FESCIRNK);
  }
}
