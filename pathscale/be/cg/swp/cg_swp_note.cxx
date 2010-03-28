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
* \file cg_swp_note.cxx                                                *
* \brief                                                               *
***********************************************************************/

/* Basic compiler include files */
#include "defs.h"
#include "mempool.h"

/* Code Generator (CG) related include files */
#include "config_asm.h"

/* Software Pipelinig (SWP) related include files */
#include "cg_swp.h"
#include "cg_swp_options.h"
#include "cg_swp_loop.h"
#include "cg_swp_note.h"



static const char *swps_prefix = ASM_CMNT_LINE"<swps> ";
static const char *swpf_prefix = ASM_CMNT_LINE"<swpf> ";
static const char *swp_prefix  = ASM_CMNT_LINE"<swp> ";
static const char *swp_suffix  = "\n";


void 
SWP_Add_Prologue_Note (BB *bb, const SWP_LOOP& swp_loop, INT version) {
  NOTE_INFO_SWP_PROLOGUE *info = TYPE_PU_ALLOC (NOTE_INFO_SWP_PROLOGUE);
  NOTE_INFO_srcpos (info)      = swp_loop.srcpos;
  NOTE_INFO_prolog_num (info)  = version;

  NOTE_Add_To_BB (bb, SWP_Prologue_Note_Handler, (NOTE_INFO *) info);

  return;
}


void 
SWP_Add_Loop_Note (BB *bb, const SWP_LOOP& swp_loop) {
  NOTE_INFO_SWP_LOOP *info = TYPE_PU_ALLOC (NOTE_INFO_SWP_LOOP);
  PRC_INFO *prc_info       = TYPE_PU_ALLOC (PRC_INFO);
  
  CGTARG_Compute_PRC_INFO (bb, prc_info);

  NOTE_INFO_srcpos (info)        = swp_loop.srcpos;
  NOTE_INFO_ii (info)            = swp_loop.ii;
  NOTE_INFO_mii (info)           = swp_loop.mii;
  NOTE_INFO_res_mii (info)       = swp_loop.res_mii;
  NOTE_INFO_rec_mii (info)       = swp_loop.rec_mii;
  NOTE_INFO_max_ii (info)        = SWP_Options.Max_II;
  NOTE_INFO_sc (info)            = swp_loop.sc;
  NOTE_INFO_kernel_unroll (info) = swp_loop.kernel_unrollings;
  NOTE_INFO_loop_unroll (info)   = swp_loop.loop_unrollings;
  NOTE_INFO_status_code (info)   = swp_loop.status_code;
  NOTE_INFO_prc_info (info)      = prc_info;
  NOTE_INFO_size_original (info) = swp_loop.size_original;
  NOTE_INFO_size (info)          = swp_loop.size;
  
  ISA_REGISTER_CLASS reg_class;
  FOR_ALL_ISA_REGISTER_CLASS (reg_class) {
    NOTE_INFO_register_count (info)[reg_class] = 
      swp_loop.register_count[reg_class];
  }


  NOTE_Add_To_BB (bb, SWP_Loop_Note_Handler, (NOTE_INFO *) info);

  return;
}


void 
SWP_Add_Kernel_Note (BB *bb, const SWP_LOOP& swp_loop, INT version) {
  NOTE_INFO_SWP_KERNEL *info   = TYPE_PU_ALLOC (NOTE_INFO_SWP_KERNEL);
  NOTE_INFO_srcpos (info)      = swp_loop.srcpos;
  NOTE_INFO_kernel_num (info)  = version;

  NOTE_Add_To_BB (bb, SWP_Kernel_Note_Handler, (NOTE_INFO *) info);

  return;
}


void 
SWP_Add_Epilogue_Note (BB *bb, const SWP_LOOP& swp_loop, INT version) {
  NOTE_INFO_SWP_EPILOGUE *info = TYPE_PU_ALLOC (NOTE_INFO_SWP_EPILOGUE);
  NOTE_INFO_srcpos (info)      = swp_loop.srcpos;
  NOTE_INFO_epilog_num (info)  = version;

  NOTE_Add_To_BB (bb, SWP_Epilogue_Note_Handler, (NOTE_INFO *) info);

  return;
}


