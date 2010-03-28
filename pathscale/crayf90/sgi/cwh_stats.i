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

/* reprentation of a counter group - */

typedef struct {
	int  *  counter ;     /* the array of counters - cumulative across PUs */
	int  *  saved   ;     /* counter values from previous PU               */
const	int  *  flags   ;     /* flags associated with counter                 */
	char ** string  ;     /* array of strings associated with counters     */
	char ** misc_str;     /* array(3) of header string & 2 summation strs  */
	int     num     ;     /* number of counters.                           */
	int     svd_sum ;     /* previous SUM                                  */
} CTR ;


#define CTR_count(c1,c2)   ((c1)->counter[c2])
#define CTR_saved(c1,c2)   ((c1)->saved[c2])
#define CTR_str(c1,c2)     ((c1)->string[c2])
#define CTR_misc_str(c1,c2)((c1)->misc_str[c2])
#define CTR_num(c1)        ((c1)->num)
#define CTR_saved_sum(c1)  ((c1)->svd_sum)

#define ADDTOSUM 0x1 

#define CTR_add_to_sum(c1,c2) ((c1)->flags[c2] & ADDTOSUM)


/*........ TY counter group declaration.......... */

int cwh_stat_ty_c[c_TY_LAST];                       /* array of counters (external)  */

static int cwh_stat_ty_last[c_TY_LAST];             /* saved array of counters */

static const int cwh_stat_ty_flags[c_TY_LAST] =  {  /* flags for printing counters */
				ADDTOSUM,
				ADDTOSUM,
				ADDTOSUM,
				ADDTOSUM,
				ADDTOSUM,
				ADDTOSUM,
				ADDTOSUM,
				ADDTOSUM,
				ADDTOSUM,
				0,0,0,0,0,0} ;

static char * cwh_stat_ty_c_TY_string[c_TY_LAST] = {   /* string printed with counter */
	                    " array       : %6d (+%4d) \n",
	                    " ptr         : %6d (+%4d) \n",
	                    " f90ptr      : %6d (+%4d) \n",
	                    " procedure   : %6d (+%4d) \n",
	                    " mis + log   : %6d (+%4d) \n",
                	    " struct      : %6d (+%4d) \n",
                	    " ty_copy     : %6d (+%4d) \n",
                	    " uniq ptr    : %6d (+%4d) \n",
	                    " d_type      : %6d (+%4d) \n",
	                    " match array :                       %6d (+%4d) \n",
	                    " match dope  :                       %6d (+%4d) \n",
	                    " match other :                       %6d (+%4d) \n",
	                    " cleared     :                       %6d (+%4d) \n",
	                    " reused      :                       %6d (+%4d) \n",
	                    " f90ptr nomat:                       %6d (+%4d) \n",
                           };

static char * cwh_stat_ty_misc_string[3] = {   /* header & summation string */
			    " TY details \n",
                            " ----------------------------\n",
                            "               %6d (+%4d) \n\n"
                           };

static  CTR ty_c = { cwh_stat_ty_c,           /* details of TY counters */
		     cwh_stat_ty_last, 
		     cwh_stat_ty_flags, 
		     cwh_stat_ty_c_TY_string, 
		     cwh_stat_ty_misc_string, 
		     c_TY_LAST,
		     0,
		   } ;



/* forward definitions */

static void cwh_stats_print_details( CTR * cp )  ;


