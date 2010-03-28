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

//  Grants of local registers
/////////////////////////////////////
//  
//  Description:
//
//      GRA allocates registers for locals as well as globals.  But it doesn't
//      actually assign locals to registers, this is the job of local register
//      allocation.  Instead it <grants> registers in each block which the
//      local register allation may use.  Local register allocation may
//      additionally use any other register in the block, but at the cost of
//      spilling and restoring the register at the top and bottom of the block.
//
//      This module contains a really very simple mechanism to maintain the
//      information about grants and communicate it to the local register
//      allocator.
//
//  Exported functions:
//
//      void GRA_GRANT_Initialize(void)
//          Call to allocate the grant mapping.
//
//      void GRA_GRANT_Finalize(void)
//          Call to delete the grant mapping.
//
//      void GRA_GRANT_Local_Register( GRA_BB* gbb, ISA_REGISTER_CLASS rc,
//                                                  REGISTER           reg )
//          Call to grant the given <reg> and <rc> in <bb>.
//
//      REGISTER_SET GRA_GRANT_Get_Local_Registers( BB*                bb,
//                                                  ISA_REGISTER_CLASS rc )
//          Return the registers granted for locals in the given <bb> and <rc>.
//
//      void GRA_GRANT_Transfer( BB* from_bb, BB* to_bb )
//
//          Transfer any local register grants from <from_bb> to <to_bb>.
//
//      void GRA_GRANT_Unused_Caller_Saved( void )
//
//          Grant any unused caller saved registers in each block to the
//	    local register allocator, regardless of its request.  This will
//	    be of some help to GCM.
//
/////////////////////////////////////


//  $Revision$
//  $Date$
//  $Author$
//  $Source$


#ifndef GRA_GRANT_INCLUDED
#define GRA_GRANT_INCLUDED

#ifndef GRA_GRANT_RCS_ID
#define GRA_GRANT_RCS_ID
#ifdef _KEEP_RCS_ID
static char *gra_grant_rcs_id = "$Source$ $Revision$";
#endif
#endif

#include "bb.h"
#include "register.h"
#include "gra_bb.h"

extern void
GRA_GRANT_Initialize(void);
extern void 
GRA_GRANT_Finalize(void);
extern void 
GRA_GRANT_Local_Register( GRA_BB* gbb, ISA_REGISTER_CLASS rc, REGISTER reg );
extern REGISTER_SET 
GRA_GRANT_Get_Local_Registers( BB* bb, ISA_REGISTER_CLASS rc );
extern void
GRA_GRANT_Transfer( BB* from_bb, BB* to_bb );

extern void
GRA_GRANT_Unused_Caller_Saved(void);
#endif
