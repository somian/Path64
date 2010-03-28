#!/bin/sh
#
#
#  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
#
#  Path64 is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3, or (at your option)
#  any later version.
#
#  Path64 is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
#  License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Path64; see the file COPYING.  If not, write to the Free
#  Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
#  02110-1301, USA.
#
#  Special thanks goes to SGI for their continued support to open source
#
#


### ====================================================================
### ====================================================================
### Module: gen_cg_loop_scc_prop.sh
### $Revision$
### $Date$
### $Author$
### $Source$
### Revision history:
###   11-Aug-92
###
### Usage:      gen_cg_loop_scc_prop MTP_BIN
###
###     Generate the cg_loop_scc_prop.[ch] module.  The argument is the MTP_BIN
###     directory.  We do this in a file so the make rule can depend on and it
###     can be rebuilt when the procedure changes
###
### ====================================================================
### ====================================================================



sh $1/gen_x_prop.sh    'struct cg_loop_scc *'                     \
                            'CG_LOOP_SCC'                              \
                            'CG_LOOP_SCC_id(x)'                        \
                            'defs.h'                                   \
                            'mempool.h'                                 \
                            'cgir.h'                                   \
                            'cg_loop_scc_prop.h'                       \
                            'cg_loop_scc.h'
