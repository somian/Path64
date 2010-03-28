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



/* USMID:  "\n@(#)5.0_pl/headers/s_rcnstrct.h	5.1	04/29/99 21:22:31\n" */

extern exp_tbl_type    bin_add_tbl[Num_Linear_Types][Num_Linear_Types];
extern exp_tbl_type    bin_sub_tbl[Num_Linear_Types][Num_Linear_Types];
extern exp_tbl_type    mult_div_tbl[Num_Linear_Types][Num_Linear_Types];
extern exp_tbl_type    power_tbl[Num_Linear_Types][Num_Linear_Types];
extern exp_tbl_type    eq_ne_tbl[Num_Linear_Types][Num_Linear_Types];
extern exp_tbl_type    gt_lt_tbl[Num_Linear_Types][Num_Linear_Types];
extern exp_tbl_type    and_or_tbl[Num_Linear_Types][Num_Linear_Types];
extern exp_tbl_type    asg_tbl[Num_Linear_Types][Num_Linear_Types];
extern exp_tbl_type    un_plus_tbl[Num_Linear_Types];
extern exp_tbl_type    not_tbl[Num_Linear_Types];

extern void    (*uminus_folders[Num_Linear_Types]) ();
extern void    (*plus_folders[Num_Linear_Types][Num_Linear_Types]) ();
extern void    (*mult_folders[Num_Linear_Types][Num_Linear_Types]) ();
extern void    (*minus_folders[Num_Linear_Types][Num_Linear_Types]) ();
extern void    (*div_folders[Num_Linear_Types][Num_Linear_Types]) ();
extern void    (*power_folders[Num_Linear_Types][Num_Linear_Types]) ();
extern void    (*eq_folders[Num_Linear_Types][Num_Linear_Types]) ();
extern void    (*lx_folders[Num_Linear_Types][Num_Linear_Types]) ();
