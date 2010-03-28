/*
 *  Copyright (C) 2008 PathScale, LLC.  All Rights Reserved.
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


/* ====================================================================
 * ====================================================================
 *
 *  CG driver declarations which include target dependencies.
 *
 *  THIS FILE IS ONLY TO BE INCLUDE BY ../cgdriver.cxx!!!!
 *
 * ====================================================================
 * ====================================================================
 */

inline BOOL Enable_Prefetch_Ahead_For_Target(void)
{
  return TRUE;
}

inline BOOL Target_Has_Prefetch(void)
{
  return TRUE;
}

inline BOOL Enable_Prefetch_For_Target(void)
{
  return TRUE;
}

inline BOOL Enable_Idiv_In_FPU_For_Target(void)
{
  return FALSE;
}

inline BOOL Target_Has_Cmoves(void)
{
  return FALSE;
}

inline BOOL Enable_Spec_Idiv_For_Target(void)
{
  return FALSE;
}

inline BOOL Enable_Fold_Expanded_daddiu_For_Target(void)
{
  return FALSE;
}

inline BOOL Enable_LOH_For_Target(void)
{
  return FALSE;
}

inline BOOL Enable_Fill_Delay_Slots_For_Target(void)
{
  if (Is_Target_Mips5kf())
    return TRUE;
  else if (Is_Target_twc9a())
    return TRUE;
  else
    // SGI assembler does not accept something like 
    //     bne $2,$0,.Lt_0_1+4
    // It expects something like 
    //     bne $2,$0,.+76
    // Because, we can not generate absolute addresses (Inline assembly),
    // we do not enable Fill_Delay_Slot optimization for R10k.
    return FALSE;
}

#ifdef PAIRS
inline BOOL Enable_SWP_Memory_Pairs(void)
{
  return FALSE;
}
#endif /* PAIRS */

inline BOOL Enable_SWP_Optimistic_II_Search(void)
{
  return TRUE;
}
