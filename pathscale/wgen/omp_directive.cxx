/*
 * Copyright (C) 2007. PathScale, LLC.  All rights reserved.
 */
/*
 * Copyright (C) 2007. QLogic Corporation. All Rights Reserved.
 */
/*
 * Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
 */

#include "defs.h"
#include "glob.h"
#include "config.h"
#include "wn.h"
#include "wn_util.h"

#include "srcpos.h"

extern "C"{
#include "gspin-wgen-interface.h"
}
#include "wgen_expr.h"
#include "wgen_misc.h"
#include "wgen_dst.h"
#include "wgen_stmt.h"
#include "wgen_spin_symbol.h"
#include "omp_types.h"
#include "omp_directive.h"
#include "wgen_omp_directives.h"
#include "wgen_omp_check_stack.h"

#include <stdio.h>
#include "errors.h"
#include "const.h"

extern std::vector<WN *> doloop_side_effects;
 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0
// the following two functions only for debugging

void
print_tree (FILE *file, gs_t node)
{
  print_node_brief (file, "", node, 0);

  fprintf (file, "\n");
}


void
print_node_brief (FILE *file, const char *prefix, gs_t node, int indent)
{
  char class1;

  if (node == 0)
    return;

  class1 = gs_tree_code_class (gs_tree_code (node));

  /* Always print the slot this node is in, and its code, address and
     name if any.  */
  if (indent > 0)
    fprintf (file, " ");
  fprintf (file, "%s <%s ", prefix, tree_code_name[(int) gs_tree_code (node)]);
  fprintf (file, HOST_PTR_PRINTF, (char *) node);

  if (class1 == 'd')
    {
      if (gs_decl_name (node))
	fprintf (file, " %s", gs_identifier_pointer (gs_decl_name (node)));
    }
  else if (class1 == 't')
    {
      if (gs_type_name (node))
	{
	  if (gs_tree_code (gs_type_name (node)) == GS_IDENTIFIER_NODE)
	    fprintf (file, " %s", gs_identifier_pointer (gs_type_name (node)));
	  else if (gs_tree_code (gs_type_name (node)) == GS_TYPE_DECL
		   && gs_decl_name (gs_type_name (node)))
	    fprintf (file, " %s",
		     gs_identifier_pointer (gs_decl_name (gs_type_name (node))));
	}
    }
  if (gs_tree_code (node) == GS_IDENTIFIER_NODE)
    fprintf (file, " %s", gs_identifier_pointer (node));

  fprintf (file, ">");

  if (gs_tree_code (node) == GS_TREE_LIST)
  	{
	  print_node_brief (file, "purpose", gs_tree_purpose (node), indent + 4);
	  print_node_brief (file, "value", gs_tree_value (node), indent + 4);
	  print_node_brief (file, "chain", gs_tree_chain (node), indent + 4);
  	}
}

///////// parallel directive ////////

void
check_parallel_directive( struct parallel_clause_list * clause_list )
{
     struct parallel_clause_list *cl = NULL;
     int count_if = 0, count_num_threads = 0, count_default = 0;
     
	 for (cl = clause_list; cl != NULL; cl = cl->next)
       {
          if ( cl->type == p_if ) count_if++;
          if ( cl->type == p_num_threads ) count_num_threads++;
          if ( cl->type == p_default ) count_default++;
       }
	 
     if ( count_if > 1 || count_num_threads > 1 || count_default > 1)
       {
          if ( count_if > 1)
          	{
          	   printf ("Too many IF clausees.\n");
          	}
          if ( count_num_threads > 1)
          	{
          	   printf ("Too many NUM_THREADS clausees.\n");
          	}
          if ( count_default > 1)
          	{
          	   printf ("Too many DEFAULT clausees.\n");
          	}
          
          SRCPOS srcpos = Get_Srcpos();
          Fail_FmtAssertion ("Invalid syntax in the #PRAGMA OMP PARALLEL directive at line: %d, file number: %d.!\n", 
                      SRCPOS_linenum(srcpos), SRCPOS_filenum(srcpos));
       }
}
#endif

