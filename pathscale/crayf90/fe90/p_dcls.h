/*
 * Copyright (C) 2008. PathScale, LLC. All Rights Reserved.
 */
/*
 *  Copyright (C) 2006. QLogic Corporation. All Rights Reserved.
 */

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



/* USMID:  "\n@(#)5.0_pl/headers/p_dcls.h	5.1	04/29/99 21:22:31\n" */

enum		attr_values	{Allocatable_Attr,
				 Automatic_Attr,
#ifdef KEY /* Bug 14150 */
				 Bind_Attr,
#endif /* KEY Bug 14150 */
				 Co_Array_Attr,
				 Dimension_Attr,
				 External_Attr,
				 Intent_Attr,
				 Intrinsic_Attr,
				 Optional_Attr,
				 Parameter_Attr,
				 Pointer_Attr,
				 Private_Attr,
#ifdef KEY /* Bug 5091 */
				 Protected_Attr,
#endif /* KEY Bug 5091 */
				 Public_Attr,
				 Save_Attr,
				 Target_Attr,
#ifdef KEY /* Bug 14150 */
				 Value_Attr,
#endif /* KEY Bug 14150 */
				 Volatile_Attr,
				 End_Attr = Volatile_Attr };

typedef	enum	 attr_values	attr_type;

static	char	*attr_str[End_Attr+1] =		{"ALLOCATABLE",
						 "AUTOMATIC",
#ifdef KEY /* Bug 14150 */
						 "BIND",
#endif /* KEY Bug 14150 */
						 "CO-ARRAY",
						 "DIMENSION",
						 "EXTERNAL",
						 "INTENT",
						 "INTRINSIC",
						 "OPTIONAL",
						 "PARAMETER",
						 "POINTER",
						 "PRIVATE",
#ifdef KEY /* Bug 5091 */
						 "PROTECTED",
#endif /* KEY Bug 5091 */
						 "PUBLIC",
						 "SAVE",
						 "TARGET",
						 "VALUE",
						 "VOLATILE" };