void
SWP_Prologue_Note_Handler (NOTE_ACTION action, 
                           NOTE_INFO *info, 
                           FILE *f) 
{
  NOTE_INFO_SWP_PROLOGUE *infop = (NOTE_INFO_SWP_PROLOGUE *) info;
  SRCPOS srcpos  = NOTE_INFO_srcpos (infop);
  INT line_num   = SRCPOS_linenum (srcpos);
  INT prolog_num = NOTE_INFO_prolog_num (infop);

  switch (action) {
    case NOTE_PRINT_TO_FILE:
      fprintf (f, "%s%s", swp_prefix, swp_suffix);
      fprintf (f, "%sPrologue %d for software pipelined loop line %d%s",
               swp_prefix, prolog_num, line_num, swp_suffix);
      fprintf (f, "%s%s", swp_prefix, swp_suffix);
      break;

    case NOTE_PRINT_TO_ANL_FILE:
      break;

    case NOTE_PRINT_HANDLER_NAME_TO_FILE:
      fprintf (f, "SWP_Prologue_Note_Handler");
      break;

    default:
      Is_True (FALSE, ("Unknown note handler action."));
  }

  return;
}


void 
SWP_Loop_Note_Handler (NOTE_ACTION action, NOTE_INFO *info, FILE *f) {
  NOTE_INFO_SWP_LOOP *infol     = (NOTE_INFO_SWP_LOOP *) info;
  SRCPOS srcpos                 = NOTE_INFO_srcpos (infol);
  INT ii                        = NOTE_INFO_ii (infol);
  INT res_mii                   = NOTE_INFO_res_mii (infol);
  INT rec_mii                   = NOTE_INFO_rec_mii (infol);
  INT mii                       = NOTE_INFO_mii (infol);
  INT max_ii                    = NOTE_INFO_max_ii (infol);
  INT sc                        = NOTE_INFO_sc (infol);
  INT kernel_unroll             = NOTE_INFO_kernel_unroll (infol);
  INT loop_unroll               = NOTE_INFO_loop_unroll (infol);
  SWP_STATUS_CODE status_code   = NOTE_INFO_status_code (infol);
  PRC_INFO *prc_info            = NOTE_INFO_prc_info (infol);
  INT size_original             = NOTE_INFO_size_original (infol);
  INT size                      = NOTE_INFO_size (infol);
  INT line_num                  = SRCPOS_linenum (srcpos);
  ISA_REGISTER_CLASS reg_class;

  switch (action) {
    case NOTE_PRINT_TO_FILE:
    {
      const char *prefix;
      const char *suffix = swp_suffix;
      if (status_code == SWP_Successful) {
        prefix = swps_prefix;
        
        fprintf (f, "%s%s", prefix, suffix);
        fprintf (f, "%sSoftware Pipelined loop line %d%s", 
                 prefix, line_num, suffix);
        fprintf (f, "%s%s", prefix, suffix);
        fprintf (f, "%s%5d unrollings before software pipelining%s", 
                 prefix, loop_unroll, suffix);
        fprintf (f, "%s%5d cycles per %d iterations (%.2f cycle/iteration)%s", 
                 prefix,
                 ii, 
                 loop_unroll ? loop_unroll : 1, 
                 (float)ii / (loop_unroll ? loop_unroll : 1), 
                 suffix);
        fprintf (f, "%s%s", prefix, suffix);
        fprintf (f, "%s%5d min. cycle due to recurrences%s", 
                 prefix, rec_mii, suffix);
        fprintf (f, "%s%5d min. cycle due to resources%s", 
                 prefix, res_mii, suffix);
        fprintf (f, "%s%5d min. cycle due to recurrences and resources%s", 
                 prefix, mii, suffix);
        fprintf (f, "%s%5d cycle II%s", 
                 prefix, ii, suffix);
        fprintf (f, "%s%5d stages per kernel%s", 
                 prefix, sc, suffix);
        fprintf (f, "%s%5d MVE kernel%s", 
                 prefix, kernel_unroll, suffix);
        fprintf (f, "%s%5d OPs before software pipelining%s",
                 prefix, size_original, suffix);
        fprintf (f, "%s%5d OPs after software pipelining%s",
                 prefix, size, suffix);
        fprintf (f, "%s%3.2fx code size increase%s",
                 prefix, (float)size/size_original, suffix);
        fprintf (f, "%s%s", prefix, suffix);
         CGTARG_Print_PRC_INFO (f, prc_info, ii, prefix, suffix);
        fprintf (f, "%s%s", prefix, suffix);
        FOR_ALL_ISA_REGISTER_CLASS (reg_class) {
          if ( NOTE_INFO_register_count (infol)[reg_class] > 0 ) {
            fprintf (f, 
                     "%s%5d %s register used%s",
                     prefix,
                     NOTE_INFO_register_count (infol)[reg_class],
                     REGISTER_CLASS_name (reg_class),
                     suffix);
          }
        }
        fprintf (f, "%s%s", prefix, suffix);
        fprintf (f, "\n");
      }
      else {
        prefix = swpf_prefix;

        fprintf (f, "%s%s", prefix, suffix);
        fprintf (f, "%sSoftware Pipelining failed: ", prefix);

        switch (status_code) {
          case SWP_Small_Trip_Count:
            fprintf (f, "trip count too small");
            break;

          case SWP_Too_Large_MII:
            fprintf (f, 
                     "too large MII. Resource MII (%d) exceeds " \
                     "MAX_II (%d)",
                     res_mii,
                     max_ii);
            break;
          
          case SWP_Scheduling_Failed:
            fprintf (f,
                     "couldn't find a modulo schedule. Tried for IIs" \
                     "(%d to %d)",
                     mii,
                     ii);
            break;
          
          case SWP_Register_Failed:
            fprintf (f, "not enough registers");
            break;

          default:
            fprintf (f, "unknown");
            break;
        }

        fprintf (f, "%s%s%s", suffix, prefix, suffix);
        fprintf (f, "\n");
      }
    }
      break;

    case NOTE_PRINT_TO_ANL_FILE:
      break;

    case NOTE_PRINT_HANDLER_NAME_TO_FILE:
      fprintf (f, "SWP_Loop_Note_Handler");
      break;

    default:
      Is_True (FALSE, ("Unknown note handler action."));
  }

  return;
}