// c_split_parallel_clauses() in GNU splits combined parallel clauses. We
// combine them back here, and we remove the NOWAIT clause.
void
expand_start_parallel_or_combined_parallel (gs_t stmt)
{
  gs_t stmts = gs_omp_parallel_body(stmt);

  // bug 13686: Disable the following for C++, i.e., split a combined-parallel
  // construct into two for C++. c_split_parallel_clauses in GNU has been
  // fixed for lastprivates. This splitting is useful for C++ due to the need
  // to insert constructors/destructors outside the DO region, and inside
  // an enclosing parallel region.
  if (!lang_cplus &&
      gs_omp_parallel_combined(stmt) && // Combined by GNU?
      // Make sure it is in the expected format for us to be able to combine.
      gs_tree_code(stmts) == GS_BIND_EXPR)
  {
    gs_t body = gs_bind_expr_body(stmts);

    if (gs_tree_code(body) == GS_OMP_FOR)
    {
      expand_start_parallel_for (stmt);
      return;
    }
    else if (gs_tree_code(body) == GS_OMP_SECTIONS)
    {
      expand_start_parallel_sections (stmt);
      return;
    }
  }

  expand_start_parallel (stmt);
}

void
expand_start_parallel (gs_t stmt)
{
     WGEN_expand_start_parallel (stmt);
     WGEN_Expand_Stmt (gs_omp_parallel_body(stmt));
#if 0
     /* Do we need this check any more? */
     WN *wn = WGEN_Stmt_Top ();
     WGEN_check_parallel (wn);
#endif
     expand_end_parallel ();

}

void
expand_end_parallel ()
{
     WGEN_expand_end_parallel ();
}


#if 0
///////// for directive ////////

void
check_for_directive(struct for_clause_list * clause_list )
{
     struct for_clause_list *cl = NULL;
     int count_schedule = 0, count_ordered = 0, count_nowait = 0;
     
	 for (cl = clause_list; cl != NULL; cl = cl->next)
       {
          if ( cl->type == f_schedule_1 || cl->type == f_schedule_2 ) count_schedule++;
          if ( cl->type == f_ordered ) count_ordered++;
          if ( cl->type == f_nowait ) count_nowait++;
       }
	 
     if ( count_schedule > 1 || count_ordered > 1 || count_nowait > 1)
       {
          if ( count_schedule > 1)
          	{
          	   printf ("Too many SCHEDULE clausees.\n");
          	}
          if ( count_ordered > 1)
          	{
          	   printf ("Too many ORDERED clausees.\n");
          	}
          if ( count_nowait > 1)
          	{
          	   printf ("Too many NOWAIT clausees.\n");
          	}

          
          SRCPOS srcpos = Get_Srcpos();
          Fail_FmtAssertion ("Invalid syntax in the #PRAGMA OMP FOR directive at line: %d, file number: %d.!\n", 
                      SRCPOS_linenum(srcpos), SRCPOS_filenum(srcpos));
       }
}

#endif

extern void WGEN_Expand_DO (gs_t);

void
expand_start_for (gs_t stmt)
{
     WGEN_expand_start_for (stmt);
     WGEN_Expand_DO (stmt);
     expand_end_for ();
}

void
expand_end_for (void)
{
     WGEN_expand_end_for ();
}


#if 0
///////// sections directive ////////


void
check_sections_directive( struct sections_clause_list * clause_list )
{
     struct sections_clause_list *cl = NULL;
     int count_nowait = 0;
     
	 for (cl = clause_list; cl != NULL; cl = cl->next)
       {
          if ( cl->type == sections_nowait ) count_nowait++;
       }
	 
     if ( count_nowait > 1)
       {
          printf ("Too many NOWAIT clausees.\n");
          
          SRCPOS srcpos = Get_Srcpos();
          Fail_FmtAssertion ("Invalid syntax in the #PRAGMA OMP SECTIONS directive at line: %d, file number: %d.!\n", 
                      SRCPOS_linenum(srcpos), SRCPOS_filenum(srcpos));
       }
}

#endif

void
expand_start_sections (gs_t stmt)
{
  WGEN_expand_start_sections (stmt);
  WGEN_Expand_Stmt (gs_omp_sections_body(stmt));
  expand_end_sections ();
}

void
expand_end_sections (void)
{
  WGEN_expand_end_sections ();
}


///////// section directive ////////


void expand_start_section (gs_t stmt)
{
  Is_True (gs_tree_code(stmt) == GS_OMP_SECTION,
           ("expand_start_section: invalid statement type"));
  WGEN_expand_start_section ();
  WGEN_Expand_Stmt (gs_omp_section_body(stmt));
  expand_end_section ();
}

void expand_end_section (void)
{
  WGEN_expand_end_section ();
}

