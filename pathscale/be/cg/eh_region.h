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


#ifndef	eh_region_INCLUDED
#define	eh_region_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * This module encapsulates the generation of exception region tables.
 */

struct EH_RANGE;

extern void EH_Generate_Range_List(WN *);
extern void EH_Set_Start_Label(struct EH_RANGE *);
extern void EH_Set_End_Label(struct EH_RANGE *);
#ifdef __cplusplus
}
#endif /* __cplusplus */



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
extern void EH_Set_Has_Call(struct EH_RANGE *);
extern void EH_Prune_Range_List(void);
extern void EH_Write_Range_Table(WN *);

extern ST * EH_Get_PU_Range_ST (void);
extern void EH_Print_Range_List(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* eh_region_INCLUDED */
