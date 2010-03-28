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


/* USMID:  "\n@(#)5.0_pl/macros/cmd_line.m	5.1	04/29/99 21:22:31\n" */

/*****************\
|* MISCELLANEOUS *|
\*****************/

# define FALSE_OPTION		(0)
# define TRUE_OPTION	       (-1)


/********************\
|* SIZES AND LIMITS *|
\********************/

# define MAX_TRUNCATION_BITS	47			/* max value on -t opt*/


/******************************\
|* OBJECT REPLACEMENT STRINGS *|
\******************************/



/***********************************\
|* CONDITIONAL REPLACEMENT STRINGS *|
\***********************************/



/***********************************************\
|* STATEMENT/FUNCTION-LIKE REPLACEMENT STRINGS *|
\***********************************************/

# define MAKE_DEFAULT_NAME(NAME, STR, SUFFIX)				       \
	 { char *_nmp, *_stp;						       \
		_nmp = NAME;						       \
		_stp = strrchr (STR, SLASH);				       \
		_stp = (_stp == NULL) ? STR : _stp+1;			       \
		while (*_nmp++ = *_stp++) ;				       \
		_stp = strrchr (NAME, DOT);				       \
		if (_stp != NULL &&                                            \
                    (EQUAL_STRS(_stp, ".f") || EQUAL_STRS(_stp, ".F"))) {      \
		   strcpy (++_stp, SUFFIX);				       \
		}							       \
		else if (_stp != NULL &&                                       \
                   (EQUAL_STRS(_stp, ".f90") || EQUAL_STRS(_stp, ".F90"))) {   \
		   strcpy (++_stp, SUFFIX);				       \
		}							       \
		else if (_stp != NULL  &&  EQUAL_STRS(_stp, ".i")) {           \
		   strcpy (++_stp, SUFFIX);				       \
		}							       \
		else {							       \
		   _nmp--;						       \
		   *_nmp++ = DOT;					       \
		   strcpy (_nmp++, SUFFIX);				       \
		}							       \
	}