///////// single directive ////////


#if 0
void
check_single_directive( struct single_clause_list * clause_list )
{
     struct single_clause_list *cl = NULL;
     int count_copyprivate = 0, count_nowait = 0;
     
	 for (cl = clause_list; cl != NULL; cl = cl->next)
       {
          if ( cl->type == single_copyprivate ) count_copyprivate++;
          if ( cl->type == single_nowait ) count_nowait++;
       }
	 
     if ( count_nowait > 1 || (count_copyprivate != 0 && count_nowait != 0) )
       {
          if ( count_copyprivate != 0 && count_nowait != 0 )
          	{
          	   printf ("The copyprivate clause must not be used with the nowait clause.\n");
          	}
          if ( count_nowait > 1)
          	{
          	   printf ("Too many NOWAIT clausees.\n");
          	}

          SRCPOS srcpos = Get_Srcpos();
          Fail_FmtAssertion ("Invalid syntax in the #PRAGMA OMP SINGLE directive at line: %d, file number: %d.!\n", 
                      SRCPOS_linenum(srcpos), SRCPOS_filenum(srcpos));
       }
}

#endif

void
expand_start_single (gs_t stmt)
{
  WGEN_expand_start_single (stmt);
  WGEN_Expand_Stmt (gs_omp_single_body(stmt));
  expand_end_single ();
}

void expand_end_single( )
{
     WGEN_expand_end_single ();
}

#if 0

///////// parallel for directive ////////


void
check_parallel_for_directive( struct parallel_for_clause_list * clause_list )
{
     struct parallel_for_clause_list *cl = NULL;
     int count_if = 0, count_num_threads = 0, count_default = 0;
     int count_schedule = 0, count_ordered = 0;
     
	 for (cl = clause_list; cl != NULL; cl = cl->next)
       {
          if ( cl->type == p_for_if ) count_if++;
          if ( cl->type == p_for_num_threads ) count_num_threads++;
          if ( cl->type == p_for_default ) count_default++;
          if ( cl->type == p_for_schedule_1 || cl->type == p_for_schedule_2 ) count_schedule++;
          if ( cl->type == p_for_ordered ) count_ordered++;
       }
	 
     if ( count_if > 1 || count_num_threads > 1 || count_default >1
     	 ||count_schedule > 1 || count_ordered > 1 )
       {
          if ( count_if > 1)
          	{
          	   printf ("Too many IF clausees.\n");
          	}
          if ( count_num_threads > 1)
          	{
          	   printf ("Too many NUM_THREADS clausees.\n");
          	}
          if ( count_default > 1)
          	{
          	   printf ("Too many DEFAULT clausees.\n");
          	}
          if ( count_schedule > 1)
          	{
          	   printf ("Too many SCHEDULE clausees.\n");
          	}
          if ( count_ordered > 1)
          	{
          	   printf ("Too many ORDERED clausees.\n");
          	}
          
          SRCPOS srcpos = Get_Srcpos();
          Fail_FmtAssertion ("Invalid syntax in the #PRAGMA OMP PARALLEL FOR directive at line: %d, file number: %d.!\n", 
                      SRCPOS_linenum(srcpos), SRCPOS_filenum(srcpos));
       }

}

#endif


void
expand_start_parallel_for (gs_t stmt)
{
     WGEN_expand_start_parallel_for (stmt);
     WGEN_Expand_DO (gs_bind_expr_body(gs_omp_parallel_body(stmt)));
     expand_end_parallel_for ();
}

void
expand_end_parallel_for (void)
{
     WGEN_expand_end_parallel_for ();
}


#if 0
///////// parallel sections directive ////////

void
check_parallel_sections_directive( struct parallel_sections_clause_list * clause_list )
{
     struct parallel_sections_clause_list *cl = NULL;
     int count_if = 0, count_num_threads = 0, count_default = 0;
     
	 for (cl = clause_list; cl != NULL; cl = cl->next)
       {
          if ( cl->type == p_sections_if ) count_if++;
          if ( cl->type == p_sections_num_threads ) count_num_threads++;
          if ( cl->type == p_sections_default ) count_default++;
       }
	 
     if ( count_if > 1 || count_num_threads > 1 || count_default > 1 )
       {
          if ( count_if > 1)
          	{
          	   printf ("Too many IF clausees.\n");
          	}
          if ( count_num_threads > 1)
          	{
          	   printf ("Too many NUM_THREADS clausees.\n");
          	}
          if ( count_default > 1)
          	{
          	   printf ("Too many DEFAULT clausees.\n");
          	}
          
          SRCPOS srcpos = Get_Srcpos();
          Fail_FmtAssertion ("Invalid syntax in the #PRAGMA OMP PARALLEL SECTIONS directive at line: %d, file number: %d.!\n", 
                      SRCPOS_linenum(srcpos), SRCPOS_filenum(srcpos));
       }

}

