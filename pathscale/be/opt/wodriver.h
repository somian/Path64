// ====================================================================
// ====================================================================
//
// Module: wodriver.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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
//
// ====================================================================
//
// Description:
//  exported functions from wopt.so
//
// ====================================================================
// ====================================================================


#ifndef wodriver_INCLUDED
#define wodriver_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

extern void wopt_main (INT argc, char **argv, INT, char **);

extern void Wopt_Init (void);

extern void Wopt_Fini (void);

extern WN *Perform_Preopt_Optimization (WN *, WN *);

extern WN *Perform_Global_Optimization (WN *, WN *, struct ALIAS_MANAGER *);

#ifdef __cplusplus
}
#endif
#endif /* wodriver_INCLUDED */
