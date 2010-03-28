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


#pragma ident "@(#) libfi/matrix/MATMUL_S4S4dispatch.c	92.2	07/09/99 15:18:08"

#define MMMM _MM_MATMUL_S4S4
#define MMMV _MV_MATMUL_S4S4
#define MMVM _VM_MATMUL_S4S4
#define MMDD _MATMUL_S4S4
#define BLASROUTINE __f90_sgemm_blas
#include "mmx_c.h"