#endif

void
expand_start_parallel_sections(gs_t stmt)
{
  WGEN_expand_start_parallel_sections (stmt);
  WGEN_Expand_Stmt (gs_omp_sections_body(
                     gs_bind_expr_body(
                      gs_omp_parallel_body(stmt))));
  expand_end_parallel_sections ();
}

void
expand_end_parallel_sections(void)
{
    WGEN_expand_end_parallel_sections ();
}

///////// master directive ////////


void expand_start_master (gs_t stmt)
{
     WGEN_expand_start_master ();
     WGEN_Expand_Stmt (gs_omp_master_body(stmt));
     expand_end_master ();
}

void expand_end_master (void)
{
     WGEN_expand_end_master ();
}

///////// critical directive ////////


void expand_start_critical( gs_t stmt )
{
    char *critical_name = NULL;
    ST *st = NULL;        
    TCON           tcon;
    TY_IDX ty;

    gs_t region_phrase = gs_omp_critical_name(stmt);
    if (region_phrase)
    {
      critical_name = gs_identifier_pointer (region_phrase);
      tcon = Host_To_Targ_String (MTYPE_STRING, critical_name,
                                  strlen(critical_name));
      st = Gen_String_Sym (&tcon, MTYPE_To_TY(MTYPE_STRING), FALSE);
    }
    if (Trace_Omp)
      printf("critical name is %s \n",critical_name);
    WGEN_expand_start_critical (st, critical_name);
    WGEN_Expand_Stmt (gs_omp_critical_body(stmt));
    expand_end_critical ();
}
 
void expand_end_critical (void)
{
      WGEN_expand_end_critical ( );
}


#if 0
///////// atomaic directive ////////


void check_atomic_expression ( gs_t atomic_expression )
{
    bool valid_for_expr = true;
    enum gs_code_t code, code1;
    
    code = gs_tree_code (atomic_expression);

    if (code != GS_MODIFY_EXPR && code != GS_PREDECREMENT_EXPR &&
        code != GS_PREINCREMENT_EXPR && code != GS_POSTDECREMENT_EXPR &&
	code != GS_POSTINCREMENT_EXPR)
    {
	printf (" No such increment operation is permitted.\n");
	valid_for_expr = false;		
    }
    else {
        code1 = gs_tree_code (gs_tree_operand (atomic_expression, 1));
	if (code1 == GS_NOP_EXPR)
	  code1 = gs_tree_code (gs_tree_operand (gs_tree_operand (atomic_expression, 1), 0));
        if (code == GS_MODIFY_EXPR && code1 != GS_PLUS_EXPR &&
	    code1 != GS_MINUS_EXPR && code1 != GS_MULT_EXPR &&
	    code1 != GS_RDIV_EXPR && code1 != GS_BIT_AND_EXPR &&
	    code1 != GS_BIT_XOR_EXPR && code1 != GS_BIT_IOR_EXPR &&
	    code1 != GS_LSHIFT_EXPR && code1 != GS_RSHIFT_EXPR &&
	    code1 != GS_TRUNC_DIV_EXPR)
           {
               printf (" No such modifying operation is supported.\n");
               valid_for_expr = false;		
           }
    }

   if ( !valid_for_expr) 
   	{
   	  SRCPOS srcpos = Get_Srcpos();
      Fail_FmtAssertion ("Invalid atomic_expression in an ATOMIC directive!  at line: %d, file number: %d.!\n", 
                      SRCPOS_linenum(srcpos), SRCPOS_filenum(srcpos));
   	}  
}

#endif

void  expand_start_atomic (gs_t stmt)
{
    WGEN_expand_start_atomic ();
    WN * rhs_wn = WGEN_Expand_Expr (gs_tree_operand(stmt,1));
    gs_t lhs = gs_tree_operand(stmt,0);
    WGEN_Lhs_Of_Modify_Expr (GS_MODIFY_EXPR, lhs,
                             0, 0, 0, 0, 0, rhs_wn, 0, 0, 0);
    expand_end_atomic ();
}

