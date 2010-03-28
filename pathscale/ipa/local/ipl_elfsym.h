/*
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 */

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


/* -*-Mode: c++;-*- (Tell emacs to use c++ mode) */
/* ====================================================================
 * ====================================================================
 *
 * Module: ipl_elfsym.h
 *
 * Description:
 *	convert WHIRL global symbol table to Elf
 *
 * ====================================================================
 * ====================================================================
 */

#ifndef ipl_elfsym_INCLUDED
#define ipl_elfsym_INCLUDED

#ifndef __IR_BWRITE_H__
#include "ir_bwrite.h"
#endif // __IR_BWRITE_H__

extern void
IPL_Write_Elf_Symtab (Output_File *fl);

#endif /* ipl_elfsym_INCLUDED */
