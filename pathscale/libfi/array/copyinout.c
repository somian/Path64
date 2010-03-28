/*
 * Copyright 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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

#include "f90_intrinsic.h"

#ifdef _DEBUG
#if defined(BUILD_OS_DARWIN)
/* Mach-O doesn't support aliases */
extern void _Copyin(void *, DopeVectorType *);
extern void _Copyout(DopeVectorType *, void *);
void copyin_(void *result, DopeVectorType *array) { _Copyin(result, array); }
void copyout_(DopeVectorType *dest, void *src) { _Copyout(dest, src); }
#else /* defined(BUILD_OS_DARWIN) */
extern void copyin_(void *, DopeVectorType *);
extern void copyout_(DopeVectorType *, void *);
#pragma weak copyin_ = _Copyin
#pragma weak copyout_ = _Copyout
#endif /* defined(BUILD_OS_DARWIN) */
#endif /* _DEBUG */

/*
 * Copy array section "array" to contiguous array "result". Modeled after
 * "pack".
 */
void 
_Copyin(
void	*result,
DopeVectorType	*array)
{
  char *result_p = (char *) result;

  size_t src_extent[MAX_NARY_DIMS];
  size_t src_stride[MAX_NARY_DIMS];
  size_t src_offset[MAX_NARY_DIMS];
  size_t counter[MAX_NARY_DIMS];

  size_t i;
  int32_t j;
  int32_t src_rank = GET_RANK_FROM_DESC(array) - 1;

  size_t typ_sz = GET_ELEMENT_SZ_FROM_DESC(array);

  int8_t  zero_szd_source = FALSE;

  size_t src_size = 1;

  for ( j = 0; j <= src_rank ; j ++  ) {
    src_extent[j] = GET_EXTENT_FROM_DESC(array,j);
    src_stride[j] = GET_STRIDE_FROM_DESC(array,j);
    src_size *= src_extent[j];
    counter[j] = 0;
    zero_szd_source = zero_szd_source || (src_extent[j] == 0);
  }

  for ( j = 1; j <= src_rank ; j ++  )
    src_offset[j-1] = src_stride[j] - (src_stride [j-1] * (src_extent[j-1]));

  int8_t byte_aligned  = GET_BYTEALIGNED_FROM_DESC(array);
  size_t tot_ext       = src_size;

  if (zero_szd_source)
    return;

  size_t a_size   = src_extent[0];
  size_t a_stride = src_stride[0];
  size_t r_stride = typ_sz;
  char *array_p = GET_ADDRESS_FROM_DESC(array);

  if (typ_sz == sizeof(i1) && ALIGNED_i1(array_p) &&  ALIGNED_i1(result_p)) {

    while (counter[src_rank] < src_extent[src_rank] ) {
      for ( i = 0; i < a_size ; i ++ ) {
	*(i1 *)result_p = *(i1 *)array_p;
	result_p += r_stride;
	array_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == src_extent[j]) && (j < src_rank)) {
	array_p += src_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  } else if (typ_sz == sizeof(i2) && ALIGNED_i2(array_p) &&  ALIGNED_i2(result_p) ) {

    while (counter[src_rank] < src_extent[src_rank] ) {
      for ( i = 0; i < a_size ; i ++ ) {
	*(i2 *)result_p = *(i2 *)array_p;
	result_p += r_stride;
	array_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == src_extent[j]) && (j < src_rank)) {
	array_p += src_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  } else if (typ_sz == sizeof(r4) && ALIGNED_r4(array_p) &&  ALIGNED_r4(result_p) ) {

    while (counter[src_rank] < src_extent[src_rank] ) {
      for ( i = 0; i < a_size ; i ++ ) {
	*(ui4 *)result_p = *(ui4 *)array_p;
	result_p += r_stride;
	array_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == src_extent[j]) && (j < src_rank)) {
	array_p += src_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  } else if (typ_sz == sizeof(r8) && ALIGNED_r8(array_p) &&  ALIGNED_r8(result_p) ) {

    while (counter[src_rank] < src_extent[src_rank] ) {
      for ( i = 0; i < a_size ; i ++ ) {
	*(ui8 *)result_p = *(ui8 *)array_p;
	result_p += r_stride;
	array_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == src_extent[j]) && (j < src_rank)) {
	array_p += src_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  } else if (typ_sz == sizeof(r16) && ALIGNED_r16(array_p) &&  ALIGNED_r16(result_p) ) {

    while (counter[src_rank] < src_extent[src_rank] ) {
      for ( i = 0; i < a_size ; i ++ ) {
	*(ui16 *)result_p = *(ui16 *)array_p;
	result_p += r_stride;
	array_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == src_extent[j]) && (j < src_rank)) {
	array_p += src_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  } else {
    while (counter[src_rank] < src_extent[src_rank] ) {
      for ( i = 0; i < a_size ; i ++ ) {
	char *ap = array_p;
	char *rp = result_p;
	if (typ_sz > BIGDEFAULTSZ)
	  (void) memcpy (rp, ap, typ_sz);
	else
	  for (j = 0; j < typ_sz ; j ++)  *rp++ = *ap ++ ;
	result_p += r_stride;
	array_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == src_extent[j]) && (j < src_rank)) {
	array_p += src_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  }
}

/*
 * Copy contiguous array "source" to array section "dest". Modeled after
 * "pack".
 */
void 
_Copyout(
DopeVectorType	*dest,
void	*source)
{
  char *source_p = (char *) source;

  size_t dest_extent[MAX_NARY_DIMS];
  size_t dest_stride[MAX_NARY_DIMS];
  size_t dest_offset[MAX_NARY_DIMS];
  size_t counter[MAX_NARY_DIMS];

  size_t i;
  int32_t j;
  int32_t dest_rank = GET_RANK_FROM_DESC(dest) - 1;

  size_t typ_sz = GET_ELEMENT_SZ_FROM_DESC(dest);

  int8_t  zero_szd_dest = FALSE;

  size_t dest_size = 1;

  for ( j = 0; j <= dest_rank; j ++  ) {
    dest_extent[j] = GET_EXTENT_FROM_DESC(dest,j);
    dest_stride[j] = GET_STRIDE_FROM_DESC(dest,j);
    dest_size *= dest_extent[j];
    counter[j] = 0;
    zero_szd_dest = zero_szd_dest || (dest_extent[j] == 0);
  }

  for ( j = 1; j <= dest_rank; j ++  )
    dest_offset[j-1] = dest_stride[j] - (dest_stride [j-1] * (dest_extent[j-1]));

  int8_t byte_aligned  = GET_BYTEALIGNED_FROM_DESC(dest);
  size_t tot_ext       = dest_size;

  if (zero_szd_dest)
    return;

  size_t a_size   = dest_extent[0];
  size_t a_stride = dest_stride[0];
  size_t s_stride = typ_sz;
  char *dest_p = GET_ADDRESS_FROM_DESC(dest);

  if (typ_sz == sizeof(i1) && ALIGNED_i1(dest_p) &&  ALIGNED_i1(source_p)) {

    while (counter[dest_rank] < dest_extent[dest_rank] ) {
      for ( i = 0; i < a_size; i ++ ) {
	*(i1 *)dest_p = *(i1 *)source_p;
	source_p += s_stride;
	dest_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == dest_extent[j]) && (j < dest_rank)) {
	dest_p += dest_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  } else if (typ_sz == sizeof(i2) && ALIGNED_i2(dest_p) &&  ALIGNED_i2(source_p) ) {

    while (counter[dest_rank] < dest_extent[dest_rank] ) {
      for ( i = 0; i < a_size; i ++ ) {
	*(i2 *)dest_p = *(i2 *)source_p;
	source_p += s_stride;
	dest_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == dest_extent[j]) && (j < dest_rank)) {
	dest_p += dest_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  } else if (typ_sz == sizeof(r4) && ALIGNED_r4(dest_p) &&  ALIGNED_r4(source_p) ) {

    while (counter[dest_rank] < dest_extent[dest_rank] ) {
      for ( i = 0; i < a_size; i ++ ) {
	*(ui4 *)dest_p = *(ui4 *)source_p;
	source_p += s_stride;
	dest_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == dest_extent[j]) && (j < dest_rank)) {
	dest_p += dest_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  } else if (typ_sz == sizeof(r8) && ALIGNED_r8(dest_p) &&  ALIGNED_r8(source_p) ) {

    while (counter[dest_rank] < dest_extent[dest_rank] ) {
      for ( i = 0; i < a_size; i ++ ) {
	*(ui8 *)dest_p = *(ui8 *)source_p;
	source_p += s_stride;
	dest_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == dest_extent[j]) && (j < dest_rank)) {
	dest_p += dest_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  } else if (typ_sz == sizeof(r16) && ALIGNED_r16(dest_p) &&  ALIGNED_r16(source_p) ) {

    while (counter[dest_rank] < dest_extent[dest_rank] ) {
      for ( i = 0; i < a_size; i ++ ) {
	*(ui16 *)dest_p = *(ui16 *)source_p;
	source_p += s_stride;
	dest_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == dest_extent[j]) && (j < dest_rank)) {
	dest_p += dest_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  } else {
    while (counter[dest_rank] < dest_extent[dest_rank] ) {
      for ( i = 0; i < a_size; i ++ ) {
	char *ap = dest_p;
	char *sp = source_p;
	if (typ_sz > BIGDEFAULTSZ)
	  (void) memcpy (ap, sp, typ_sz);
	else
	  for (j = 0; j < typ_sz; j ++)  *ap++ = *sp++;
	source_p += s_stride;
	dest_p += a_stride;
      }

      counter[0] = a_size;
      j = 0;
      while ((counter[j] == dest_extent[j]) && (j < dest_rank)) {
	dest_p += dest_offset[j];
	counter[j+1]++;
	counter[j] = 0;
	j ++;
      }
    }

  }
}
