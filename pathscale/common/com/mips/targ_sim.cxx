/*
 * Copyright (C) 2008 PathScale, LLC.  All Rights Reserved
 */

/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */

/*
 * Copyright 2002, 2003, 2004 PathScale, Inc.  All Rights Reserved.
 */

/*

  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it would be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Further, this software is distributed without any warranty that it is
  free of the rightful claim of any third person regarding infringement 
  or the like.  Any license provided herein, whether implied or 
  otherwise, applies only to this software file.  Patent licenses, if 
  any, provided herein do not apply to combinations of this program with 
  other software, or any other product whatsoever.  

  You should have received a copy of the GNU General Public License along
  with this program; if not, write the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston MA 02111-1307, USA.

  Contact information:  Silicon Graphics, Inc., 1600 Amphitheatre Pky,
  Mountain View, CA 94043, or:

  http://www.sgi.com

  For further information regarding this notice, see:

  http://oss.sgi.com/projects/GenInfo/NoticeExplan

*/


/*
 * This defines the ABI subprogram interface,
 * and is used to determine how parameters and results are passed.
 * We have an array of tables, where each table describes the info
 * for one abi.  The array is indexed by the TARGET_ABI enumeration.
 * The register values are the PREG offsets, so these values can be
 * used in WHIRL.
 */

#define TRACE_ENTRY(x)
#define TRACE_EXIT(x)
#define TRACE_EXIT_i(x,i)

#include <limits.h>
#include "defs.h"
#include "mtypes.h"
#include "errors.h"
#include "erglob.h"
#include "stab.h"
#include "config_targ.h"
#include "targ_sim.h"

#include "targ_sim_body.h"


#define I0 (Int_Preg_Min_Offset - 1)
#define F0 Float_Preg_Min_Offset

#if (__GNUC__ == 2)
static
#endif /* _LP64 */
SIM SIM_Info[] = {
	/* flags */
	/* int args, flt args, dbl args */
	/* int res , flt res, dbl res */
	/* int type, flt type, dbl type */
	/* save area, formal-area, var ofst */
	/* struct arg, struct res, slink, pic */
  {/* ABI_UNDEF */
	0,
	{0,0,0}, {0,0,0}, {0,0,0},
	{0,0,0}, {0,0,0}, {0,0,0},
	0, 0, 0,
	0, 0, 0, 
	0, 0, 0, 0
  },
  { /* ABI_N32 */
	SIM_FLT_AFTER_INT | SIM_COORD_MEM_REG | SIM_COORD_INT_FLT 
	| SIM_REG_STRUCTS | SIM_FLT_RTN_COMPLEX | SIM_DBL_REG_FIELDS ,
	{I0+4,I0+11,1}, {F0+12,F0+19,1}, {F0+12,F0+19,1},
	{I0+2,I0+3,1},  {F0,F0+2,2}, {F0,F0+2,2},
	MTYPE_I8, MTYPE_F8, MTYPE_F8,
	0, 64, -64, 
	-1, 128, I0+2, I0+25
  },
  { /* ABI_N64 */
	SIM_FLT_AFTER_INT | SIM_COORD_MEM_REG | SIM_COORD_INT_FLT 
	| SIM_REG_STRUCTS | SIM_FLT_RTN_COMPLEX | SIM_DBL_REG_FIELDS ,
	{I0+4,I0+11,1}, {F0+12,F0+19,1}, {F0+12,F0+19,1},
	{I0+2,I0+3,1},  {F0,F0+2,2}, {F0,F0+2,2},
	MTYPE_I8, MTYPE_F8, MTYPE_F8,
	0, 64, -64, 
	-1, 128, I0+2, I0+25
  }
};

/* return whether preg is a return preg */
extern BOOL 
Is_Return_Preg (PREG_NUM preg)
{
	return (preg >= First_Int_Preg_Return_Offset
	     && preg <= Last_Int_Preg_Return_Offset)
            || (preg >= First_Float_Preg_Return_Offset
             && preg <= Last_Float_Preg_Return_Offset);
}

/* return whether preg is an output preg */
extern BOOL 
Is_Int_Output_Preg (PREG_NUM preg)
{
  Fail_FmtAssertion (
    ("Is_Int_Output_Preg not applicable to MIPS targets"));
}

