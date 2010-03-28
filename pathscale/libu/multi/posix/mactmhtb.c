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


#pragma ident "@(#) libu/multi/posix/mactmhtb.c	92.1	06/25/99 14:35:10"

#include "macdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>

#if defined(ASSEMBLY)
#   undef ASSEMBLY
#endif
#include "mactdefs.h"


/*========================================
 * BUFDUMP_
 *
 * Dump unformatted contents of multitasking history trace buffer.
 *
 * Note: This is only present to allow ported programs that call it
 *       to be compiled and linked without change.  It has no effect
 *       in the SPARC f90 v1.0 implementation.
 */
void
bufdump_()
{

    return;
}


/*========================================
 * BUFPRINT_
 *
 * Dump formatted contents of multitasking history trace buffer.
 *
 * Note: This is only present to allow ported programs that call it
 *       to be compiled and linked without change.  It has no effect
 *       in the SPARC f90 v1.0 implementation.
 */
void
bufprint_()
{

    return;
}


/*========================================
 * BUFUSER_
 *
 * Add user entries to the multitasking history trace buffer.
 *
 * Note: This is only present to allow ported programs that call it
 *       to be compiled and linked without change.  It has no effect
 *       in the SPARC f90 v1.0 implementation.
 */
void
bufuser_()
{

    return;
}


/*========================================
 * BUFTUNE_
 *
 * Tune multitasking history trace buffer parameters.
 *
 * Note: This is only present to allow ported programs that call it
 *       to be compiled and linked without change.  It has no effect
 *       in the SPARC f90 v1.0 implementation.
 */
void
buftune_()
{

    return;
}
