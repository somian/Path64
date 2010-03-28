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
* \file cg_swp_loop.cxx                                                *
* \brief Implementation of SWP_LOOP.                                   *
***********************************************************************/

/* Basic compiler include files */
#include "defs.h"
#include "tracing.h"

/* Software Pipelining (SWP) related include files */
#include "cg_swp_loop.h"



SWP_LOOP::SWP_LOOP (void) :
  loop_descr        (0),
  loop_type         (SWP_Do_Loop),
  status_code       (SWP_Successful),
  srcpos            (0),
  mii               (0),
  rec_mii           (0),
  res_mii           (0),
  ii                (0),
  sc                (0),
  lps               (0),
  les               (0),
  ps                (0),
  es                (0),
  kernel_unrollings (0),
  loop_unrollings   (0),
  time_ddg          (0.0),
  time_mii          (0.0),
  time_scheduler    (0.0),
  time_mve          (0.0),
  time_register     (0.0),
  time_code_gen     (0.0),
  time_total        (0.0),
  size_original     (0),
  size              (0)
{
  ISA_REGISTER_CLASS reg_class;
  FOR_ALL_ISA_REGISTER_CLASS (reg_class) { 
    register_count[reg_class] = 0;
  }

  return;
}


void 
SWP_LOOP::Print (FILE *f) const {
  fprintf (f, "%sSWP Summary:\n%s", DBar, DBar);
  Print_MII (f);
  Print_Time (f);
  fprintf (f, "%s\n", DBar);

  return;
}


void 
SWP_LOOP::Print_MII (FILE *f) const {
  fprintf (TFile, 
           "%sMII:\n%s"            \
           "Resource MII   = %d\n" \
           "Recurrence MII = %d\n" \
           "MII            = %d\n" \
           "%s\n",
           SBar,
           SBar,
           res_mii,
           rec_mii,
           mii,
           SBar);

  return;
}


void
SWP_LOOP::Print_Time (FILE *f) const {
  fprintf (f, "%sTime:\n%s", SBar, SBar);
  fprintf (f, "+DDG:                 %lf\n", time_ddg);
  fprintf (f, "+MII:                 %lf\n", time_mii);
  fprintf (f, "+Scheduler:           %lf\n", time_scheduler);
  fprintf (f, "+MVE:                 %lf\n", time_mve);
  fprintf (f, "+Register Allocation: %lf\n", time_register);
  fprintf (f, "+Code Generation:     %lf\n", time_code_gen);
  fprintf (f, "%s", SBar);
  fprintf (f, " Total:               %lf\n", time_total);
  fprintf (f, "%s\n", DBar);
 
  return; 
}


void 
SWP_LOOP::Verify (void) const {
  
  return;
}