/* return whether preg is an input preg */
extern BOOL
Is_Formal_Preg (PREG_NUM preg)
{
	return (preg >= First_Int_Preg_Param_Offset
		&& preg <= (First_Int_Preg_Param_Offset 
			+ MAX_NUMBER_OF_REGISTER_PARAMETERS) )
	    || (preg >= First_Float_Preg_Param_Offset
		&& preg <= (First_Float_Preg_Param_Offset 
			+ MAX_NUMBER_OF_REGISTER_PARAMETERS) );
}


// Given an offset from the start of a struct, determine if that struct
// has a double field starting at that offset.
static BOOL
Type_Has_Double_At_Offset(TY_IDX ty, INT64 offset)
{
  // 14910: I originally used "while" here instead of "if" because thei
  // n32/64 ABI appeared to imply that nested structs should be treated
  // as if flattened.  However, both gcc and the SGI compiler disagree
  // with this interpretation.
  if (ty != 0 && TY_kind(ty) == KIND_STRUCT && TY_fld(ty) != FLD_HANDLE()) {
    if (TY_is_union(ty)) return FALSE;  // 14935
    // Find the field that overlaps this offset
    FLD_ITER fld_iter = Make_fld_iter(TY_fld(ty));
    INT64 fld_ofst = 0;
    do {
      ty = FLD_type(fld_iter);
      fld_ofst = FLD_ofst(fld_iter);
    } while (! FLD_last_field(fld_iter) && FLD_ofst(++fld_iter) <= offset);
    offset -= fld_ofst;
  }

  if (TY_mtype(ty) == MTYPE_F8 && offset == 0) return TRUE;
  // What about C8, V8F4
  return FALSE;
} // Type_Has_Double_At_Offset


static BOOL
Struct_Has_One_Float (const TY_IDX ty, TYPE_ID &ftype)
{
    if (TY_is_union (ty))
	return FALSE;

    if (TY_fld (ty).Is_Null ())
	return FALSE;

    FLD_HANDLE fld = TY_fld (ty);

    if (FLD_last_field (fld) && FLD_type (fld) != 0 &&
	TY_kind (FLD_type (fld)) == KIND_SCALAR) {
	ftype = TY_mtype (FLD_type (fld));
	if (MTYPE_float (ftype))
	    return TRUE;
    }
    ftype = MTYPE_V;
    return FALSE;
} // Struct_Has_One_Float


// check if two fields in a struct overlaps
static BOOL
no_overlap (const FLD_HANDLE fld1, const FLD_HANDLE fld2)
{
    if (FLD_ofst (fld1) <= FLD_ofst (fld2))
	return FLD_ofst(fld1) + TY_size (FLD_type (fld1)) <= FLD_ofst (fld2);
    else
	return FLD_ofst(fld2) + TY_size (FLD_type (fld2)) <= FLD_ofst (fld1);
} // no_overlap

static BOOL
Struct_Has_Two_Floats (const TY_IDX ty, TYPE_ID& ftype1, TYPE_ID& ftype2)
{
    if (TY_is_union (ty))
	return FALSE;

    if (TY_fld (ty).Is_Null ())
	return FALSE;

    FLD_HANDLE fld1 = TY_fld (ty);

    if (FLD_last_field (fld1))
	return FALSE;

    FLD_HANDLE fld2 (FLD_next (fld1));

    if (FLD_last_field (fld2) &&
	FLD_type (fld1) != 0 && TY_kind (FLD_type (fld1)) == KIND_SCALAR &&
	FLD_type (fld2) != 0 && TY_kind (FLD_type (fld2)) == KIND_SCALAR &&
	no_overlap (fld1, fld2)) {
	ftype1 = TY_mtype (FLD_type(fld1));
	ftype2 = TY_mtype (FLD_type(fld2));
	if (MTYPE_float (ftype1) && MTYPE_float (ftype2))
	    return TRUE;
    }
    ftype1 = ftype2 = MTYPE_V;
    return FALSE;

} // Struct_Has_Two_Floats

static BOOL
Is_Simulated_Type (TYPE_ID mtype)
{
	switch (mtype) {
	case MTYPE_FQ: return TRUE;
	case MTYPE_C4: return TRUE;
	case MTYPE_C8: return TRUE;
	case MTYPE_CQ: return TRUE;
	default:	return FALSE;
	}
}