void  expand_end_atomic (void)
{
    WGEN_expand_end_atomic ();
}

///////// ordered directive ////////


void expand_start_ordered(gs_t stmt)
{
    WGEN_expand_start_ordered ();
    WGEN_Expand_Stmt (gs_omp_ordered_body(stmt));
    expand_end_ordered ();
} 

void expand_end_ordered (void)
{
    WGEN_expand_end_ordered ();
}

#if 0

///////// barrier directive ////////


void  expand_barrier ()         
{
     WGEN_expand_barrier ();
}


///////// flush directive ////////


void  expand_flush ( gs_t flush_variables)         
{
    register gs_t t = NULL;
    WN_list *flush_wn_list = NULL;
    WN_list *wnlist = NULL;

    for (t = flush_variables; t; t = gs_tree_chain (t))
    { 
       WN *wn = WGEN_Expand_Expr(gs_tree_value(t));
       wnlist = (struct WN_list *) malloc(sizeof(struct WN_list));
       wnlist->wn = WN_CopyNode (wn) ;
       wnlist->next = flush_wn_list;
       flush_wn_list = wnlist;
    }

    WGEN_expand_flush (flush_wn_list);
    
}


///////// do loop expander ////////

int  check_do_loop_for(gs_t init_expr, gs_t logical_expr, gs_t incr_expr)
{

    bool valid_for_expr = true;
    enum gs_code_t code, code1;
    
    WN *index, *start, *end, *step;
    
	WN *wn_tmp,  * lcv , * incv;

	TYPE_ID lcv_t;

    code = gs_tree_code (init_expr);

    if (code != MODIFY_EXPR) 
    	{
	      valid_for_expr = false;		
    	}
    else 
    	{
           lcv = WGEN_Expand_Expr (gs_tree_operand (init_expr, 0));
           lcv_t = TY_mtype(ST_type(WN_st(lcv)));

           if (lcv_t != MTYPE_I4 && lcv_t != MTYPE_I8 && lcv_t != MTYPE_I2
           	          && lcv_t != MTYPE_I1)
           	{         
   	             valid_for_expr = false;		
           	}
        }
    
    code = gs_tree_code (logical_expr);

    if (valid_for_expr && (code != LT_EXPR) && (code != LE_EXPR) && (code != GT_EXPR) && (code != GE_EXPR)) 
    	{
   	       valid_for_expr = false;		
    	}
    
    
    code = gs_tree_code (incr_expr);

    if (valid_for_expr && (code != MODIFY_EXPR) && (code != PREDECREMENT_EXPR) && (code != PREINCREMENT_EXPR) 
           && (code != POSTDECREMENT_EXPR) && (code != POSTINCREMENT_EXPR)) 
    	{
   	       valid_for_expr = false;		
    	}
    else {
           incv = WGEN_Expand_Expr (gs_tree_operand (incr_expr, 0));
           if (WN_st_idx(lcv)!=WN_st_idx(incv))
           	{	     
   	           valid_for_expr = false;		
           	}
           else 
           	{
               code1 = gs_tree_code (gs_tree_operand (incr_expr, 1));
               if ((code == MODIFY_EXPR) && (code1 != PLUS_EXPR) && (code1 != MINUS_EXPR))
               	{
   	                valid_for_expr = false;		
               	}
             
           	}
    }

   if (valid_for_expr) 
   	{
         return true;
   	}
   else
   	{
   	     return false;
   	}  


}
#endif

extern OPERATOR Operator_From_Tree_node (gs_code_t);
extern bool Has_Subsumed_Cvtl (OPERATOR);

