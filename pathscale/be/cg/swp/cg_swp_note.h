/* 
 *  Copyright (C) 2008  PathScale, LLC. All Rights Reserved.
 *
   Path64 is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation version 3

   Path64 is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with Path64; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
 */


/*******************************************************************//**
* \file cg_swp_note.h                                                  *
* \brief                                                               *
***********************************************************************/

#ifndef cg_swp_note_INCLUDED
#define cg_swp_note_INCLUDED "cg_swp_note.h"

/* Basic compiler include files */
#include "defs.h"

/* Code Generator (CG) related include files */
#include "cgtarget.h"
#include "note.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_loop.h"



typedef struct {
  SRCPOS srcpos;
  INT prolog_num;
} NOTE_INFO_SWP_PROLOGUE;

typedef struct {
  SRCPOS srcpos;
  INT ii;
  INT mii;
  INT res_mii;
  INT rec_mii;
  INT max_ii;
  INT sc;
  INT kernel_unrollings;
  INT loop_unrollings;
  SWP_STATUS_CODE status_code;
  PRC_INFO *prc_info;
  INT register_count[ISA_REGISTER_CLASS_MAX+1];
  INT size_original;
  INT size;
} NOTE_INFO_SWP_LOOP;

typedef struct {
  SRCPOS srcpos;
  INT kernel_num;
} NOTE_INFO_SWP_KERNEL;

typedef struct {
  SRCPOS srcpos;
  INT epilog_num;
} NOTE_INFO_SWP_EPILOGUE;


#define NOTE_INFO_srcpos(info)         (info->srcpos)
#define NOTE_INFO_prolog_num(info)     (info->prolog_num)
#define NOTE_INFO_kernel_num(info)     (info->kernel_num)
#define NOTE_INFO_epilog_num(info)     (info->epilog_num)
#define NOTE_INFO_ii(info)             (info->ii)
#define NOTE_INFO_mii(info)            (info->mii)
#define NOTE_INFO_res_mii(info)        (info->res_mii)
#define NOTE_INFO_rec_mii(info)        (info->rec_mii)
#define NOTE_INFO_max_ii(info)         (info->max_ii)
#define NOTE_INFO_sc(info)             (info->sc)
#define NOTE_INFO_kernel_unroll(info)  (info->kernel_unrollings)
#define NOTE_INFO_loop_unroll(info)    (info->loop_unrollings)
#define NOTE_INFO_status_code(info)    (info->status_code)
#define NOTE_INFO_prc_info(info)       (info->prc_info)
#define NOTE_INFO_register_count(info) (info->register_count)
#define NOTE_INFO_size_original(info)  (info->size_original)
#define NOTE_INFO_size(info)           (info->size)


void 
SWP_Add_Prologue_Note (BB *bb, const SWP_LOOP& swp_loop, INT version);
void 
SWP_Add_Loop_Note (BB *bb, const SWP_LOOP& swp_loop);
void 
SWP_Add_Kernel_Note (BB *bb, const SWP_LOOP& swp_loop, INT version);
void 
SWP_Add_Epilogue_Note (BB *bb, const SWP_LOOP& swp_loop, INT version);


void 
SWP_Prologue_Note_Handler (NOTE_ACTION action, 
                           NOTE_INFO *info, 
                           FILE *f);
void 
SWP_Loop_Note_Handler (NOTE_ACTION action, NOTE_INFO *info, FILE *f);
void 
SWP_Kernel_Note_Handler (NOTE_ACTION action, 
                         NOTE_INFO *info, 
                         FILE *f);
void 
SWP_Epilogue_Note_Handler (NOTE_ACTION action, 
                           NOTE_INFO *info, 
                           FILE *f);

#endif /* cg_swp_note_INCLUDED */