/* This routine figures out the mtypes of the return registers that are 
 * used for returning an object of the given type.
 * This returns the mtypes to use for the CALL opcode in high-level whirl.
 * This means that returns of simulated objects, like FQ, are just shown
 * as returning FQ, which will later be split into F8F8.
 * However, structures that return in registers are specified explicitly.
 */
/*ARGSUSED*/
extern void
Get_Return_Mtypes (
  TY_IDX rtype,		/* The result type */
  Mtype_Return_Level level,	/* whether to lower the mtypes */
  TYPE_ID *mreg1,	/* out: mtype for result register 1 */
  TYPE_ID *mreg2)	/* out: mtype for result register 2 */
{
  Fail_FmtAssertion (
    ("Get_Return_Mtypes should not be invoked; invoke Get_Return_Info instead"));
}

/* This routine figures out which return registers are to be used
 * for returning an object with the given mtypes.
 * It is assumed that the mtypes will be determined by calling
 * Get_Return_Mtypes.
 */
/*ARGSUSED*/
extern void
Get_Return_Pregs (
  TYPE_ID mreg1,	/* in:  mtype for result register 1 */
  TYPE_ID mreg2,	/* in:  mtype for result register 2 */
  PREG_NUM *rreg1,	/* out: result register 1 */
  PREG_NUM *rreg2)	/* out: result register 2 */
{
  Fail_FmtAssertion (
    ("Get_Return_Pregs should not be invoked; invoke Get_Return_Info instead"));
}

