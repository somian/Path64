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


/**
*** Description:
***
***   This file contains information about the Whirl Browser, which can be 
***   used to interactively debug programs while running the LNO Phase.  
***
***   To invoke the whirl browser from dbx, type "p debug()".  You can get 
***   a list of available options by typing 'H' at the prompt "WB>". 
***
***  Exported functions: 
***
***   void WB_OMP_Initialize(WN* global_fd, INT sanity_check_level)
***
***	Initializes the whirl browser to operated on the program unit 
***     'global_fd' and sets the 'sanity_check_level'. 
***
***   void WB_OMP_Terminate();
***
***     Makes the whirl browser inoperative. 
***
***   void WB_Set_Sanity_Check_Level(INT sanity_check_level) 
***	
***	Set the sanity check level to value 'sanity_check_level'. 
***	The following are valid values: 
***	  WBC_DISABLE => Disable sanity checking 
***	  WBC_DU_ONLY => Do DU sanity checking only 
***	  WBC_DU_AND_ARRAY => Check DU and array dependences 
***	  WBC_FULL_SNL => Do Full SNL sanity checking 
***	Sanity checking is invoked by using the 'C' command. 
***   
***   const char* WB_Whirl_Symbol(WN* wn, BOOL print_type=FALSE)
***
***	 A printable string of characters for whirl node 'wn'. For 
***	 loads and stores, the symbol is printed, if any.  For do loops,
***	 the symbol of the do loop is printed.  
***
***   extern BOOL WB_Dep_Symbol(WN* wn, char buffer[], INT max_string)
***
***	 Write an abbreviated version of node 'wn' to the first 
***	 'max_string' characters of 'buffer'. 
**/

#ifndef debug_INCLUDED
#define debug_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#define WBC_MIN		        0
#define WBC_DISABLE		0
#define WBC_DU_ONLY		1
#define WBC_DU_AND_ARRAY	2
#define WBC_FULL_SNL		3
#define WBC_MAX			3
 
extern void WB_LNO_Initialize(WN* global_fd, struct DU_MANAGER* du_mgr, 
  struct ALIAS_MANAGER* alias_mgr, INT sanity_check_level); 
extern void WB_LNO_Terminate(void); 
extern void WB_Set_Sanity_Check_Level(INT sanity_check_level); 
extern const char* WB_Whirl_Symbol(WN* wn); 
extern const char* WB_Whirl_Symbol_Type(WN* wn); 
extern void Lisp_Loops(WN* wn_root, FILE *fp);
extern BOOL WB_Dep_Symbol(WN* wn, char buffer[], INT max_string);

#ifdef __cplusplus
}
#endif


#endif /* debug_INCLUDED */

