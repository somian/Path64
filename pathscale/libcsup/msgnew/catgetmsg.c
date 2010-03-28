#pragma ident "@(#)92/msgnew/catgetmsg.c	92.2	06/03/99 09:59:18"

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


#ident  "$Header$"

#if ! defined(BUILD_OS_DARWIN)
/* Mach-O doesn't implement aliases; Linux version ignores these anyway
 * because there's no C declaration of the alias id */
#if defined(__STDC__) && defined(_LITTLE_ENDIAN)
	#pragma weak _catgetmsg = catgetmsg
#else				/* else STDC and _LITTLE_ENDIAN */
#if defined(__STDC__) && !defined(_LIBU)
        #pragma weak catgetmsg = _catgetmsg
        #pragma weak __catgetmsg_error_code = ___catgetmsg_error_code
#endif
#endif				/* end STDC and _LITTLE_ENDIAN */
#endif /* ! defined(BUILD_OS_DARWIN) */

/*
 * IMPORTANT:
 * This section is needed since this file also resides in the compilers'
 * libcsup/msg (v7.2 and higher). Once the compilers drop support for
 * pre-IRIX 6.5 releases this can be removed. Please build a libu before
 * checking in any changes to this file.
 *
 */

#ifdef	_LITTLE_ENDIAN
#include <nl_types.h>
#include <cray/nlcatmsg.h>
#include <string.h>
#else				/* Else _LITTLE_ENDIAN */
#ifndef _LIBU
#include "synonyms.h"
#endif

#define __NLS_INTERNALS 1
#include <nl_types.h>
#undef __NLS_INTERNALS
#include <string.h>
#endif				/* End _LITTLE_ENDIAN */

/*
 * catgetmsg -- retrieves a message to a user supplied buffer from a catalog
 *
 */
char *
catgetmsg(
	  nl_catd catd,
	  int set_num,
	  int msg_num,
	  char *buf,
	  int buflen
	  )
{
  size_t len;
  char *str;
  
  if ((str = catgets(catd, set_num, msg_num, NULL)) != NULL) {
    
    /* find the proper string length to copy */
    len = strlen(str); 
    if (len >= buflen)
      len = buflen - 1;
    
    (void) strncpy(buf, str, len);
    buf[len] = '\0';
    
    return buf;
  } else {
    return "\0";
  }
}


#ifndef	_LITTLE_ENDIAN
/*
 *      __catgetmsg_error_code - returns the error status from the last
 *              failed catgetmsg() call.
 *
 *              returns  < 0    Internal error code
 *                       > 0    System error code
 */
int
__catgetmsg_error_code(void)
{
        return(__catgets_error_code());
}
#endif				/* End NOT _LITTLE_ENDIAN */