RETURN_INFO
Get_Return_Info (TY_IDX rtype, Mtype_Return_Level level)
{
  TYPE_ID mtype = TY_mtype (rtype);
  RETURN_INFO info;
  INT32 i; 


  info.return_via_first_arg = FALSE;

  switch (mtype) {

    case MTYPE_UNKNOWN:

      // FORTRAN character array
      info.count = 0;
      // f90 already has made visible the arg for arrays
      // info.return_via_first_arg = TRUE;
      break;

    case MTYPE_V:

      info.count = 0;
      break;

    case MTYPE_I1:
    case MTYPE_I2:
    case MTYPE_I4:
    case MTYPE_I8:
    case MTYPE_U1:
    case MTYPE_U2:
    case MTYPE_U4:
    case MTYPE_U8:
    case MTYPE_A4:
    case MTYPE_A8:

      info.count = 1;
      info.mtype [0] = mtype;
      info.preg  [0] = PR_first_reg(SIM_INFO.int_results);
      break;

    case MTYPE_F4:
    case MTYPE_F8:
    case MTYPE_V8F4:

      info.count = 1;
      info.mtype [0] = mtype;
      info.preg  [0] = PR_first_reg(SIM_INFO.flt_results);
      break;

    case MTYPE_FQ:

      if (level == No_Simulated) {

        info.count     = 2;
        info.mtype [0] = MTYPE_F8;
        info.mtype [1] = MTYPE_F8;
        info.preg  [0] = PR_first_reg(SIM_INFO.flt_results);
        info.preg  [1] =   PR_first_reg(SIM_INFO.flt_results)
                         + PR_skip_value(SIM_INFO.flt_results);
      }

      else {

        info.count     = 1;
        info.mtype [0] = mtype;
        info.preg  [0] = PR_first_reg(SIM_INFO.flt_results);
      }
      break;

    case MTYPE_C4:
    case MTYPE_C8:

      if (level == Use_Simulated) {

        info.count     = 1;
        info.mtype [0] = mtype;
        info.preg  [0] = PR_first_reg(SIM_INFO.flt_results);
      }

      else {

        info.count     = 2;
        info.mtype [0] = Mtype_complex_to_real(mtype);
        info.mtype [1] = Mtype_complex_to_real(mtype);
        info.preg  [0] = PR_first_reg(SIM_INFO.flt_results);
        info.preg  [1] =   PR_first_reg(SIM_INFO.flt_results)
                         + PR_skip_value(SIM_INFO.flt_results);
      }
      break;

    case MTYPE_CQ:

      // Bug 10742: MTYPE_CQ cannot fit into $f0 and $f2.
      info.count = 0;
      info.return_via_first_arg = TRUE;
      break;

    case MTYPE_M:

#ifdef KEY  // bug 10758
      /* MIPS abi specifies that a struct containing only one or two
       * floating point fields should be returned in $f0 (and $f2);
       * In particular, struct complex should be returned in $f0 and $f2 */
      {
	const TY& ty = Ty_Table[rtype];
	if (! TY_fld(ty).Is_Null() && level != Use_Simulated) {
	  FLD_ITER iter = Make_fld_iter(TY_fld(ty));
	  TYPE_ID mtype1 = TY_mtype(FLD_type(iter));
	  if ((mtype1 == MTYPE_C4 ||	// Exactly 1 complex field.  Bug 12831.
	       mtype1 == MTYPE_C8) &&
	      FLD_last_field(iter)) {
	    TYPE_ID float_type = (mtype1 == MTYPE_C4) ? MTYPE_F4 : MTYPE_F8;
	    info.count     = 2;
	    info.mtype [0] = float_type;
	    info.preg  [0] = PR_first_reg(SIM_INFO.flt_results);
	    info.mtype [1] = float_type;
	    info.preg  [1] = PR_first_reg(SIM_INFO.flt_results)
			     + PR_skip_value(SIM_INFO.flt_results);
	    break;
	  } else if (mtype1 == MTYPE_F4 || mtype1 == MTYPE_F8 ||
		     mtype1 == MTYPE_V8F4) {
	    info.count     = 1;
	    info.mtype [0] = mtype1;
	    info.preg  [0] = PR_first_reg(SIM_INFO.flt_results);
	    if (FLD_last_field(iter)) break;	// Exactly 1 float field.
	    if (FLD_last_field(++iter)) {
	      TYPE_ID mtype2 = TY_mtype(FLD_type(iter));
	      if (mtype2 == MTYPE_F4 || mtype2 == MTYPE_F8 ||
		  mtype2 == MTYPE_V8F4) {  // Exactly 2 float fields.
		info.count     = 2;
		info.mtype [1] = mtype2;
		info.preg  [1] =   PR_first_reg(SIM_INFO.flt_results)
				 + PR_skip_value(SIM_INFO.flt_results);
		break;
	      }
	    }
	  }
	}
      }
#endif

      info.count = 0;
      info.return_via_first_arg = TRUE;

      if (SIM_INFO.max_struct_result != 0) {

        UINT64 size = TY_size(Ty_Table[rtype]);

        if (size > 0 && 8 * size <= SIM_INFO.max_struct_result) {

            int n =   (size + MTYPE_RegisterSize(SIM_INFO.int_type) - 1)
                    / MTYPE_RegisterSize(SIM_INFO.int_type);
            PREG_NUM reg = PR_first_reg(SIM_INFO.int_results);

            info.return_via_first_arg = FALSE;
            info.count = n;
            for (int i = 0; i < n; i++) {

              info.mtype [i] = SIM_INFO.int_type;
              info.preg  [i] = reg++;
            }
        }
      }
      break;

    default:

      info.count = 0;
      Fail_FmtAssertion ("Invalid return mtype %s encountered",
                         (MTYPE_name(mtype)));
      break;
  } /* switch (mtype) */

  for (i = info.count; i < MAX_NUMBER_OF_REGISTERS_FOR_RETURN; i++) {

    info.mtype [i] = MTYPE_V;
    info.preg  [i] = 0;
  }

  return info;
} /* Get_Return_Info */

static BOOL First_Param_In_Return_Reg = FALSE;

static PLOC
Setup_Parameter_Locations (TY_IDX pu_type)
{
    static PLOC plocNULL;

    TY_IDX ret_type = (TY_kind(pu_type) == KIND_FUNCTION ? TY_ret_type(pu_type)
			: pu_type);
    RETURN_INFO info = Get_Return_Info (ret_type, No_Simulated);
    First_Param_In_Return_Reg = (RETURN_INFO_return_via_first_arg(info) 
			       & SIM_return_addr_via_int_return_reg);
    if (TY_is_varargs (pu_type)) {
	// find last fixed parameter
	TYLIST_IDX idx = TY_tylist (pu_type);
	Last_Fixed_Param = -1;
	for (++idx; Tylist_Table[idx] != 0; ++idx)
	    ++Last_Fixed_Param;
	// old style varargs is counting va_alist and should not
	if ( ! TY_has_prototype(pu_type))
	    --Last_Fixed_Param;
	// account for functions returning to first parameter
	if (TY_return_to_param (pu_type))
	    ++Last_Fixed_Param;
    } else
	Last_Fixed_Param = INT_MAX;

    Current_Param_Num = -1;
    Last_Param_Offset = 0;
    return plocNULL;
} // Setup_Parameter_Locations