void 
SWP_Kernel_Note_Handler (NOTE_ACTION action, NOTE_INFO *info, FILE *f) {
  NOTE_INFO_SWP_KERNEL *infok = (NOTE_INFO_SWP_KERNEL *) info;
  SRCPOS srcpos  = NOTE_INFO_srcpos (infok);
  INT line_num   = SRCPOS_linenum (srcpos);
  INT kernel_num = NOTE_INFO_kernel_num (infok);

  switch (action) {
    case NOTE_PRINT_TO_FILE:
      fprintf (f, "%s%s", swp_prefix, swp_suffix);
      fprintf (f, 
               "%sMVE kernel %d for software pipelined loop line %d%s",
               swp_prefix, kernel_num, line_num, swp_suffix);
      fprintf (f, "%s%s", swp_prefix, swp_suffix);
      break;

    case NOTE_PRINT_TO_ANL_FILE:
      break;

    case NOTE_PRINT_HANDLER_NAME_TO_FILE:
      fprintf (f, "SWP_Kernel_Note_Handler");
      break;

    default:
      Is_True (FALSE, ("Unknown note handler action."));
  }

  return;
}


void 
SWP_Epilogue_Note_Handler (NOTE_ACTION action, NOTE_INFO *info, FILE *f)
{
  NOTE_INFO_SWP_EPILOGUE *infoe = (NOTE_INFO_SWP_EPILOGUE *) info;
  SRCPOS srcpos  = NOTE_INFO_srcpos (infoe);
  INT line_num   = SRCPOS_linenum (srcpos);
  INT epilog_num = NOTE_INFO_epilog_num (infoe);

  switch (action) {
    case NOTE_PRINT_TO_FILE:
      fprintf (f, "%s%s", swp_prefix, swp_suffix);
      fprintf (f, "%sEpilogue %d for software pipelined loop line %d%s",
               swp_prefix, epilog_num, line_num, swp_suffix);
      fprintf (f, "%s%s", swp_prefix, swp_suffix);
      break;

    case NOTE_PRINT_TO_ANL_FILE:
      break;

    case NOTE_PRINT_HANDLER_NAME_TO_FILE:
      fprintf (f, "SWP_Epilogue_Note_Handler");
      break;

    default:
      Is_True (FALSE, ("Unknown note handler action."));
  }

  return;
}

