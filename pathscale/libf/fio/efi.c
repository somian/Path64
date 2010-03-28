/*

  Copyright (C) 2000, 2001, Silicon Graphics, Inc.  All Rights Reserved.

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



#pragma ident "@(#) libf/fio/efi.c	92.1	06/18/99 10:21:14"

#include "fio.h"
#include <errno.h>
#include <liberrno.h>
#include <fortran.h>
#ifdef _CRAYMPP
#include <stdarg.h>
#endif

#ifdef	_CRAY2
#pragma	_CRI duplicate $EFI as @EFI
#endif

/* _numargs returns the number of words passed. We need to know */
/* the number of arguments. These macros convert words to arguments. */
#define ARGS_3  (1 + 2*sizeof(_fcd)/sizeof(long))
#define ARGS_4  (2 + 2*sizeof(_fcd)/sizeof(long))
#define ARGS_7  (5 + 2*sizeof(_fcd)/sizeof(long))
#define ARGS_9  (7 + 2*sizeof(_fcd)/sizeof(long))

/*
 *	$EFI - encode initialization
 *
 *	CALL	$EFI,(len, format, fwa, _arg4, _arg5, _arg6, pform, inumcfe)
 *
 *		len	Address of length (in characters)
 *		format	Address of format (Fortran character descriptor or
 *			hollerith)
 *		fwa	Address of output character string
 *		_arg4	Unused (old pform argument)
 *		_arg5	Unused
 *		_arg6	Unused
 *		pform	Address of address of parsed format (NULL if no
 *			compiler-supplied word; points to NULL if not yet
 *			parsed).  This parameter is absent in CFT77 5.0.2
 *			and previous on CX/CEA systems.
 *		inumelt	Address of number of interal array elements
 *			(internal I/O only)
 *		inumcfe	Address of number of array elements in a character
 *			format (to be added in a later compiler release).
 *
 *		A zero argument indicates an unspecified option
 *
 *       $EFI calls:
 *
 *		$WFI
 */

#ifdef _CRAYMPP
int
$EFI(
long		*len,		/* Address of length (in characters)	*/
...
)
#else
int
$EFI(
long		*len,		/* Address of length (in characters)	*/
_fcd		format,		/* Address of format (FCD or hollerith)	*/
_fcd		fwa,		/* Address of output character string	*/
fmt_type	**_arg4,	/* Unused (old pform arugment)		*/
long		*_arg5,		/* Unused				*/
long		*_arg6,		/* Unused				*/
fmt_type	**pform,	/* Address of address of parsed format	*/
long		*inumelt,	/* Address of int. array element count	*/
long		*inumcfe	/* Address of number of format elements */
)
#endif
{
	long	mone = -1L;
	_fcd	fch;
	int	nargs;

#ifdef _CRAYMPP
	va_list args;
	_fcd	format;		/* Address of format (FCD or hollerith)	*/
	_fcd	fwa;		/* Address of output character string	*/
	fmt_type **_arg4;	/* Unused (old pform arugment)		*/
	long	*_arg5;		/* Unused				*/
	long	*_arg6;		/* Unused				*/
	fmt_type **pform;	/* Address of address of parsed format	*/
	long	*inumelt;	/* Address of int. array element count	*/
	long	*inumcfe;	/* Address of number of format elements */

	va_start(args, len);
	format = va_arg(args, _fcd);
	fwa = va_arg(args, _fcd);
#endif

	if (*len <= 0)		/* If length is zero or negative */
		_ferr(NULL, FEENCDRL);	/* Invalid ENCODE record length */

	/* Round byte count up to a word multiple */

	fch	= _cptofcd(_fcdtocp(fwa), (*len + 7) & ~07);

	nargs = _numargs();
#ifdef _CRAYMPP
        if (nargs >= ARGS_7) {
                _arg4 = va_arg(args, fmt_type **);
                _arg5 = va_arg(args, long *);
                _arg6 = va_arg(args, long *);
                pform = va_arg(args, fmt_type **);
                if (nargs >= ARGS_9) {
                        inumelt = va_arg(args, long *);
                        inumcfe = va_arg(args, long *);
                }
        }
        va_end(args);
#endif


	/* Map ENCODE into an internal WRITE */

	switch ( nargs ) {

#ifdef	_CRAY2
		case ARGS_4:
			return ( $WFI(fch, format, NULL, NULL, NULL, NULL,
					_arg4, &mone) );
#else
		case ARGS_3:
			return ( $WFI(fch, format, NULL, NULL, NULL, NULL,
					NULL, &mone) );
#endif

		case ARGS_7:
			return ( $WFI(fch, format, NULL, NULL, NULL, NULL,
					pform, &mone) );

		case ARGS_9:
			return ( $WFI(fch, format, NULL, NULL, NULL, NULL,
					pform, inumelt, inumcfe) );

		default:
			_ferr(NULL, FEINTUNK);

	}	/* switch */

	return(IO_ERR);		/* Should never get here */
}