static inline PREG_NUM
Get_Current_Float_Preg_Num (Preg_Range pr)
{
	PREG_NUM i;
        TRACE_ENTRY("Get_Current_Float_Preg_Num");
	i = PR_first_reg(pr) + (Current_Param_Num * PR_skip_value(pr));
	if (i > PR_last_reg(pr)) {
        	TRACE_EXIT_i("Get_Current_Float_Preg_Num", 0);
		return 0;
        }
	else {
		i = PR_first_reg(pr) + (Current_Param_Num * PR_skip_value(pr));
        	TRACE_EXIT_i("Get_Current_Float_Preg_Num", i);
		return i;
        }
}


static PLOC
Get_Parameter_Location (TY_IDX ty, BOOL is_output)
{
    PLOC ploc;				// return location

    ploc.reg = 0;
    ploc.start_offset = Last_Param_Offset;
    ploc.size = 0;
    ploc.vararg_reg = 0;               // to silence purify
    if (TY_kind (ty) == KIND_VOID) {
	return ploc;
    }

    /* check for array case where fe doesn't fill in right btype */
    TYPE_ID pmtype = Fix_TY_mtype (ty);	/* Target type */
    ploc.size = MTYPE_RegisterSize(pmtype);

    if (First_Param_In_Return_Reg) {
	First_Param_In_Return_Reg = FALSE;
	ploc.reg = PR_first_reg(SIM_INFO.int_results);
	return ploc;
    }
    ++Current_Param_Num;
    if (TY_align_exp (ty) == 4 && (Current_Param_Num % 2) == 1) {
	/* skip a parameter slot so quad-aligned */
	++Current_Param_Num;
	/* adjust Last_Fixed_Param in varargs case */
	if (Last_Fixed_Param < INT_MAX)
	  ++Last_Fixed_Param;
	ploc.start_offset += MTYPE_RegisterSize(SIM_INFO.flt_type);
    }

    INT rpad = 0;			/* padding to right of object */

    switch (pmtype) {
	
    case MTYPE_I1:
    case MTYPE_U1:
    case MTYPE_I2:
    case MTYPE_U2:
    case MTYPE_I4:
    case MTYPE_U4:
    case MTYPE_A4:
      if (Target_Byte_Sex == BIG_ENDIAN) {
	/* want to right-justify the object */
	ploc.start_offset += (MTYPE_RegisterSize(SIM_INFO.int_type) -
			      ploc.size);
      }
      else {
	/* Pad to doubleword; leave address alone   */
          rpad = (MTYPE_RegisterSize(SIM_INFO.int_type) - ploc.size);
      }
	ploc.reg = Get_Current_Preg_Num (SIM_INFO.int_args);
	break;
	
    case MTYPE_I8:
    case MTYPE_U8:
    case MTYPE_A8:
	ploc.reg = Get_Current_Preg_Num (SIM_INFO.int_args);
	if (MTYPE_size_reg(SIM_INFO.int_type) < MTYPE_size_reg(pmtype)) {
	    Current_Param_Num++;
	    /* adjust Last_Fixed_Param in varargs case */
	    if (Last_Fixed_Param < INT_MAX)
	  	++Last_Fixed_Param;
	}
	break;
	
    case MTYPE_F4:
    case MTYPE_F8:
    case MTYPE_V8F4:
	/* want to left-justify the object */
	rpad = MTYPE_RegisterSize(SIM_INFO.flt_type) - ploc.size;
	if (Current_Param_Num > Last_Fixed_Param && !SIM_varargs_floats) {
	    /* varargs causes float args to be int regs */
	    ploc.reg = Get_Current_Preg_Num (SIM_INFO.int_args);
	} else {
	    ploc.reg = Get_Current_Float_Preg_Num (SIM_INFO.flt_args);
	    ploc.vararg_reg = Get_Current_Preg_Num (SIM_INFO.int_args);
	}
	break;
	
    case MTYPE_FQ:
	if (Current_Param_Num > Last_Fixed_Param && !SIM_varargs_floats) {
	    /* varargs causes float args to be int regs */
	    ploc.reg = Get_Current_Preg_Num (SIM_INFO.int_args);
	} else {
	    ploc.reg = Get_Current_Float_Preg_Num (SIM_INFO.flt_args);
	    ploc.vararg_reg = Get_Current_Preg_Num (SIM_INFO.int_args);
	}
	Current_Param_Num++;
	/* adjust Last_Fixed_Param in varargs case */
	if (Last_Fixed_Param < INT_MAX)
	    ++Last_Fixed_Param;
	break;
	
    case MTYPE_C4:
    case MTYPE_C8:
    case MTYPE_CQ:
	ploc.reg = Get_Current_Float_Preg_Num (SIM_INFO.flt_args);
	Current_Param_Num++;
	/* adjust Last_Fixed_Param in varargs case */
	if (Last_Fixed_Param < INT_MAX)
	    ++Last_Fixed_Param;
	break;
	
    case MTYPE_M:
        {
	  ploc.size = TY_size (ty);
	  /* 1470, 12830, 14336: In C++ (but not C), an empty class/struct
	   * has non-zero size, so that distinct objects always have
	   * distinct addresses.
	   */
	  INT psize = TY_size (ty);
	  if (psize == 0 && PU_cxx_lang(Get_Current_PU()) ) psize = 1;
	  /* round up */
	  psize = (psize + MTYPE_RegisterSize(SIM_INFO.int_type) - 1)
	    / MTYPE_RegisterSize(SIM_INFO.int_type);
	  /* structures are left-justified, so may be padding at end */
	  rpad = (psize * MTYPE_RegisterSize(SIM_INFO.int_type)) - ploc.size;

	  if (Type_Has_Double_At_Offset(ty, 0) &&  // 14910
	      !(Current_Param_Num > Last_Fixed_Param && !SIM_varargs_floats)) {
	    ploc.reg = Get_Current_Float_Preg_Num (SIM_INFO.flt_args);
	    ploc.vararg_reg = Get_Current_Preg_Num (SIM_INFO.int_args);
	  } else {
	    ploc.reg = Get_Current_Preg_Num (SIM_INFO.int_args);
	  }
	  Current_Param_Num += psize - 1;
	  /* adjust Last_Fixed_Param in varargs case */
	  if (Last_Fixed_Param < INT_MAX)
	    Last_Fixed_Param += psize - 1;
	}
	break;
	
    default:
	FmtAssert (FALSE, ("Get_Parameter_Location:  mtype %s",
			   MTYPE_name(pmtype)));
    }
    Last_Param_Offset = ploc.start_offset + ploc.size + rpad;
    return ploc;
} // Get_Parameter_Location