static	long	 err_attrs[End_Attr+1] =       {

		 /* Allocatable_Attr */		((1 << Allocatable_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Bind_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << Automatic_Attr) |
						 (1 << External_Attr) |
#ifndef KEY /* Bug 6845 */
		 /* Allocatable now allowed on dummy args */
						 (1 << Intent_Attr) |
#endif /* KEY Bug 6845 */
						 (1 << Intrinsic_Attr) |
#ifndef KEY /* Bug 6845 */
		 /* Allocatable now allowed on dummy args */
						 (1 << Optional_Attr) |
#endif /* KEY Bug 6845 */
						 (1 << Parameter_Attr) |
						 (1 << Pointer_Attr)),

		 /* Automatic_Attr */		((1 << Allocatable_Attr) |
						 (1 << Automatic_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Bind_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << Co_Array_Attr) |
						 (1 << External_Attr) |
						 (1 << Intent_Attr) |
						 (1 << Intrinsic_Attr) |
						 (1 << Optional_Attr) |
						 (1 << Parameter_Attr) |
						 (1 << Save_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Value_Attr)
#endif /* KEY Bug 14150 */
						 ),

#ifdef KEY /* Bug 14150 */
		 /* Bind_Attr */		(
		 			         (1 << Allocatable_Attr) |
						 (1 << Automatic_Attr) |
						 (1 << Bind_Attr) |
						 (0 << Co_Array_Attr) |
						 (0 << Dimension_Attr) |
						 (0 << External_Attr) |
						 (1 << Intent_Attr) |
						 (1 << Intrinsic_Attr) |
						 (1 << Optional_Attr) |
						 (1 << Parameter_Attr) |
						 (1 << Pointer_Attr) |
						 (0 << Private_Attr) |
						 (0 << Public_Attr) |
						 (0 << Save_Attr) |
						 (0 << Target_Attr) |
						 (0 << Volatile_Attr) |
						 (1 << Value_Attr)
						 ),
#endif /* KEY Bug 14150 */

		 /* Co_Array_Attr */		((1 << Automatic_Attr) |
						 (1 << Co_Array_Attr) |
						 (1 << External_Attr) |
						 (1 << Intrinsic_Attr) |
						 (1 << Optional_Attr) |
						 (1 << Parameter_Attr) |
						 (1 << Pointer_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Value_Attr)
#endif /* KEY Bug 14150 */
						 ),

		 /* Dimension_Attr */		((1 << Dimension_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Value_Attr)
#endif /* KEY Bug 14150 */
						 ),

		 /* External_Attr */		((1 << Allocatable_Attr) |
						 (1 << Automatic_Attr) |
						 (1 << Co_Array_Attr) |
						 (1 << External_Attr) |
						 (1 << Intent_Attr) |
						 (1 << Intrinsic_Attr) |
						 (1 << Parameter_Attr) |
						 (1 << Pointer_Attr) |
#ifdef KEY /* Bug 5091 */
						 (1 << Protected_Attr) |
#endif /* KEY Bug 5091 */
						 (1 << Save_Attr) |
						 (1 << Target_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Value_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << Volatile_Attr)),

		 /* Intent_Attr */		(
#ifndef KEY /* Bug 6845 */
		 /* Allocatable now allowed on dummy args */
		                                 (1 << Allocatable_Attr) |
#endif /* KEY Bug 6845 */
						 (1 << Automatic_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Bind_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << External_Attr) |
						 (1 << Intent_Attr) |
						 (1 << Intrinsic_Attr) |
						 (1 << Parameter_Attr) |
#ifndef KEY /* Bug 14150 */
						 /* F2003 allows this */
						 (1 << Pointer_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << Save_Attr)),

		 /* Intrinsic_Attr */		((1 << Allocatable_Attr) |
						 (1 << Automatic_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Bind_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << Co_Array_Attr) |
						 (1 << External_Attr) |
						 (1 << Intent_Attr) |
						 (1 << Intrinsic_Attr) |
						 (1 << Optional_Attr) |
						 (1 << Parameter_Attr) |
						 (1 << Pointer_Attr) |
#ifdef KEY /* Bug 5091 */
						 (1 << Protected_Attr) |
#endif /* KEY Bug 5091 */
						 (1 << Save_Attr) |
						 (1 << Target_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Value_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << Volatile_Attr)),

		 /* Optional_Attr */		(
#ifndef KEY /* Bug 6845 */
		 /* Allocatable now allowed on dummy args */
		                                 (1 << Allocatable_Attr) |
#endif /* KEY Bug 6845 */
						 (1 << Automatic_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Bind_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << Intrinsic_Attr) |
						 (1 << Optional_Attr) |
						 (1 << Parameter_Attr) |
						 (1 << Save_Attr)),

		 /* Parameter_Attr */		((1 << Allocatable_Attr) |
						 (1 << Automatic_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Bind_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << Co_Array_Attr) |
						 (1 << External_Attr) |
						 (1 << Intent_Attr) |
						 (1 << Intrinsic_Attr) |
						 (1 << Optional_Attr) |
						 (1 << Parameter_Attr) |
						 (1 << Pointer_Attr) |
#ifdef KEY /* Bug 5091 */
						 (1 << Protected_Attr) |
#endif /* KEY Bug 5091 */
						 (1 << Save_Attr) |
						 (1 << Target_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Value_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << Volatile_Attr)),

		 /* Pointer_Attr */		((1 << Allocatable_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Bind_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << Co_Array_Attr) |
						 (1 << External_Attr) |
#ifndef KEY /* Bug 14150 */
						 /* F2003 allows this */
						 (1 << Intent_Attr) |
#endif /* KEY Bug 14150 */
						 (1 << Intrinsic_Attr) |
						 (1 << Parameter_Attr) |
						 (1 << Pointer_Attr) |
						 (1 << Target_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Value_Attr)
#endif /* KEY Bug 14150 */
						 ),

		 /* Private_Attr */		((1 << Private_Attr) |
						 (1 << Automatic_Attr) |
						 (1 << Public_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Value_Attr)
#endif /* KEY Bug 14150 */
						 ),

#ifdef KEY /* Bug 5091 */
		 /* Protected_Attr */		((1 << Automatic_Attr) |
						 (1 << External_Attr) |
						 (1 << Intent_Attr) |
						 (1 << Intrinsic_Attr) |
						 (1 << Optional_Attr) |
						 (1 << Parameter_Attr) |
						 (1 << Value_Attr)
						 ),
#endif /* KEY Bug 5091 */

		 /* Public_Attr */		((1 << Private_Attr) |
						 (1 << Automatic_Attr) |
						 (1 << Public_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Value_Attr)
#endif /* KEY Bug 14150 */
						 ),

		 /* Save_Attr */		((1 << External_Attr) |
						 (1 << Automatic_Attr) |
						 (1 << Intent_Attr) |
						 (1 << Intrinsic_Attr) |
						 (1 << Parameter_Attr) |
						 (1 << Optional_Attr) |
						 (1 << Save_Attr) |
#ifdef KEY /* Bug 14150 */
						 (1 << Value_Attr)
#endif /* KEY Bug 14150 */
						 ),

		 /* Target_Attr */		((1 << External_Attr) |
						 (1 << Intrinsic_Attr) |
						 (1 << Parameter_Attr) |
						 (1 << Pointer_Attr) |
						 (1 << Target_Attr)),

#ifdef KEY /* Bug 14150 */
		 /* Value_Attr */		((1 << Allocatable_Attr) |
						 (1 << Automatic_Attr) |
						 (1 << Bind_Attr) |
						 (1 << Co_Array_Attr) |
						 (1 << Dimension_Attr) |
						 (1 << External_Attr) |
						 (0 << Intent_Attr) |
						 (1 << Intrinsic_Attr) |
						 (0 << Optional_Attr) |
						 (1 << Parameter_Attr) |
						 (0 << Private_Attr) |
						 (0 << Public_Attr) |
						 (1 << Pointer_Attr) |
						 (1 << Save_Attr) |
						 (0 << Target_Attr) |
						 (1 << Value_Attr) |
						 (1 << Volatile_Attr)),
#endif /* KEY Bug 14150 */

		 /* Volatile_Attr */		((1 << External_Attr) |
						 (1 << Intrinsic_Attr) |
						 (1 << Parameter_Attr))

					       };
