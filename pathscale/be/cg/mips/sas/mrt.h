/*
 * Copyright 2005, 2007 PathScale Inc.  All Rights Reserved.
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

#ifndef _MRT_H
#define _MRT_H

#include <stdint.h>
#include "swp.h"


class RESOURCE {
 private:
  const RESOURCE_MASK mask;
  const char* name;
  const RESOURCE_ID id;
  const uint8_t avail_per_cycle;

 public:
  RESOURCE( RESOURCE_MASK mask, char* name, RESOURCE_ID id, uint8_t avail ) :
    mask( mask ), name( name ), id( id ), avail_per_cycle( avail )
    {}

  const char* RESOURCE_name() const { return name; }
  const uint8_t RESOURCE_id() const { return id; }
  const RESOURCE_MASK RESOURCE_mask() const      { return mask; }
  const uint8_t RESOURCE_avail_per_cycle() const { return avail_per_cycle; }
};


class RESERVATION {
 private:
  const char* name;
  const uint8_t latency;     /* how many cycle(s) later the result will be available. */
  const RESOURCE** resource; /* an array of resources that this op could use. */
  uint8_t   size;            /* the number of resources that could be used. */
  RESOURCE_MASK mask;        /* a mask of resources. */

 public:
  RESERVATION( char* name, uint8_t latency, const RESOURCE** res ) :
    name( name ), latency( latency ), resource( res )
    {
      mask = 0;
      size = 0;
      while( resource[size] != NULL ){
	mask |= resource[size]->RESOURCE_mask();
	size++;
      }
      //printf( "%s %d %x\n", name, size, mask );
    }

  const RESOURCE_MASK RESERVATION_mask() const { return mask; }
  const uint8_t RESERVATION_size() const { return size; }
  const uint8_t RESERVATION_latency() const    { return latency; }
  const RESOURCE* RESERVATION_resource( int i ) const { return resource[i]; }
  const char* RESERVATION_name() const         { return name; }
};


class MRT {
 private:
  /* Only routines which will access Resource_Table will notice mii. */
  int mii;
  int MaxLive;
  int rtable_size;

  struct Resource_Table_Entry {
    uint8_t  nExFp;  // number of Ex and Fp pipes are allowed per cycle
    uint8_t* nRes;   // number of nRes[res_id] ops are allowed per cycle
  };

  Resource_Table_Entry*  Resource_Table;

  bool Consume_Resource( OPR*, OPR*, int, const RESOURCE* ) const;
  void Adjust_OPR_res_mask( OPR*, int ) const;
  void Setup_Resource_Table();

  TALK talk;
  
 public:
  MRT( TALK talk ) : talk( talk )
    {
      mii = MaxLive = rtable_size = 0;
      Resource_Table = NULL;
    }

  ~MRT()
    {
      // memory would be freed in mempool pop
      if( rtable_size > 0 ){
	for( int i = rtable_size - 1; i >= 0; i-- ){
	  Resource_Table[i].nRes = NULL;
	}

	Resource_Table = NULL;
      }
    }

  static const uint8_t Types_Per_Cycle;  // how many different types of FUs per cycle
  static const RESERVATION* Reservation_Table[];
  static const RESOURCE* res_per_cycle[];

  void Init( SBB* bb, int = 0 );
  void Set_MII( int mii ) { this->mii = mii; ASSERT( mii <= rtable_size ); }
  int  Size() const       { return rtable_size; }
  void Reserve_Resources( OPR*, int );
  void Reserve_Register( int );
  void Unreserve_Resources( OPR*, int );
  int  Find_First_Schedule_Cycle( OPR* );
  int  Find_Last_Schedule_Cycle( OPR* );
  bool Resources_Available( OPR*, int, bool );
  const char* Resource_Name( const OPR* ) const;
  int Compute_MaxLive( SBB* );

  void Print( FILE* f = stdout ) const;
};


void Adjust_Latency( DEP* );


#endif
