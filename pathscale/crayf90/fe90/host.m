
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

/* USMID:  "\n@(#)5.0_pl/macros/host.m	5.1	04/29/99 21:22:31\n" */

/* This module is for host specific information.  */

/*   CHAR_BIT  is defined in limits.h and is the number of bits in a char.    */
/*   _WORD_BIT is defined in limits.h and is the number of bits per word.     */


# ifdef _HOST64

# ifndef _HOST_OS_UNICOS 
#    define _WORD_BIT				64
# endif

#    define HOST_BITS_PER_WORD          	_WORD_BIT    /* Bits per word */
#    define HOST_BYTES_PER_WORD			8	/* Bytes per word     */

#    define HOST_BYTES_TO_WORDS(BYTE_SIZE)	(((BYTE_SIZE)+7) >> 3)

#    define MAX_MSG_SIZE			200
#ifdef KEY /* Bug 3635 */
/* This must increase to track our increase in MAX_ID_LEN */
#    define NUM_ID_WDS				8	/* words in identifier*/
#else
#    define NUM_ID_WDS				4	/* words in identifier*/
#endif /* KEY Bug 3635 */

# elif _HOST32

#    define HOST_BITS_PER_WORD			32	/* Bits per word   */
#    define HOST_BYTES_PER_WORD			4	/* Bytes per word  */

#    define HOST_BYTES_TO_WORDS(BYTE_SIZE)	(((BYTE_SIZE)+3) >> 2)

#    define MAX_MSG_SIZE			400
#ifdef KEY /* Bug 3635 */
#    define NUM_ID_WDS				16      /* words in identifier*/
#else
#    define NUM_ID_WDS				8       /* words in identifier*/
#endif /* KEY Bug 3635 */
# endif

/* From Cray man page.                                                       */
/*   The _toupper and _tolower functions accomplish the same thing as        */
/*   toupper and tolower, but have a restricted domain and are faster.  The  */
/*   _toupper function requires a lowercase letter as its argument; its      */
/*   result is the corresponding uppercase letter.  The _tolower function    */
/*   requires an uppercase letter as its argument; its result is the         */
/*   corresponding lowercase letter.  Arguments outside the domain cause     */
/*   undefined results.                                                      */

# if defined(_HOST_OS_UNICOS) || defined(_HOST_OS_MAX)

#    define TOUPPER                     _toupper        /* To use macro       */

# else

#    define TOUPPER                     toupper
# endif

# ifndef _MAXVL
# define _MAXVL 64
# endif

/* This is standard C used by fseek.  gcc does not support it. */

# ifndef SEEK_CUR
# define SEEK_CUR 1
# endif

# if defined(_HOST_OS_UNICOS) || defined(_HOST_OS_MAX)
# define Pragma(S)	_Pragma(S)
# else
# define Pragma(S)
# endif

# if defined(_HOST_OS_SOLARIS) 
#   define      MAXHOSTNAMELEN  256
# endif


