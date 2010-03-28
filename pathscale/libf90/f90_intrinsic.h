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


#include <cray/portdefs.h> 
#include <cray/dopevec.h> 
#include <liberrno.h>
#include <float.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>


#define MAX_NARY_DIMS MAXDIM   
#define BIGDEFAULTSZ 64

#define ERROR _lerror
#define FALSE 0
#define TRUE  1


/* descriptor accessors */

#define GET_RANK_FROM_DESC(array)       (array->n_dim) 
#define GET_EXTENT_FROM_DESC(array,dim) (array->dimension[dim].extent)
#define GET_ASSOCIATED_FROM_DESC(array) (array->assoc)
#define GET_ALEN_FROM_DESC(array)       (array->base_addr.a.el_len)
#define GET_DV_ASCII_FROM_DESC(array)   (array->type_lens.type == DVTYPE_ASCII)
#define GET_DV_LOGICAL_FROM_DESC(array) (array->type_lens.type == DVTYPE_LOGICAL)

#define GET_ADDRESS_FROM_DESC(array)   \
         ((array->type_lens.type == DVTYPE_ASCII) ?  \
          ((char *) _fcdtocp (array->base_addr.charptr))    :  \
          ((char *) array->base_addr.a.ptr)) 

#define GET_BYTEALIGNED_FROM_DESC(array) \
         ((array->type_lens.type == DVTYPE_ASCII) ||  \
	  (array->type_lens.type == DVTYPE_DERIVEDBYTE)) 

#define GET_ELEMENT_SZ_FROM_DESC(array) \
               ((array->type_lens.type == DVTYPE_ASCII)?  \
 		(_fcdlen(array->base_addr.charptr))    :  \
		((array->base_addr.a.el_len)>>3))    

static size_t GET_STRIDE_FROM_DESC(DopeVectorType * array, int32_t dim)
{
   size_t rval;
   rval = array->dimension[dim].stride_mult;
   switch (array->type_lens.type) {
   case DVTYPE_ASCII:
   case DVTYPE_DERIVEDBYTE:
      return (rval);

   case DVTYPE_DERIVEDWORD:
     return (rval << 2 );

    default:
      switch (array->type_lens.int_len) {
      case 8: 
	return (rval);

      case 16:
	return (rval << 1);

      default:
	return ( rval <<2 );
      }
   }
}

#define MK_STRIDE(byte_aligned,typ_sz)  \
     ((byte_aligned || typ_sz == 1) ?   \
	 (str_sz = typ_sz)          :   \
           ((typ_sz == 2)           ?   \
  	   (typ_sz/(BYTES_PER_WORD/2)) :\
	      (typ_sz/BYTES_PER_WORD)))


#define SET_RANK_IN_DESC(array,r)       (array->n_dim = r) 
#define SET_ADDRESS_IN_DESC(array,d)    (array->base_addr.a.ptr = d) 
#define SET_CHARPTR_IN_DESC(array,cr,sz) (array->base_addr.charptr = _cptofcd (cr, sz)) 
#define SET_LBOUND_IN_DESC(array,dim,b) (array->dimension[dim].low_bound = b)
#define SET_EXTENT_IN_DESC(array,dim,e) (array->dimension[dim].extent = e)
#define SET_STRMULT_IN_DESC(array,dim,e)(array->dimension[dim].stride_mult = e)
#define SET_ASSOCIATED_IN_DESC(array)   (array->assoc = TRUE)
#define SET_CONTIG_IN_DESC(array)       (array->a_contig = TRUE)
#define SET_ORIG_BS_IN_DESC(array,a)    (array->orig_base = a) 
#define SET_ORIG_SZ_IN_DESC(array,s)    (array->orig_size = s) 
#define SET_ALEN_IN_DESC(array,l)       (array->base_addr.a.el_len = l)


/* assume 1/0 for TRUE is set in byte for mask arguments. offset to correct byte */

#if defined(_LITTLE_ENDIAN)
#define OFFSET_TO_TF_BYTE(by) 0
#else
#define OFFSET_TO_TF_BYTE(by) (by-1)
#endif


/* definitions of types to match usage in generated files */

#define i8 int64_t 
#define i4 int32_t 
#define i2 int16_t 
#define i1 int8_t  

#define ui8 uint64_t 
#define ui4 uint32_t 
#define ui2 uint16_t 
#define ui1 uint8_t  

#define l8 uint64_t 
#define l4 uint32_t 
#define l2 uint16_t 
#define l1 uint8_t  

#define r16 long double
#define r8  double 
#define r4  float  

typedef struct { float r, i; } complex;
typedef struct { double r, i; } dcomplex;
typedef struct { long double r, i ; } qcomplex;

#define c32 qcomplex  
#define c16 dcomplex  
#define c8  complex  

#define ALIGNED_r16(address) (((size_t)address & 0x7) == 0)
#define ALIGNED_r8(address)  (((size_t)address & 0x7) == 0)
#define ALIGNED_i8(address)  (((size_t)address & 0x7) == 0)
#define ALIGNED_i4(address)  (((size_t)address & 0x3) == 0)
#define ALIGNED_r4(address)  (((size_t)address & 0x3) == 0)
#define ALIGNED_i2(address)  (((size_t)address & 0x1) == 0)
#define ALIGNED_i1(address)  (TRUE)
			     