struct PSTRUCT {
    BOOL	no_flt_regs;
    BOOL        first_call;
    BOOL        is_hfa;
    TYPE_ID     hfa_mtype;
    INT64	offset;			// offset from beginning of struct
    INT64	size;
    TY_IDX	ty;
    PREG_NUM	int_reg;
    PREG_NUM	flt_reg;

    PSTRUCT () : no_flt_regs (FALSE), first_call (TRUE),
		 is_hfa (FALSE), hfa_mtype (MTYPE_V),
		 offset (0), size (0), ty(TY_IDX_ZERO),
		 int_reg(0), flt_reg(0) {}
};

static PSTRUCT pstruct;

#define	PSTRUCT_no_flt_regs	pstruct.no_flt_regs
#define	PSTRUCT_first_call	pstruct.first_call
#define	PSTRUCT_hfa		pstruct.is_hfa
#define	PSTRUCT_hfa_mtype	pstruct.hfa_mtype
#define	PSTRUCT_offset		pstruct.offset
#define	PSTRUCT_size		pstruct.size
#define	PSTRUCT_ty		pstruct.ty
#define	PSTRUCT_int_reg		pstruct.int_reg
#define	PSTRUCT_flt_reg		pstruct.flt_reg


static void
Setup_Struct_Parameter_Locations (TY_IDX struct_ty)
{
    PSTRUCT_no_flt_regs = ( TY_is_union (struct_ty) ||
			    ( Current_Param_Num > Last_Fixed_Param &&
			      ! SIM_varargs_floats ) );  // 14934, 14935
    PSTRUCT_first_call = TRUE;
    PSTRUCT_hfa = FALSE;
    PSTRUCT_offset = 0;
    PSTRUCT_size = TY_size (struct_ty);
    PSTRUCT_ty = struct_ty;
}

