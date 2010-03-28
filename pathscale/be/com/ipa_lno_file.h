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



#ifndef _IPA_LNO_FILE
#define _IPA_LNO_FILE "ipa_lno_file.h"

#if ! defined(BUILD_OS_DARWIN)
#ifndef __SYS_ELF_H__ 
#include <elf.h>
#endif 
#endif /* ! defined(BUILD_OS_DARWIN) */

#ifndef pu_info_INCLUDED
#include "pu_info.h"
#endif 

#ifndef __IR_BWRITE_H__
#include <sys/types.h> 
#include "ir_bwrite.h"
#endif 

#ifndef cxx_ipa_lno_summary_INCLUDED
#include "ipa_lno_summary.h"
#endif 

#ifndef ipa_section_INCLUDED
#include "ipa_section.h"
#endif

class SUMMARY_VALUE; 
class SUMMARY_EXPR; 

//-----------------------------------------------------------------------
// Error codes for reading and writing IPALNO file. 
//-----------------------------------------------------------------------

const INT IPALNO_SUCCESS	   =  0; 
const INT IPALNO_FORMAT_ERROR 	   = -1; 
const INT IPALNO_ABI_MISMATCH      = -2; 
const INT IPALNO_REVISION_MISMATCH = -3;
const INT IPALNO_READER_ERROR      = -4; 
const INT IPALNO_CLOSE_ERROR	   = -5; 

//-----------------------------------------------------------------------
// NAME: Input_File  
// FUNCTION: Information about IPALNO while being read. 
//-----------------------------------------------------------------------

const INT MAX_FILE_REVISION = 80;

typedef struct input_file { 
  void* mapped_address; 	// start address of mmapped region
  off_t mapped_size; 		// size of mmapped region   
  char file_revision[MAX_FILE_REVISION];  // file revision string 
} Input_File; 

//-----------------------------------------------------------------------
// NAME: IPA_LNO_READ_FILE
// FUNCTION: Information and methods for IPALNO file while being read. 
//-----------------------------------------------------------------------

class IPA_LNO_READ_FILE {
private:
  INT Check_Elf_Header(); 
  INT Check_Section_Headers();
  INT Check_Input();
  Elf64_Shdr* Section_Header(Elf64_Word info);
  INT Original_Ivar_Count();
  DYN_ARRAY<IVAR> _extra_ivars; 
public: 
  IPA_LNO_READ_FILE(MEM_POOL* pool) 
    { ifl = NULL; _extra_ivars.Set_Mem_Pool(pool); }; 
  Input_File* ifl; 
  INT Open_Read_File(const char *file_name); 
  void* Section_Address(Elf64_Word info);
  INT Section_Size(Elf64_Word info);
  void Close_Read_File(); 
    // The following methods are for specific sections. 
  IPA_LNO_SUMMARY_PROCEDURE* Procedure(INT i);
  INT Procedure_Count();
  PROJECTED_REGION* Projected_Region(INT i);
  INT Projected_Region_Count();
  PROJECTED_NODE* Projected_Node(INT i);
  INT Projected_Node_Count();
  TERM* Term(INT i);
  INT Term_Count();
  IVAR* Ivar(INT i);
  INT Ivar_Count();
  INT Add_Translated_Ivar(const IVAR& ivar); 
  INT Add_Translated_Ivar_Unique(const IVAR& ivar); 
  IPA_LNO_SUMMARY_FORMAL* Formal(INT i);
  INT Formal_Count();
  IPA_LNO_SUMMARY_GLOBAL* Global(INT i);
  INT Global_Count();
  SUMMARY_VALUE* Value(INT i);
  INT Value_Count();
  SUMMARY_EXPR* Expr(INT i);
  INT Expr_Count();
}; 

//-----------------------------------------------------------------------
// NAME: IPA_LNO_WRITE_FILE
// FUNCTION: Information and methods for IPALNO file while being written. 
//-----------------------------------------------------------------------

class IPA_LNO_WRITE_FILE { 
private: 
  Section* Create_Or_Get_Section(Elf64_Word sh_info, const char *name); 
  void Create_Sections();
  INT Create_Temp_File(); 
  Elf64_Off Create_String_Table_Section(Elf64_Shdr *strtab_sec);
  void Write_Headers(Elf64_Off e_shoff, Elf64_Shdr* strtab_sec);
  void Write_Revision(); 
  void Write_Cleanup(); 
public: 
  Output_File* ofl;
  IPA_LNO_WRITE_FILE() {ofl = NULL;};
  void Open_Write_File(char *file_name); 
  void Write_Section(Elf64_Word sh_info, const char* name, void* buf, INT size);
  INT Close_Write_File(); 
};

#endif /* IPA_LNO_FILE */

