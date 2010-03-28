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

#ifndef cxx_ip_call_INCLUDED
#define cxx_ip_call_INCLUDED

#ifndef clone_INCLUDED
#include "clone.h"	    // for IPO_ADDR_HASH
#endif // clone_INCLUDED

/*---------------------------------------------------------------------*/
/* caller specific state information                                   */
/*---------------------------------------------------------------------*/
class CALLEE_STATE {

private:
    
    IPO_ADDR_HASH *addr_map;	    // maps for originals/copies of TY and ST

    INT _count;

public:

    CALLEE_STATE (IPO_ADDR_HASH *hash, INT c) {
	addr_map = hash;
	_count = c;
    };

    INT Get_count() { return _count;};
    IPO_ADDR_HASH *Get_addr_map () { return addr_map; };
    void Set_count(INT c) { _count = c;};
    void Set_addr_map (IPO_ADDR_HASH *hash) { addr_map = hash; };
}; // CALLEE_STATE

#endif // cxx_ip_call_INCLUDED
