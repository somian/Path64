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


// -*-C++-*-
/* ====================================================================
 * ====================================================================
 *
 * Module: if_info.cxx
 * $Revision: 
 * $Date: 
 * $Author:
 * $Source:
 *
 * Revision history:
 *  20-Nov-96 - Original Version 
 *
 * Description:
 *
 * Basic if information required to analyze loops
 *
 * ====================================================================
 * ====================================================================
 */

#ifdef USE_PCH
#include "lno_pch.h"
#endif // USE_PCH
#pragma hdrstop

#include "if_info.h"

void IF_INFO::Print(FILE *fp)
{
  if (Contains_Do_Loops) fprintf(fp,"It has dos \n");
  if (Contains_Regions) fprintf(fp,"It has regions \n");
  if (Condition_On_Then) {
    fprintf(fp,"The access array (on the then) is "); 
    Condition->Print(fp,TRUE);
  } else {
    fprintf(fp,"The access array (on the else) is "); 
    Condition->Print(fp,TRUE);
  }
  fprintf(fp,"\n");
  if (Freq_True>=0.0) 
    fprintf(fp,"True branch taken probability is %f\n", Freq_True);
  if (Freq_False>=0.0)
    fprintf(fp,"False branch taken probablity is %f\n", Freq_False);
}
