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


#pragma ident "@(#) libu/util/cliblevel.c	92.1	07/07/99 13:18:33"

#include <cray/clibinc_config.h>

/*
 *	The CrayLibs release version in the format 'Mmru' where:
 *
 *		M  is the major release
 *		m  is the minor release
 *		r  is the revision level	(inconsistently implemented)
 *		u  is the update level		(inconsistently implemented)
 * 	
 *	For example:
 *
 *	1111 represents CrayLibs 1.1.1.1.
 *	2000 represents CrayLibs 2.0
 */

long	_CRAYLIBS_RELEASE_LEVEL		= _CRAYLIBS_RELEASE;