void expand_start_do_loop (gs_t init_expr, gs_t logical_expr, gs_t incr_expr)
{
    bool valid_for_expr = true;
    gs_code_t code, code1;
    gs_t temp; 
    
    WN *index, *start, *end, *step;
    
    WN *wn_tmp,  * lcv , * incv;
    WN *wn,*wn1;
    WN *tdecl;
    ST *tst;
    ST_IDX stlcv;

    TYPE_ID lcv_t;

    FmtAssert (doloop_side_effects.empty(),
               ("Expected DO-loop side-effects to be empty."));
    code = gs_tree_code (init_expr);

    if (code != GS_MODIFY_EXPR && code != GS_VAR_DECL) 
    {
      printf ("Invalid init_expr in a FOR statement! \n");
      valid_for_expr = false;
      WGEN_Stmt_Push (WN_CreateBlock (), wgen_stmk_comma, Get_Srcpos());
    }
    else 
    {
      if(code == GS_VAR_DECL)
      {
        tst = DECL_ST(init_expr);
        index = WN_CreateIdname(0,tst);
        WGEN_Stmt_Push (WN_CreateBlock (), wgen_stmk_comma, Get_Srcpos());
              
        wn1 = WGEN_Expand_Expr (gs_decl_initial(init_expr)); // r.h.s.
        wn_tmp = WGEN_Lhs_Of_Modify_Expr(GS_MODIFY_EXPR, init_expr, FALSE, 
                                         0, 0, 0, FALSE, wn1, 0, FALSE, FALSE);
        wn_tmp = WGEN_Stmt_Pop (wgen_stmk_comma);
        start = WN_COPY_Tree( WN_first( wn_tmp ));
        WN_DELETE_Tree( wn_tmp );
      }
      else
      {
        lcv = WGEN_Expand_Expr (gs_tree_operand (init_expr, 0));
        lcv_t = TY_mtype(ST_type(WN_st(lcv)));

        if (lcv_t != MTYPE_I4 && lcv_t != MTYPE_I8 &&
            lcv_t != MTYPE_I2 && lcv_t != MTYPE_I1)
        {
          printf ("Invalid induction variable type in init_expr in a FOR statement! \n");
          valid_for_expr = false;		
        }
        else
        {
          index = WN_CreateIdname(0,WN_st_idx(lcv));
          if (lang_cplus)
          { // Handle any side-effects in loop init.
            WN * rhs = WGEN_Expand_Expr(gs_tree_operand(init_expr, 1));
            if (WN_has_side_effects(rhs))
            {
              TY_IDX ty_idx = MTYPE_TO_TY_array[WN_rtype(rhs)];
              ST * st = Gen_Temp_Symbol (ty_idx, "__doloop_init");
              // Mark as local in surrounding parallel region.
              WGEN_add_pragma_to_enclosing_regions (WN_PRAGMA_LOCAL, st, TRUE);
              WN * stid = WN_Stid (TY_mtype(ty_idx), 0, st, ty_idx, rhs);
              // Hold it in a vector for emitting it outside and before the
              // PDO region.
              doloop_side_effects.push_back (stid);
              rhs = WN_Ldid (TY_mtype(ty_idx), 0, st, ty_idx);
            }
            WGEN_Stmt_Push (WN_CreateBlock (), wgen_stmk_comma, Get_Srcpos());
            WGEN_Lhs_Of_Modify_Expr(code, gs_tree_operand (init_expr, 0), FALSE,
                                    0, 0, 0, FALSE, rhs, 0, FALSE, FALSE);
            wn_tmp = WGEN_Stmt_Pop (wgen_stmk_comma);
            // loop init
            start = WN_COPY_Tree (WN_first (wn_tmp));
            WN_DELETE_Tree (wn_tmp);
          }
          else
          {
            WGEN_Stmt_Push (WN_CreateBlock (), wgen_stmk_comma, Get_Srcpos());
            wn_tmp = WGEN_Expand_Expr (init_expr); 
            wn_tmp = WGEN_Stmt_Pop (wgen_stmk_comma);
            start = WN_COPY_Tree( WN_first( wn_tmp ));
            WN_DELETE_Tree( wn_tmp );
          }
        }
      }
    }
    code = gs_tree_code (logical_expr);
    if (valid_for_expr && (code != GS_LT_EXPR) && (code != GS_LE_EXPR) && (code != GS_GT_EXPR) && (code != GS_GE_EXPR)) 
    {
      printf ("Invalid logical_expr in a FOR statement! \
	   	      The logical operators can only be <=, <, > or >= \n");
      valid_for_expr = false;		
    }
    else
    {
      if (lang_cplus)
      { // Handle any side-effects in loop upper bound.
        // index variable
        WN * var = WGEN_Expand_Expr(gs_tree_operand(logical_expr, 0));
        // upper bound
        WN * ub = WGEN_Expand_Expr(gs_tree_operand(logical_expr, 1));

        // Mimick WGEN_Expand_Expr for type conversion.
        TY_IDX ty_idx = Get_TY (gs_tree_type(logical_expr));
        TYPE_ID mtyp = TY_mtype(ty_idx);
        TY_IDX ty_idx0 = Get_TY(gs_tree_type(gs_tree_operand(logical_expr, 0)));
        TYPE_ID mtyp0 = TY_mtype(ty_idx0);
        TY_IDX ty_idx1 = Get_TY(gs_tree_type(gs_tree_operand(logical_expr, 1)));
        TYPE_ID mtyp1 = TY_mtype(ty_idx1);

        if (MTYPE_size_min(mtyp1) > MTYPE_size_min(mtyp0) &&
            ! Has_Subsumed_Cvtl(WN_operator(var)))
          var = WN_CreateCvtl(OPR_CVTL, Widen_Mtype(mtyp0), MTYPE_V,
                              MTYPE_size_min(mtyp0), var);
        if (MTYPE_size_min(mtyp0) > MTYPE_size_min(mtyp1) &&
            ! Has_Subsumed_Cvtl(WN_operator(ub)))
          ub = WN_CreateCvtl(OPR_CVTL, Widen_Mtype(mtyp1), MTYPE_V,
                              MTYPE_size_min(mtyp1), ub);

        if (WN_has_side_effects(ub))
        {
          TY_IDX ty_idx = MTYPE_TO_TY_array[WN_rtype(ub)];
          ST * st = Gen_Temp_Symbol (ty_idx, "__doloop_ub");
          // Mark as local in any enclosing parallel region.
          WGEN_add_pragma_to_enclosing_regions (WN_PRAGMA_LOCAL, st, TRUE);
          WN * stid = WN_Stid (TY_mtype(ty_idx), 0, st, ty_idx, ub);
          // Hold it in a vector for emitting it outside and before the
          // PDO region.
          doloop_side_effects.push_back (stid);
          ub = WN_Ldid (TY_mtype(ty_idx), 0, st, ty_idx);
        }
        end  = WN_CreateExp2(Operator_From_Tree_node(code),
                             Widen_Mtype(mtyp), Widen_Mtype(mtyp0),
                             var, ub);
      }
      else
        end = WGEN_Expand_Expr (logical_expr); 
    }
    
    code = gs_tree_code (incr_expr);

    if (valid_for_expr && (code != GS_MODIFY_EXPR) && (code != GS_PREDECREMENT_EXPR) && (code != GS_PREINCREMENT_EXPR) 
           && (code != GS_POSTDECREMENT_EXPR) && (code != GS_POSTINCREMENT_EXPR)) 
    {
      printf ("Invalid incr_expr in a FOR statement! \n");
      valid_for_expr = false;		
    }
    else
    {
      incv = WGEN_Expand_Expr (gs_tree_operand (incr_expr, 0));
      if (gs_tree_code(init_expr) == GS_VAR_DECL)
        stlcv = ST_st_idx(tst);
      else
        stlcv = WN_st_idx(lcv);
      if (stlcv!=WN_st_idx(incv))
      {
        printf ("Invalid incr_expr in a FOR statement!  \
		       	      No induction variable to be modified.\n");
        valid_for_expr = false;		
      }
      else 
      {
        code1 = gs_tree_code (gs_tree_operand (incr_expr, 1));
        if ((code == GS_MODIFY_EXPR) && (code1 != GS_PLUS_EXPR) && (code1 != GS_MINUS_EXPR))
        {
          printf ("Invalid incr_expr in a FOR statement!  \
                   No such increment operation is permitted.\n");
          valid_for_expr = false;		
        }
        else
       	{
          WGEN_Stmt_Push (WN_CreateBlock (), wgen_stmk_comma, Get_Srcpos());
          WGEN_Expand_Expr (incr_expr, FALSE); 
          wn_tmp = WGEN_Stmt_Pop (wgen_stmk_comma);
          step = WN_COPY_Tree( WN_first( wn_tmp ));
          WN_DELETE_Tree(  wn_tmp );
          // TODO: side-effects in increment amount not handled properly.
       	}
      }
    }
    if (valid_for_expr) 
    {
      WGEN_expand_start_do_loop (index, start, end, step);
    }
    else
    {
      SRCPOS srcpos = Get_Srcpos();
      Fail_FmtAssertion ("Invalid syntax in the FOR statement at line: %d, file number: %d.!\n", 
                      SRCPOS_linenum(srcpos), SRCPOS_filenum(srcpos));
    }
}

void expand_end_do_loop (void)
{
    WGEN_expand_end_do_loop();
}
