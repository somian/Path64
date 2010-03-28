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



/* USMID:  "\n@(#)5.0_pl/headers/s_cnstrct.h	5.3	06/11/99 08:54:22\n" */

long64			words_in_constructor;
long64			bits_in_constructor;
long_type		result_value[MAX_WORDS_FOR_NUMERIC];
char			*char_result_buffer = NULL;
long64			char_result_len = 0;
long64			char_result_buffer_len = 0;
long64			char_result_offset = 0;
long64			the_cn_bit_offset;
int			the_cn_idx;
long64			bcast_cn_bit_offset;
boolean			in_array_constructor = FALSE;
boolean			no_result_value = FALSE;
boolean			unequal_char_lens;
boolean			do_constructor_init = TRUE;
boolean			single_value_array;
opnd_type		single_value_opnd;

struct	save_env_entry	{
			long64               	words_in_constructor;
			long64               	bits_in_constructor;
			long64               	char_result_len;
			long64                  char_result_offset;
			long64 	              	the_cn_bit_offset;
			int                     the_cn_idx;
			long64               	bcast_cn_bit_offset;
			boolean                 no_result_value;
			boolean                 unequal_char_lens;
			boolean                 do_constructor_init;
			boolean			check_type_conversion;
			int			target_type_idx;
			int   	                target_char_len_idx;
			opnd_type		init_target_opnd;
			boolean			single_value_array;
			opnd_type		single_value_opnd;
			};

typedef struct	save_env_entry save_env_type;


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
