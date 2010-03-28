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


#ifndef wb_buffer_INCLUDED
#define wb_buffer_INCLUDED "wb_buffer.h"

class WN;

const INT WB_BUFFER_MAX = 132; 

enum WB_SKIP_CLASS {
  WB_SKIP_NONE, 
  WB_SKIP_ALPHANUMERIC, 
  WB_SKIP_NUMERIC, 
  WB_SKIP_HEX,
};

class WB_BUFFER { 
  char _buffer[WB_BUFFER_MAX]; 
  INT _buffer_start; 
public: 
  WB_BUFFER() { _buffer_start = 0; _buffer[0] = '\0'; } 
  void Reset_Buffer() { _buffer_start = 0; } 
  void Load_Buffer(); 
  void Load_Buffer(const char s[]); 
  void Scan_Blanks_And_Tabs(); 
  void Skip_To_Separator(WB_SKIP_CLASS skip_type); 
  void Load_Loop(WN** wn_loop);
  void Load_UINT32(UINT32* int_value);
  void Load_mINT32(mINT32* value);
  void Load_mINT64(mINT64* value);
  void Load_Integer(INT* int_value); 
  void Load_Double(double* value);
  void Load_Boolean(BOOL* bool_value, BOOL default_present = FALSE,
    BOOL default_value = FALSE);
  void Skip_Chars(INT int_value); 
  char Scan_Character(); 
  void Pushback_Character(); 
  void Scan_Integer(INT* int_value); 
  void Scan_Unsigned(UINT32* int_value); 
  void Scan_HexInteger(INT* int_value); 
  void Scan_Alphanumeric(char s[]); 
  BOOL Is(char ch) {return _buffer[_buffer_start] == ch;};
  BOOL Is_Integer();  
  char Get_Command(); 
}; 

#endif /* wb_buffer_INCLUDED */ 
