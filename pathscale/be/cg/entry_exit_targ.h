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


// target-specific entry_exit code

// PFS = Previous Function State
extern void EETARG_Save_Pfs (TN *saved_pfs, OPS *ops);

extern void EETARG_Restore_Pfs (TN *saved_pfs, OPS *ops);

extern void EETARG_Init_Entry_Exit_Code (WN *pu_wn, BOOL need_frame_pointer);

// save and restore any extra callee-tns that need special processing
extern void EETARG_Save_Extra_Callee_Tns (OPS *ops);
extern void EETARG_Restore_Extra_Callee_Tns (OPS *ops);

// target-specific adjustments to entry ops
extern void EETARG_Fixup_Entry_Code (BB *bb);

//  Replace the call OP with a jump.
extern OP *EETARG_Build_Jump_Instead_Of_Call(OP *call_op);

#ifdef USE_HIGH_LEVEL_PROCEDURE_EXIT
extern OP* EETARG_High_Level_Procedure_Exit();
#else
inline OP* EETARG_High_Level_Procedure_Exit() { return NULL; }
#endif

#ifdef TARG_X8664
void EETARG_Generate_PIC_Entry_Code( BB* bb, OPS* ops );
void EETARG_Generate_PIC_Exit_Code( BB* bb, OPS* ops );
#endif