static PLOC
Get_Struct_Parameter_Location (PLOC prev)
{
    // 14910: Rewrote Get_Struct_Parameter, because the older code did
    // not follow the n32/64 ABI rules for assigning float registers.
    TYPE_ID pmtype;
    PLOC next;
    INT ireg_size = MTYPE_RegisterSize(SIM_INFO.int_type);
    BOOL	onStack = (prev.reg == 0);

    if (PSTRUCT_first_call)
	PLOC_offset(next) = PLOC_offset(prev);
    else
	PLOC_offset(next) = PLOC_offset(prev) + PLOC_size(prev);

    if (PSTRUCT_offset >= PSTRUCT_size) {
      PLOC_size(next) = 0;
      return next;
    }

    if (onStack) {
      PLOC_reg(next) = 0;
      PSTRUCT_first_call = FALSE;

    } else if (PSTRUCT_first_call) {

      PSTRUCT_first_call = FALSE;
      PREG_NUM reg = PLOC_reg(next) = PLOC_reg(prev);
      // This code is a bit ugly, but correct for MIPS n32 and 64 ABI
      if (IS_INT_PREG(reg)) {
	PSTRUCT_int_reg = reg;
	PSTRUCT_flt_reg
	  = (PSTRUCT_int_reg - PR_first_reg(SIM_INFO.int_args))
	  / PR_skip_value(SIM_INFO.int_args)
	  * PR_skip_value(SIM_INFO.flt_args)
	  + PR_first_reg(SIM_INFO.flt_args);
      } else if (IS_FLT_PREG(reg)) {
	PSTRUCT_flt_reg = reg;
	PSTRUCT_int_reg
	  = (PSTRUCT_flt_reg - PR_first_reg(SIM_INFO.flt_args))
	  / PR_skip_value(SIM_INFO.flt_args)
	  * PR_skip_value(SIM_INFO.int_args)
	  + PR_first_reg(SIM_INFO.int_args);
      }

    } else {

      PSTRUCT_int_reg += PR_skip_value(SIM_INFO.int_args);
      PSTRUCT_flt_reg += PR_skip_value(SIM_INFO.flt_args);
      if (!PSTRUCT_no_flt_regs &&
	  Type_Has_Double_At_Offset(PSTRUCT_ty, PSTRUCT_offset)) {
	PLOC_reg(next) = PSTRUCT_flt_reg;
	PLOC_vararg_reg(next) = PSTRUCT_int_reg;
	if (!IS_FLT_PREG(PSTRUCT_flt_reg))
	  PLOC_reg(next) = 0;
      } else {
	PLOC_reg(next) = PSTRUCT_int_reg;
	if (!IS_INT_PREG(PSTRUCT_int_reg))
	  PLOC_reg(next) = 0;
      }
    }

    PLOC_size(next) = ireg_size;
    PSTRUCT_offset += ireg_size;

    return next;
} // Get_Struct_Parameter_Location


/* Iterate over vararg non-fixed parameters */
static PLOC
Get_Vararg_Parameter_Location (PLOC prev)
{
  PLOC next;
  Current_Param_Num++;
  next.reg = Get_Current_Preg_Num (SIM_INFO.int_args);

  if (next.reg > PR_last_reg(SIM_INFO.int_args))
  {
    next.reg = 0;
    next.size = 0;
  }
  else
  {
    next.size = MTYPE_RegisterSize(SIM_INFO.int_type);
  }
  /* use Last_Param_Offset in case last fixed arg had padding */
  next.start_offset = Last_Param_Offset;
  Last_Param_Offset = next.start_offset + next.size;
  return next;
}

BOOL Is_Caller_Save_GP;  /* whether GP is caller-save */

INT Formal_Save_Area_Size = 64;
INT Stack_Offset_Adjustment = 0;

extern void 
Init_Targ_Sim (void)
{
	Is_Caller_Save_GP = SIM_caller_save_gp;
}

