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


#ifndef __IPC_MAIN_H__
#define __IPC_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

extern void ipa_driver (int, char **);
extern void ipa_dot_so_init (void);

#ifdef _LD_IPA_INTERFACE

#pragma weak ipa_driver
#pragma weak ipa_dot_so_init

#endif

#ifdef __cplusplus
}
#endif

#endif /* __IPC_MAIN_H__ */
