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


/* $Header$ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmplrs/host.h"

extern void f77fatal (int32, char *);

#define BUFSIZE	((size_t)8192)
typedef struct {
	struct bufstruct *next;
	size_t len_so_far;
    } OVERHEAD;

typedef char BUF[ BUFSIZE - sizeof(OVERHEAD) ];

typedef struct bufstruct {
        OVERHEAD header;
	BUF      buffer;
    } KAI_MARKED_HEAP;


/* The following three variables need to be in processor-private memory */
static KAI_MARKED_HEAP *KAI_heap_stack = NULL;
static size_t len_used = 0;

size_t _kai_mhalloc_tos_ = 0;

/* The linkage name "_kai_mhalloc_tos_" is shared with Fortran-generated object codes.	*/
/* Restoring a previously saved value of _kai_mhalloc_tos_ sets up a lazy "deallocate"	*/
/* of all the regions mhalloc'ed since that value was saved.  This discipline is often	*/
/* described as a "marked heap". 							*/

/* The following routine allocates len bytes on this processor's marked heap.		*/
/* The byte beyond the mhalloc'ed region is ascii NUL.					*/

char *
kai_mhalloc( size_t len )
{
    size_t		n;
    char	       *result;
    KAI_MARKED_HEAP    *nxtbuf;
    
    if (_kai_mhalloc_tos_ < len_used) {
	while (_kai_mhalloc_tos_ < KAI_heap_stack->header.len_so_far) {
	    nxtbuf = KAI_heap_stack->header.next;
	    free( KAI_heap_stack );
	    KAI_heap_stack = nxtbuf;
	  }
	len_used = _kai_mhalloc_tos_;
      }
    
    len++; /* Put ascii NUL at end to help F77/C interfaces */
    if (KAI_heap_stack) {
	n = len_used - KAI_heap_stack->header.len_so_far;
	if (len + n <= sizeof( BUF )) {
	    result = KAI_heap_stack->buffer + n;
	  } else {
	    n = len + sizeof( OVERHEAD ); /* shipping and handling */
	    nxtbuf = (KAI_MARKED_HEAP *) malloc( n < BUFSIZE ? BUFSIZE : n );
	    if (!nxtbuf) f77fatal(113,"mhalloc");
	    len_used++; /* freedom isn't free */
	    nxtbuf->header.next = KAI_heap_stack;
	    nxtbuf->header.len_so_far = len_used;
	    KAI_heap_stack = nxtbuf;
	    result = KAI_heap_stack->buffer;
	  }
      } else {
	len_used = (len > sizeof( BUF )); /* Permit leading big BUF to be free'd */
	if (len_used) {
	    n = len + sizeof( OVERHEAD ); /* shipping and handling */
	    KAI_heap_stack = (KAI_MARKED_HEAP *) malloc( n );
	  } else {
	    KAI_heap_stack = (KAI_MARKED_HEAP *) malloc( BUFSIZE );
	  }
	if (!KAI_heap_stack) f77fatal(113,"mhalloc");
	KAI_heap_stack->header.next = NULL;
	KAI_heap_stack->header.len_so_far = len_used;
	result = KAI_heap_stack->buffer;
      }

    len_used += len;
    *(result + len) = '\0';

    _kai_mhalloc_tos_ = len_used;
    return result;
}


string 
s_cat_kai( string strings[], fsize_t lengths[], int32 count, fsize_t *result_len)
{
    int32		i;
    size_t		len = 0;
    string	        tmpbuf;
    string	        result;
    
    i = count;
    while (i--) len += lengths[i];
    *result_len = len;

    tmpbuf = result = kai_mhalloc( len );

    for (i = 0; i < count ; ++i) {
	memcpy (tmpbuf, strings[i], lengths[i]);
	tmpbuf += lengths[i];
      }

    return result;
}

