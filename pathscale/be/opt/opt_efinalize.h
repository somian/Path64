//-*-c++-*-
// ====================================================================
// ====================================================================
//
// Module: opt_efinalize.h
// $Revision$
// $Date$
// $Author$
// $Source$
//
// ====================================================================
//
// Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
//
/*
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
//
// ====================================================================
// ====================================================================


class REHASH_INFO {
private:
  typedef enum {
    RI_NONE              = 0x0,
    RI_CODEREP_UNOWNABLE = 0x1
  } RI_FLAGS;

  UINT32            _flags;
  EXP_OCCURS *      _owning_t_ver;
  UINT32            _max_rehash_cost;
  CODEREP    *const _coderep;

public:
  REHASH_INFO(CODEREP *coderep) : _coderep(coderep)
    {
      _owning_t_ver    = NULL;
      _max_rehash_cost = 0;
      _flags           = 0;
    }

  CODEREP    *Coderep() const             { return _coderep; }

  void        Set_coderep_unownable(void)
    { _flags |= RI_CODEREP_UNOWNABLE; }

  BOOL        Coderep_unownable(void) const
    { return _flags & RI_CODEREP_UNOWNABLE; }

  void        Set_max_rehash_cost(UINT32 cost)
    { _max_rehash_cost = cost; }

  UINT32      Max_rehash_cost(void) const { return _max_rehash_cost; }

  void        Set_owning_t_ver(EXP_OCCURS *occ)
    { _owning_t_ver = occ; }

  EXP_OCCURS *Owning_t_ver(void) const    { return _owning_t_ver; }

  void        Print(FILE *fp) const
    {
      if (_owning_t_ver != NULL) {
	_owning_t_ver->Print(fp);
      }
      else {
	fprintf(fp, "  unowned");
      }
      if (Coderep_unownable()) {
	fprintf(fp, " / unownable");
      }
      fprintf(fp, "\n");
    }
};
