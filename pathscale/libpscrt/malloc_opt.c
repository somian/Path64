/*
 *  Copyright (C) 2008. Pathscale, LLC. All Rights Reserved.
 */

/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 *
 * Unpublished -- rights reserved under the copyright laws of the
 * United States. USE OF A COPYRIGHT NOTICE DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE. THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND
 * TRADE SECRETS OF PATHSCALE, INC. USE, DISCLOSURE, OR REPRODUCTION
 * IS PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF
 * PATHSCALE, INC.
 *
 * U.S. Government Restricted Rights:
 * The Software is a "commercial item," as that term is defined at 48
 * C.F.R. 2.101 (OCT 1995), consisting of "commercial computer software"
 * and "commercial computer software documentation," as such terms are used
 * in 48 C.F.R. 12.212 (SEPT 1995).  Consistent with 48 C.F.R. 12.212 and
 * 48 C.F.R. 227-7202-1 through 227-7202-4 (JUNE 1995), all U.S. Government
 * End Users acquire the Software with only those rights set forth in the
 * accompanying license agreement.
 *
 * QLogic Corporation
 * 2071 Stierlin Court, Suite 200
 * Mountain View CA 94043
 * USA
 */


#if defined(BUILD_OS_DARWIN)
#include <stdlib.h>
#else /* defined(BUILD_OS_DARWIN) */
#include <malloc.h>
#endif /* defined(BUILD_OS_DARWIN) */

// Use the alternate malloc algorithm indicated by ALG_NUM.
void
__pathscale_malloc_alg (int alg_num)
{
#if ! defined(BUILD_OS_DARWIN) /* Mallopt not supported */
  // Currently there are three alternate algorithms.  Bug 10736
  switch (alg_num){
   case 1:
   default:
       mallopt(M_MMAP_MAX, 2);
       mallopt(M_TRIM_THRESHOLD, 0x10000000);
       break;
   case 2:
       mallopt(M_MMAP_MAX, 2);
       mallopt(M_TRIM_THRESHOLD, 0x40000000);
       break;
   case 3:
       mallopt(M_MMAP_MAX, 0);
       mallopt(M_TRIM_THRESHOLD, -1);
       break;
  }       
#endif /* defined(BUILD_OS_DARWIN) */
}
