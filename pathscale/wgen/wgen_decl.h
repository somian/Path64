/* 
 * Copyright 2003, 2004, 2005, 2006, 2007, 2008 PathScale Inc.  All Rights Reserved.
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

/* WFE == WHIRL Front End */
/* translate gnu decl trees to whirl */

#ifndef wfe_decl_INCLUDED
#define wfe_decl_INCLUDED

extern void Init_Deferred_Function_Stack(void);
extern void Init_Deferred_Decl_Init_Stack(void);

/* expand namespace scope declaration into symtab and whirl */
extern BOOL expanding_function_definition;
extern "C" void WGEN_Expand_Decl (gs_t decl, BOOL can_skip);

/* called for each initialized variable */
extern void WGEN_Initialize_Decl (gs_t decl);

#ifdef KEY
// For initialization of any variables  except globals.
// Called for each initialized variable.
extern void WGEN_Initialize_Nested_Decl (gs_t decl);

// Add a VAR_DECL typeinfo to be emitted
extern void gxx_emits_typeinfos (gs_t);

// Add VAR_DECLs and TYPE_DECLs that are to be expanded last.
extern void defer_decl (gs_t);

// Add struct fields whose type we want to expand last.
extern void defer_field (gs_t, FLD_HANDLE);

// Add type whose DST info we want to create last.
extern void defer_DST_type (gs_t, TY_IDX, TY_IDX);

// Add DSTs for the defered types.
extern void add_deferred_DST_types();

// Interface description in .cxx file
extern void template_substituted (gs_t, gs_t, gs_t);
extern gs_t get_substituted (gs_t, gs_t);
extern void push_mp_local_vars (gs_t);
extern gs_t pop_mp_local_vars (void);

// Initialize a vector.
extern void Traverse_Aggregate_Vector_Const (ST *, gs_t, BOOL, UINT);
// Handle initialization through a modify_expr.
extern void WGEN_Process_Initialization (gs_t);
// Return TRUE if STR is attribute ATTRIB.
extern BOOL is_attribute (const char * str, gs_t attrib);
#endif /* KEY */

/* generate a temp with extension 'name' having the initialization as specified
   by 'init' */
extern ST *WGEN_Generate_Temp_For_Initialized_Aggregate (gs_t init, const char *name);

/* handle __attribute__ ((alias)) */
extern BOOL WGEN_Assemble_Alias (gs_t decl, gs_t target);

/* handle __attribute__ ((constructor)) */
extern void WGEN_Assemble_Constructor (char *name);

/* handle __attribute__ ((destructor)) */
extern void WGEN_Assemble_Destructor (char *name);

/* call this routine when have a decl that doesn't have an initialization */
extern void WGEN_Decl (gs_t decl);

/* call this routine to determine the return address ST at specified level */
extern ST *WGEN_Get_Return_Address_ST (int level);

/* call this routine to save the SP for first alloca in a scope */
extern ST *WGEN_Alloca_0 (void);
#if 0
/* call this routine to assign ST for VLA as well as allocate space for it */
extern ST *WGEN_Alloca_ST (gs_t decl);
#endif
/* call this routine to deallocate STs for VLA */
extern void WGEN_Dealloca (ST *, vector<ST*> *);

/* call this routine to resolve conflicts between duplicate declarations */
extern void WGEN_Resolve_Duplicate_Decls (gs_t olddecl, gs_t newdecl);

/* call this routine to mark all the symbols in the weak decls list weak */
extern "C" void WGEN_Weak_Finish(void);

#ifdef KEY
/* call this routine to process leftover symbols with alias targets */
extern "C" void WGEN_Alias_Finish(void);
#endif

/* get the current function declaration.  This just comes from a static
 * global variable in the absence of nested function declarations.
 */

extern gs_t Current_Function_Decl(void);

#ifdef KEY
/* get the current function's entry wn.  This just comes from a static
 * global variable in the absence of nested function declarations.
 */
extern WN *Current_Entry_WN(void);

/* The initializer for the named return value object.  Expand this in place of
 * the DECL_INITIAL in the object's VAR_DECL.
 */
extern gs_t named_ret_obj_initializer;
#endif
/* KEY: ST to represent EXC_PTR_EXPR if C++ exceptions are disabled */
extern ST * Dummy_Exc_Ptr_Expr;
#endif

