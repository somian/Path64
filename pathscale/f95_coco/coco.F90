! bof
! **********************************************************************
! Fortran 95 program coco

! **********************************************************************
! Source Control Strings

! $Id: coco.f90 1.28 2003/10/22 13:08:20Z Dan Release $

! 
!  Copyright (C) 2007. PathScale, LLC. All Rights Reserved.
!
!  Copyright (C) 2006. PathScale, Inc. All Rights Reserved.
!  File modified by PathScale, Inc. to integrate it with the mfef95
!  Fortran front end.
!

! **********************************************************************
!  Copyright 2003 Purple Sage Computing Solutions, Inc.
!  All Rights Reserved

!   This program is free software; you can redistribute it and/or
!   modify it under the terms of the GNU General Public
!   License as published by the Free Software Foundation; either
!   version 2 of the License, or (at your option) any later version.

!   This program is distributed in the hope that it will be useful,
!   but WITHOUT ANY WARRANTY; without even the implied warranty of
!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
!   General Public License for more details.

!   You should have received a copy of the GNU General Public
!   License along with this program; if not, write to the Free
!   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

! To report bugs, suggest enhancements, etc. to the Authors,
! Contact:
!    Purple Sage Computing Solutions, Inc.
!                               send email to dnagle@erols.com
!                                   or fax to 703 471 0684 (USA)
!                                  or mail to 12142 Purple Sage Ct.
!                                             Reston, VA 20194-5621 USA

! **********************************************************************
! coco implements Part 3: Conditional Compilation

! **********************************************************************

!  coco reads

!     input source file(s)- named via command line, or stdin
!     named set file- name taken from the output filename, or coco.set

!  coco writes

!     output source file- named via command line, or stdout
!     logfile- named via ??logfile directive, or stderr

!  coco temp files

!     scratch- hold the contents of text blocks while counting their size

!  coco uses

!     f2kcli (http://www.winteracter.com/f2kcli)

!  coco constants

!     coco_rcs_id- this file's rcs id string
!     *_unit- logical unit numbers
!     *_fmt- formats
!     *_len- lengths of character entities
!     alter_*- the alter states
!     if_*- the current state of if-directive processing

!  coco types

!     file_t- filename and unit
!     path_t- include directory
!     symbol_t- integer, logical, macro, text symbol
!     if_t- if block and state
!     state_t- set of options
!     report_t- statistics

!  coco data

!  coco library

!     add_directory() appends a directory to the path list
!     add_integer() build an integer entry and store it on the symbol list
!     add_logical() build a logical entry and store it on the symbol list
!     add_macro() build a macro entry and store it on the symbol list
!     add_symbol() append an entry to the symbol list
!     add_text() build a text entry and store it on the symbol list
!     blank_compress_lower_case() read lines return statement for processing
!     close_file() closes a named file and unlinks pointers
!     close_scratch() closes a scratch file
!     coco() main program
!     copy_set_file() appends the set file to the end of the output
!     edit_coco_strings() edit ?coco? in source
!     edit_date_time_strings() edit ?date? and ?time? strings in source lines
!     edit_file_line_strings() edit ?file? and ?line? strings in source lines
!     edit_integer_strings() edit integer substitutions
!     edit_logical_strings() edit logical substitutions
!     edit_macro_strings() edit macro substitutions
!     edit_source_line() edit a source line with substitutions
!     eval_int_expr() return an integer value from an expression
!     eval_int_primary() return a value from an integer primary
!     eval_log_expr() return a logical value from an expression
!     eval_log_primary() return a value from a logical primary
!     eval_rel_expr() return a logical value from a comparison
!     gather_coco_statement() signal when a complete statement has been read
!     get_integer_value() return the value of an integer symbol
!     get_logical_value() return the value of a logical value
!     get_macro_name() return a macro name from statement
!     get_next_integer() return a pointer to the next integer on the symbol list
!     get_next_logical() return a pointer to the next logical on the symbol list
!     get_next_macro() return a pointer to the next macro on the symbol list
!     get_symbol_name() return integer or logical name from statement
!     get_text_name() return a text block name from statement
!     get_text_ptr() return pointer to a text block name
!     getopt() get each command line word
!     integer_or_logical() determine whether an expression is integer or logical
!     is_coco_statement() decide whether line is a directive or a comment
!     msg_continue() log message and continue
!     msg_quit() log message and stop
!     open_file() opens a file and sets pointers
!     open_scratch() opens a scratch file
!     print_help() print command line options to stderr
!     process_actual_arglist() edit macro definition dummy args
!     process_alter_directive() set alter state from directive
!     process_alter_option() set alter state from -a
!     process_assert_directive() process a coco assert statement
!     process_assertif_directive() process a coco assertif statement
!     process_block_directive() process a coco statement within a text block
!     process_coco_statement() process a coco directive in a source file
!     process_command_line() process command line options including filenames
!     process_copy_directive() process a copy statement
!     process_copyif_directive() process a copyif statement
!     process_directory_directive() process a setfile directory directive
!     process_doc_directive() processes a doc directive
!     process_docfile_directive() processes a docfile directive
!     process_dummy_arglist() process a macro or text definition dummy arg list
!     process_dump_directive() process a dump directive
!     process_edit_directive() process a setfile edit directive
!     process_else_directive() process a coco else statement
!     process_elseif_directive() process a coco else if statement
!     process_endif_directive() process a coco end if statement
!     process_if_directive() process a coco if statement
!     process_ifdef_directive() true if symbol is defined
!     process_ifndef_directive() true if symbol is not defined
!     process_include_directive() process a source file include directive
!     process_include_option() set include directory from -Idir
!     process_input_file() read a source file and process it
!     process_integer_assignment() assign a value to an integer symbol
!     process_integer_constant() define an integer constant
!     process_integer_declaration() process an integer statement
!     process_logfile_directive() process a setfile logfile directive
!     process_logical_assignment() assign a value to a logical symbol
!     process_logical_constant() define a logical constant
!     process_logical_declaration() process a logical statement
!     process_macro_declaration() process a macro statement
!     process_message_directive() process a coco message statement
!     process_number_directive() set numbering from setfile directive
!     process_output_directive() opens a new output file
!     process_set_file() read and process the setfile
!     process_set_statement() process one setfile statement
!     process_stop_directive() process a coco stop statement
!     process_summary_directive() process a setfile summary directive
!     process_symbol_option() define symbol from -Dname
!     process_text_directive() process a begin text statement
!     process_undefine_directive() processes an undefine directive
!     process_verbose_directive() process a setfile verbose directive
!     process_warn_directive() process a setfile warn directive
!     process_wrap_value() process the length supplied on a -w or ??wrap
!     remove_symbol() remove a symbol entry from the symbol list
!     replace_substring() globally replace a substring in a string
!     seek_close_paren() return the location of the matching parenthesis
!     seek_directory() seek an directory to find an include file
!     seek_include_file() find an include file
!     seek_log_primary() find the next logical operator
!     seek_set_file() try to open setfile
!     seek_symbol_name() find an integer or logical name
!     set_option_defaults() check options after command line and setfile processing
!     unquote_string() return an unquoted string
!     verify_actual_args() check actual arguments for need of parenthesis
!     verify_dummy_args() verify macro or text dummy args
!     verify_macro_value() verify a macro's value
!     verfiy_text_directive() verify which directives are in a text block
!     wrap_source_line() ensure that a source line exceeds not length
!     write_coco_line() write a coco line
!     write_options() write the current options to the logfile
!     write_report() log summary statistics
!     write_source_line() write a source line

! **********************************************************************
! Changes made by Pathscale:
! 1. Add command-line option "-#setfile" which emits "#" directives as cpp
!    does to communicate line numbers and file names to the Fortran front end
!    so that it can store them in the debugging information. When this
!    option is in effect, the preprocessor does not search for a file
!    "output.set" to use as the setfile, but does default to coco.set
!    if the setfile name is missing.
! 2. Add command-line option "-E" to indicate that output should appear on
!    stdout even though there is a single source file name.
! 3. If necessary, append the directory separator character "/" to paths
!    specified by -I
! 4. In -# mode, don't print a 'stop' message, but do return Linux status code
! 5. Use Fortran 2003 intrinsics instead of the f2kcli module
!    
! **********************************************************************

! **********************************************************************

!  coco

! **********************************************************************

program coco

!  coco implements ISO/IEC 1539-3 Conditional Compilation standard

!  coco steps

!  1. call process_command_line() to read command line, get filenames & options
!  2. call process_setfile() to read the setfile, if there is one
!  3. open the output file, if named, use stdout if not
!  4. open the input file(s), if named, use stdin if not
!  5. call process_input_file() to process the input file(s) & write the output file
!  6. copy the setfile contents to the output file
!  7. close all files
!  8. call write_report() print summary information

! **********************************************************************

!  explicit declaration

implicit none                                                        ! declare all variables

#ifdef KEY /* Bug 5087 */
intrinsic command_argument_count, get_command_argument, &
   get_environment_variable, exit
#endif /* KEY Bug 5087 */

! **********************************************************************

!  coco RCS strings

! **********************************************************************

!  program source filename supplied by RCS

character( len= *), parameter :: coco_rcs_id = &                     ! printed via -V
   '$Id: coco.f90 1.28 2003/10/22 13:08:20Z Dan Release $'

! **********************************************************************

!  coco constants

! **********************************************************************

!  coco logical unit numbers

!  scheme for logical unit numbers:

!  The setfile is read first and processed.
!  The setfile is then closed.  The output is opened
!  (if need be), and then the input file is opened (again, if need be).
!  If an include file is encountered, the logical unit numbers used are
!  computed by adding to the current input unit number.  If the current
!  input file is stdin, read_unit is used first, then successive unit
!  numbers are used for nested include files.  When all input files have
!  been read, the set scratch file is copied to the output file.  All
!  Fortran files are then closed.  The summary is written to the output.
!  A text block is copied to the text_unit to count the number of lines.
!  Then it is copied back to a character array and the text_unit is closed.

! **********************************************************************

!  global constants

!  the unit names are provided in the Fortran 2003 intrinsic module iso_fortran_env

integer, parameter :: input_unit = 5
integer, parameter :: output_unit = 6
integer, parameter :: error_unit = 0

!  logfile unit else use error_unit, +4 tries to avoid plot_unit, punch_unit, etc.

integer, parameter :: log_unit = max( input_unit, output_unit, error_unit) + 4

!  documentation unit for doc ... end doc lines

integer, parameter :: doc_unit = log_unit + 1                        ! doc file

!  scratch unit for text scratch files

integer, parameter :: text_unit = doc_unit + 1                       ! text scratch file

!  setfile unit

integer, parameter :: set_unit = text_unit + 1                       ! setfile

!  output unit if named output file (else use unit= *)

integer, parameter :: write_unit = set_unit + 1                      ! (sole) output unit

!  input unit if named input file (else use unit= *)

integer, parameter :: read_unit = write_unit + 1                     ! input(s) unit(s) start here

#ifdef KEY /* Bug 5087 */
#if defined(LINUX) || defined(BUILD_OS_DARWIN)
character(len=1), parameter :: directory_separator = '/'
#else
error: need to specify a directory separator for the OS on which coco will run
#endif /* LINUX */
#endif /* KEY Bug 5087 */

! **********************************************************************

!  coco formats

! **********************************************************************

!  used to read/write lines

character( len= *), parameter :: string_fmt = '( a)'                 ! all characters

!  used to write reports

character( len= *), parameter :: integer_fmt = '( a, i10)'           ! report integer counts

!  used to write reports

character( len= *), parameter :: directory_fmt = '( a, i0, a)'       ! report directory(count)

!  length of format strings

integer, parameter :: format_len = max( len( string_fmt), len( integer_fmt) )

!  length of input/output specifier strings

integer, parameter :: io_specifier_len = 16                          ! form=, action=, status=

! ----------------------------------------------------------------------

!  length of strings used to convert between integers and characters

integer, parameter :: conversion_len = 10                            ! 10^10 > 32 bit integer

!  format used to convert between integers and characters

character( len= *), parameter :: conversion_fmt = '(i10)'            ! integer <-> character

! **********************************************************************

!  coco character lengths

! **********************************************************************

!  these are the lengths of strings used throughout coco

! ----------------------------------------------------------------------

!  length of character storing a constant or variable name

integer, parameter :: symbol_name_len = 32                           ! standard ;ength plus one

!  length of a Fortran source line

integer, parameter :: free_format_len = 132                          ! source length in free format

integer, parameter :: fixed_format_len = 72                          ! source length in fixed source format

!  length used to write lines is free_format_len + len( '!?>') + len( blank)

integer, parameter :: source_line_len = free_format_len + len( '!?>') + len( ' ')

!  length of character storing filenames

integer, parameter :: filename_len = 256                             ! filenames

!  length of character line buffers (allows for max_continuations number of continuations)

integer, parameter :: max_continuations = 39                         ! specified by the standard

!  buffer a whole coco statement and always have a blank at the end

integer, parameter :: buffer_len = ( max_continuations + 1) * source_line_len

! **********************************************************************

!  this string is used to initialize character variables

! ----------------------------------------------------------------------

!  null string

character( len= *), parameter :: null_string = ''                    ! initialize strings

!  mark beginning of the setfile in the output

character( len= *), parameter :: mark_set_file = &                   ! separates SET file from output
   "?? This was produced using the following SET file"               ! string specified by the standard

! ----------------------------------------------------------------------

!  names must be made of alphanumeric characters only

character( len= *), parameter :: alpha_chars = 'abcdefghijklmnopqrstuvwxyz'

character( len= *), parameter :: digit_chars = '0123456789'

character( len= *), parameter :: underscore = '_'

character( len= *), parameter :: alphanum_chars =  alpha_chars // digit_chars // underscore

! ----------------------------------------------------------------------

!  ascii characters change case

integer, parameter :: change_case = 32                               ! ascii (lc = uc + change_case)

! **********************************************************************

!  coco directives constants

! **********************************************************************

!  many character string constants' lengths are used to count past
!  the string as coco processes each statement

!  coco line and statement syntax uses the next set of character constants

! **********************************************************************

!  . separates filenames from extensions, delimits logical operators & literals

character( len= *), parameter :: dot = '.'                           ! dot

! ----------------------------------------------------------------------

!  constants defining coco directives, comments, separators, etc.

! ----------------------------------------------------------------------

!  coco line key ??coco_directive

character( len= *), parameter :: coco_key = '??'                     ! starts a coco line

!  substitution key

character( len= *), parameter :: arg_key = '?'                       ! initiates a substitution

!  length of ?name?

integer, parameter :: target_len = len( arg_key) + symbol_name_len + len( arg_key)

!  continuation character

character( len= *), parameter :: continuation = '&'                  ! coco continuation

!  blank character

character( len= *), parameter :: blank = ' '                         ! whitespace in coco lines

!  tab character

character( len= *), parameter :: tab = achar( 9)                     ! whitespace in coco lines

!  whitespace is blank or tab

character( len= *), parameter :: white_space = blank // tab          ! any whitespace

!  coco comment initializer

character( len= *), parameter :: comment = '!'                       ! starts a coco comment

!  separates items within a list

character( len= *), parameter :: comma = ','                         ! between items

!  quotes

character( len= *), parameter :: single_quote = "'"                  ! single quote

character( len= *), parameter :: double_quote = '"'                  ! double quote

! **********************************************************************

!  process_logical_declaration() constants

! ----------------------------------------------------------------------

!  process name[=value][,name[=value]]...

character( len= *), parameter :: end_of_decl = comma // blank        ! either may end a definition

! ----------------------------------------------------------------------

!  constants defining coco (integer or logical) operators, constants, etc.

! ----------------------------------------------------------------------

!  minus sign

character( len= *), parameter :: minus = '-'                         ! unary or binary operator

!  plus sign

character( len= *), parameter :: plus = '+'                          ! unary or binary operator

! ----------------------------------------------------------------------

!  logical binary operators

character( len= *), parameter :: or_str = '.or.'                     ! the or operator

character( len= *), parameter :: and_str = '.and.'                   ! the and operator

character( len= *), parameter :: eqv_str = '.eqv.'                   ! the equivalent operator

character( len= *), parameter :: neqv_str = '.neqv.'                 ! the not equivalent operator

! ----------------------------------------------------------------------

!  logical uniary operator

character( len= *), parameter :: not_str = '.not.'                   ! the not operator

! ----------------------------------------------------------------------

!  logical literals

character( len= *), parameter :: true_str = '.true.'                 ! true string

character( len= *), parameter :: false_str = '.false.'               ! false string

! ----------------------------------------------------------------------

!  the archaic versions of the relational operators

character( len= *), parameter :: dot_eq = '.eq.'                     ! equals

character( len= *), parameter :: dot_ne = '.ne.'                     ! not equals

character( len= *), parameter :: dot_gt = '.gt.'                     ! greater than

character( len= *), parameter :: dot_ge = '.ge.'                     ! greater than and equal to

character( len= *), parameter :: dot_le = '.le.'                     ! less than and equal to

character( len= *), parameter :: dot_lt = '.lt.'                     ! less than

!  the modern versions of the relational operators

character( len= *), parameter :: ch_eq = '=='                        ! equals

character( len= *), parameter :: ch_ne = '/='                        ! not equals

character( len= *), parameter :: ch_gt = '>'                         ! greater than

character( len= *), parameter :: ch_ge = '>='                        ! greater than or equal to

character( len= *), parameter :: ch_le = '<='                        ! less than or equal to

character( len= *), parameter :: ch_lt = '<'                         ! less than

! ----------------------------------------------------------------------

!  strings used to declare symbol names and values

! ----------------------------------------------------------------------

!  equal sign

character( len= *), parameter :: equals = '='                        ! equals symbol

!  open parenthesis

character( len= *), parameter :: open_paren = '('                    ! between name and arglist

!  close parenthesis

character( len= *), parameter :: close_paren = ')'                   ! between name and arglist

! ----------------------------------------------------------------------

!  directives which must appear in the setfile

! ----------------------------------------------------------------------

!  alter directive

character( len= *), parameter :: alter_str = 'alter:'                ! ?? alter: ...

!  directory declaration

character( len= *), parameter :: directory_str = 'directory'         ! ?? directory '...'

!  edit directive allows changing the edit mode from the setfile

character( len= *), parameter :: edit_str = 'edit:'                  ! ?? edit: on|off

!  docfile declaration

character( len= *), parameter :: docfile_str = 'docfile'             ! ?? docfile '...'

!  logfile declaration

character( len= *), parameter :: logfile_str = 'logfile'             ! ?? logfile '...'

!  number directive controls placing "! file: line" strings on source lines

character( len= *), parameter :: number_str = 'number:'              ! ?? number: on|off

!  parens directive sets warning when actual arguments don't have enclosing parenthesis

character( len= *), parameter :: parens_str = 'parens:'              ! ?? parens: on|off

!  summary directive allows changing the summary mode from the setfile

character( len= *), parameter :: summary_str = 'summary:'            ! ?? summary: on|off

!  verbose directive allows changing the verbose mode from the setfile

character( len= *), parameter :: verbose_str = 'verbose:'            ! ?? verbose: on|off

!  warn directive allows changing the warning mode from the setfile

character( len= *), parameter :: warn_str = 'warn:'                  ! ?? warn: on|off

!  wrap directive allows changing the wrap length from the setfile

character( len= *), parameter :: wrap_str = 'wrap:'                  ! ?? wrap: <n>

! ----------------------------------------------------------------------

!  directives which may appear in the setfile or source file

! ----------------------------------------------------------------------

!  integer declaration

character( len= *), parameter :: integer_str = 'integer::'           ! ?? integer :: ...

!  integer constant declaration

character( len= *), parameter :: integer_constant_str = 'integer,parameter::'

!  logical declaration

character( len= *), parameter :: logical_str = 'logical::'           ! ?? logical :: ...

!  logical constant declaration

character( len= *), parameter :: logical_constant_str = 'logical,parameter::'

! ----------------------------------------------------------------------

!  directives which must appear in the source file

! ----------------------------------------------------------------------

!  include directive

character( len= *), parameter :: include_str = 'include'             ! ?? include '...'

! ----------------------------------------------------------------------

!  stop directive

character( len= *), parameter :: stop_str = 'stop'                   ! ?? stop

! ----------------------------------------------------------------------

!  message directive

character( len= *), parameter :: message_str = 'message'             ! ?? message ...

! ----------------------------------------------------------------------

!  if directive

character( len= *), parameter :: if_str = 'if('                      ! ?? if( ...

! ----------------------------------------------------------------------

!  elseif directive

character( len= *), parameter :: elseif_str = 'elseif('              ! ?? elseif( ...

! ----------------------------------------------------------------------

!  )then must close an if( or elseif(

character( len= *), parameter :: then_str = ')then'                  ! ... )then

! ----------------------------------------------------------------------

!  else directive

character( len= *), parameter :: else_str = 'else'                   ! ?? else

! ----------------------------------------------------------------------

!  endif directive

character( len= *), parameter :: endif_str = 'endif'                 ! ?? endif

! ----------------------------------------------------------------------

!  directives which are extensions

! ----------------------------------------------------------------------

!  ifdef directive

character( len= *), parameter :: ifdef_str = 'ifdef('                ! ?? ifdef( ...

! ----------------------------------------------------------------------

!  ifndef directive

character( len= *), parameter :: ifndef_str = 'ifndef('              ! ?? ifndef( ...

! ----------------------------------------------------------------------

!  undefine directive

character( len= *), parameter :: undefine_str = 'undefine::'         ! ?? undefine:: ...

! ----------------------------------------------------------------------

!  macro declaration

character( len= *), parameter :: macro_str = 'macro::'               ! ?? macro :: ...

! ----------------------------------------------------------------------

!  assert directive

character( len= *), parameter :: assert_str = 'assert'               ! ?? assert '...'

! ----------------------------------------------------------------------

!  assertif directive

character( len= *), parameter :: assertif_str = 'assertif('          ! ?? assertif( ... ) '...'

! ----------------------------------------------------------------------

!  dump directive

character( len= *), parameter :: dump_str = 'dump'                   ! ?? dump

!  options directive

character( len= *), parameter :: options_str = 'options'             ! ?? options

!  report directive

character( len= *), parameter :: report_str = 'report'               ! ?? report

! ----------------------------------------------------------------------

!  text directive

character( len= *), parameter :: text_str = 'text::'                 ! ?? text :: ...

!  copy directive

character( len= *), parameter :: copy_str = 'copy::'                 ! ?? copy :: ...

!  copyif directive

character( len= *), parameter :: copyif_str = 'copyif('              ! ?? copyif( ... ) ...

! ----------------------------------------------------------------------

!  doc directive (the end doc directive is defined in process_doc_directive() )

character( len= *), parameter :: doc_str = 'doc'                     ! ?? doc

!  endfile directive

character( len= *), parameter :: endfile_str = 'endfile'             ! ?? endfile

!  output directive

character( len= *), parameter :: output_str = 'output'               ! ?? output '...'

! ----------------------------------------------------------------------

!  these strings are replaced when editing source lines

! ----------------------------------------------------------------------

!  provide file, line, date, time strings in programs

character( len= *), parameter :: file_str = '?file?'                 ! file target

character( len= *), parameter :: line_str = '?line?'                 ! line target

character( len= *), parameter :: date_str = '?date?'                 ! date of preprocessing

character( len= *), parameter :: time_str = '?time?'                 ! time of preprocessing

!  provide coco rcs id string in programs

character( len= *), parameter :: coco_str = '?coco?'                 ! coco rcs id

! ----------------------------------------------------------------------

!  on directive

character( len= *), parameter :: on_str = 'on'                       ! ... on

!  off directive

character( len= *), parameter :: off_str = 'off'                     ! ... off

! **********************************************************************

!  possible states encountered during execution

! ----------------------------------------------------------------------

!  codes for possible alter states

integer, parameter :: alter_none = 0                                 ! no alter code has been set

integer, parameter :: alter_delete = 1                               ! delete unused lines

integer, parameter :: alter_blank = 2                                ! blank unused lines

integer, parameter :: alter_shift_1 = 3                              ! prepend ! to unused lines

integer, parameter :: alter_shift_0 = 4                              ! overwrite ! on unused lines

integer, parameter :: alter_shift_3 = 5                              ! prepend !?> to unused lines

! ----------------------------------------------------------------------

!  wrap lengths

integer, parameter :: wrap_none = -1                                 ! no wrap length has been set

integer, parameter :: wrap_off = huge( 0)                            ! no wrapping

! ----------------------------------------------------------------------

!  codes for possible symbol types

integer, parameter :: type_none = 0                                  ! not (yet) typed

integer, parameter :: type_integer = 1                               ! is integer

integer, parameter :: type_logical = 2                               ! is logical

integer, parameter :: type_macro = 3                                 ! is macro

integer, parameter :: type_text = 4                                  ! is text block

! ----------------------------------------------------------------------

!  codes for possible if construct phases

integer, parameter :: outside_block = 0                              ! outside any if construct

integer, parameter :: if_block = 1                                   ! within an if block

integer, parameter :: elseif_block = 2                               ! within an elseif block

integer, parameter :: else_block = 3                                 ! within an else block

! **********************************************************************

!  communication with getopt()

! ----------------------------------------------------------------------

!  getopt() 'no more arguments'

integer, parameter :: end_of_args = -1                               ! returned by getopt()

!  getopt() 'not in optltrs'

character( len= *), parameter :: unknown_option = '?'                ! returned by getopt()

! **********************************************************************

!  coco types

! **********************************************************************

!  coco files and search paths

! ----------------------------------------------------------------------

!  file type

type :: file_t                                                       ! a coco file

   integer :: logical_unit                                           ! logical unit number

   character( len= filename_len) :: name_str                         ! name (null_string if none)

   character( len= format_len) :: format_str                         ! format (null_string if none)

   character( len= free_format_len), pointer :: line                 ! the line transfered

   integer :: io_status                                              ! status of last operation

   integer :: lines_transfered                                       ! lines read/written

   logical :: named_file                                             ! true if named file

   logical :: create                                                 ! true to create and write

end type file_t                                                      ! a coco file

! ----------------------------------------------------------------------

!  search location type

type :: path_t                                                       ! a directory to search for includes

   character( len= filename_len) :: name_str                         ! directory name

   integer :: times_accessed                                         ! number of times file found here

   type( path_t), pointer :: next                                    ! next element in list

end type path_t                                                      ! a directory to search for includes

! **********************************************************************

!  these derived types are used to store coco constants or variables

! ----------------------------------------------------------------------

!  type stores a coco symbol & value

type :: symbol_t                                                     ! a coco symbol

   integer :: type_code                                              ! type= integer, logical, macro, text

   character( len= symbol_name_len) :: name_str                      ! name of symbol

   logical :: defined                                                ! true if value assigned

   logical :: constant                                               ! true if parameter

   logical :: predefined                                             ! true if defined in setfile

   integer :: integer_value                                          ! integer value

   logical :: logical_value                                          ! logical value

   character( len= symbol_name_len), pointer, dimension( :) :: dummy_args

   character( len= buffer_len) :: macro_value                        ! macro string value

   character( len= free_format_len), pointer, dimension( :) :: text_lines

   type( symbol_t), pointer :: next                                  ! next symbol on list

end type symbol_t                                                    ! a coco symbol

! **********************************************************************

!  if_t stores the state of an if block

! ----------------------------------------------------------------------

!  if_t

type :: if_t                                                         ! a coco if construct

   logical :: now_selected                                           ! the "executed" block

   logical :: ever_selected                                          ! passed an "executed" block

   integer :: phase                                                  ! if_block, elseif_block, else_block

   type( if_t), pointer :: nested                                    ! nested if construct

   type( if_t), pointer :: enclosing                                 ! enclosing if construct

end type if_t                                                        ! a coco if construct

! **********************************************************************

!  state_t stores a set of coco options

! ----------------------------------------------------------------------

!  state_t

type :: state_t                                                      ! a coco option state

   integer :: alter_state                                            ! the alter state

   logical :: edit_date                                              ! edit date & time strings

   logical :: edit_file                                              ! edit file & line strings

   logical :: edit_source                                            ! edit source lines

   logical :: edit_integers                                          ! edit integer & logical strings

   logical :: edit_macros                                            ! edit macro strings

   logical :: number_source                                          ! put "! file: line" on source lines

   logical :: args_in_parens                                         ! warn when args are not enclosed in parenthesis

   logical :: report_extensions                                      ! report processing an extension

   logical :: print_summary                                          ! print summary to logfile

   logical :: warn_undeclared                                        ! warn of symbols in setfile but not source

   logical :: verbose_mode                                           ! print file opening & closing

   integer :: wrap_length                                            ! wrap output length before this

#ifdef KEY /* Bug 5087 */
   logical :: integrated ! integrated with Fortran front end
   logical :: write_stdout ! write to stdout instead of explicit output file
#endif /* KEY Bug 5087 */

end type state_t                                                     ! a coco option state

! **********************************************************************

!  report_t stores coco statistics

! ----------------------------------------------------------------------

!  report_t records the source and sink of lines

type :: report_t                                                     ! report coco statistics

   integer :: input_lines                                            ! total lines read

   integer :: input_files                                            ! total files read

   integer :: include_files                                          ! total include files

   integer :: coco_lines                                             ! coco statements

   integer :: selected_lines                                         ! source lines in selected ifs

   integer :: elided_lines                                           ! source lines in nonselected ifs

   integer :: text_blocks                                            ! text block definitions

   integer :: text_lines                                             ! text lines defined

   integer :: copied_lines                                           ! text lines written

end type report_t                                                    ! report coco statistics

! **********************************************************************

!  coco variables

! **********************************************************************

!  option swtiches

! ----------------------------------------------------------------------

type( state_t) :: options = state_t( alter_none, &                   ! default is alter_shift3
                                     .true., &                       ! do process ?date? & ?time?
                                     .true., &                       ! do source editing
                                     .true., &                       ! do process ?file? & ?line?
                                     .true., &                       ! do process ?integer? & ?logical?
                                     .true., &                       ! do process ?macro?
                                     .false., &                      ! do not number source lines
                                     .false., &                      ! do not warn when args are not (arg)
                                     .false., &                      ! do not report use of extensions
                                     .true., &                       ! do make summary report
                                     .false., &                      ! do not report undeclared
                                     .false., &                      ! do not report open & close
#ifdef KEY /* Bug 5087 */
                                     wrap_none, &                    ! default is free_format_len
                                     .false., &                      ! integrated
                                     .false. &                       ! write_stdout
#else /* KEY Bug 5087 */
                                     wrap_none &                     ! default is free_format_len
#endif /* KEY Bug 5087 */
                                     )

! ----------------------------------------------------------------------

!  report actions initially zero

type( report_t) :: total = report_t( 0, 0, 0, 0, 0, 0, 0, 0, 0)

! ----------------------------------------------------------------------

!  if construct outside any if construct

type( if_t), target :: outside_any_if_construct = if_t( .true., .true., outside_block, null(), null() )

!  if construct status

type( if_t), pointer :: if_construct => null()                       ! process nested if-then-else

! ----------------------------------------------------------------------

!  coco symbols are stored in a singly linked list

type( symbol_t), pointer :: first_symbol => null()                   ! first symbol on list

! ----------------------------------------------------------------------

!  mark when non constants are used to provide a value for a constant

logical :: all_constants                                             ! flag non constants

! ----------------------------------------------------------------------

!  signal when reading the setfile

logical :: processing_set_file = .true.                              ! mark symbols from setfile (or command line)

! **********************************************************************

!  coco file name variables

! ----------------------------------------------------------------------

!  input file, output file, or setfile

! ----------------------------------------------------------------------

!  the (first) input file

type( file_t), target :: input_file = file_t( &
                                         input_unit, '<stdin>', &    ! unit, name
                                         string_fmt, null(), &       ! format, line pointer
                                         0, 0, &                     ! iostat, lines transfered
                                         .false., .false.)           ! unnamed, do not create

! ----------------------------------------------------------------------

!  the output file

type( file_t), target :: output_file = file_t( &
                                          output_unit, '<stdout>', & ! unit, name
                                          string_fmt, null(), &      ! format, line pointer
                                          0, 0, &                    ! iostat, lines transfered
                                          .false., .true.)           ! unnamed, create

! ----------------------------------------------------------------------

!  the docfile

type( file_t), target :: doc_file = file_t( &
                                          doc_unit, null_string, &   ! unit, name
                                          string_fmt, null(), &      ! format, line pointer
                                          0, 0, &                    ! iostat, lines transfered
                                          .false., .true.)           ! unnamed, create

! ----------------------------------------------------------------------

!  the setfile

type( file_t), target :: set_file = file_t( set_unit, null_string, & ! unit, name
                                           string_fmt, null(), &     ! format, line pointer
                                           0, 0, &                   ! iostat, lines transfered
                                           .false., .false.)         ! unnamed, do not create

! ----------------------------------------------------------------------

!  the logfile

type( file_t) :: log_file = file_t( error_unit, '<stderr>', &        ! unit, name
                                   integer_fmt, null(), &            ! format, line pointer
                                   0, 0, &                           ! iostat, lines transfered
                                   .false., .true.)                  ! unnamed, create

! ----------------------------------------------------------------------

!  point to current input file for error messages

type( file_t), pointer :: current_file => null()                     ! set to known state

! ----------------------------------------------------------------------

!  a list of source files for reports

type( file_t), allocatable, dimension(:) :: source_file_list         ! print list of input files

!  number of filenames

integer :: number_of_names                                           ! names on command line

! ----------------------------------------------------------------------

!  the input/output line buffer

character( len= free_format_len), target :: line                     ! files link to this buffer

!  the logfile line buffer

character( len= free_format_len), target :: log_line                 ! logfile links to this buffer

! ----------------------------------------------------------------------

!  list of include directories is initially . only

type( path_t), pointer :: first_directory => null()                  ! first directory in path (after .)

! ----------------------------------------------------------------------

!  communicate with getopt()

! ----------------------------------------------------------------------

!  getopt() string returning non-option letter words

character( len= filename_len) :: optarg = null_string                ! option argument

! ----------------------------------------------------------------------

!  number of command line args

integer :: nargs                                                     ! words on command line

!  count command line words

integer :: optind = 0                                                ! current option

! **********************************************************************

!  coco local

! ----------------------------------------------------------------------

!  loop index of filename args

   integer :: this_input                                             ! do index

! **********************************************************************

!  coco text

! **********************************************************************

continue                                                             ! coco

! ----------------------------------------------------------------------

!  initialize pointers

   if_construct => outside_any_if_construct                          ! not in an if construct at the start

   log_file% line => log_line                                        ! link logfile to its buffer

   output_file% line => line                                         ! link output to the i/o buffer

! ----------------------------------------------------------------------

!  process command line up to the filenames

   call process_command_line                                         ! process command line

! ----------------------------------------------------------------------

!  see if setfile exists and process it if it does

   call seek_set_file                                                ! process setfile

! ----------------------------------------------------------------------

!  set option to default values if the command line or the setfile hasn't set them

   call set_option_defaults                                          ! process options

! ----------------------------------------------------------------------

!  open the output file but link not current_file

   call open_file( output_file)                                      ! open the output

! **********************************************************************

!  read all input file(s)

   number_of_input_files: select case( number_of_names)              ! how many args left on command line

   case( 0, 1) number_of_input_files                                 ! files are stdin or stdout or one name (suffixes added)

!  process the input file

      call process_input_file( input_file)                           ! process the single input file

!  end of input

   case default number_of_input_files                                ! names are explicit on command line

!  process several input files

      read_all_files: do this_input = 2, number_of_names

!  process the input using coco default units

         call process_input_file( source_file_list( this_input) )    ! process next named input file

!  repeat for each input file

      enddo read_all_files                                           ! until command line exhausted

!  end of input

   end select number_of_input_files                                  ! number of input file(s)

! **********************************************************************

!  if the output file has content, copy the setfile to it

   made_output: if( output_file% lines_transfered > 0 )then          ! if any actual output

!  mark the setfile in the output (whether it is present or not)

      line = mark_set_file

      call write_coco_line( output_file)                             ! use the current alter setting

! ----------------------------------------------------------------------

!  if processed a setfile

      append_set_file: if( set_file% named_file )then                ! copy setfile

!  copy setfile file to output

         call copy_set_file                                          ! setfile to output

!  if processed setfile

      endif append_set_file                                          ! copy setfile

   endif made_output                                                 ! if actual output

! ----------------------------------------------------------------------

!  if processed a docfile

   close_doc_file: if( doc_file% named_file )then                    ! have docfile

!  copy setfile file to output

      call close_file( doc_file)                                     ! close docfile

!  if processed docfile

   endif close_doc_file                                              ! have docfile

! ----------------------------------------------------------------------

!  close the output file

   call close_file( output_file)

! ----------------------------------------------------------------------

!  report to logfile

   summary: if( options% print_summary )then                         ! if summary requested

      call write_report                                              ! lines read, written, etc.

   endif summary                                                     ! if summary requested

! ----------------------------------------------------------------------

!  close the logfile

   call close_file( log_file)

! ----------------------------------------------------------------------

!  coco exit

#ifdef KEY /* Bug 5087 */
if (options%integrated) call exit(0)
#endif /* KEY Bug 5087 */

stop 'coco normal exit'                                              ! coco

! **********************************************************************

!  coco library

! **********************************************************************

contains                                                             ! coco

! **********************************************************************
! **********************************************************************

!  %%% open and close files, write logfile messages, parse command line

! **********************************************************************
! **********************************************************************

!  open_file() open a file and remark

subroutine open_file( this_file)

! **********************************************************************

!  open_file() interface

! ----------------------------------------------------------------------

!  the file to be opened

type( file_t), target, intent( inout) :: this_file

! **********************************************************************

!  open_file() constants

! ----------------------------------------------------------------------

!  open for reading or writing

   character( len= *), parameter :: read_action = 'READ'

   character( len= *), parameter :: write_action = 'WRITE'

!  open existing file or create a new one

   character( len= *), parameter :: read_status = 'OLD'

   character( len= *), parameter :: write_status = 'REPLACE'

! **********************************************************************

!  open_file() local

! ----------------------------------------------------------------------

!  open for reading or writing

   character( len= io_specifier_len) :: open_status

   character( len= io_specifier_len) :: open_action

! **********************************************************************

!  open_file() text

! ----------------------------------------------------------------------

continue                                                             ! open_file()

! ----------------------------------------------------------------------

!  open the file if file is named

   file_has_name: if( this_file% named_file )then                    ! named logfile

!  establish open parameters for reading or writing

      direction: if( this_file% create )then                         ! if opening for writing

         open_status = write_status                                  ! create the file

         open_action = write_action                                  ! write to the file

      else direction                                                 ! if opening for reading

         open_status = read_status                                   ! existing file

         open_action = read_action                                   ! read from the file

      endif direction                                                ! open for reading or writing

!  open this file

      open( unit= this_file% logical_unit, &                         ! specify unit
            file= this_file% name_str, &                             ! specify file
            status= open_status, &                                   ! specify current status
            action= open_action, &                                   ! specify direction
            iostat= this_file% io_status)                            ! check result status

      current_file => this_file                                      ! give correct messages

      named_status: if( this_file% io_status > 0 )then               ! open error

         call msg_quit( "can't open file: " // trim( this_file% name_str) )

      elseif( options% verbose_mode )then named_status               ! remark on open

         call msg_continue( "opened file: " // trim( this_file% name_str) )

         nullify( current_file)                                      ! point only to input files

      endif named_status                                             ! open error

   endif file_has_name                                               ! named logfile

!  the logfile is never the current input file, since it receives error messages

   current_input_only: select case( this_file% logical_unit)         ! the current file points to an input file

   case( input_unit, set_unit, read_unit: )                          ! possible input units

      current_file => this_file                                      ! point to the input file

      this_file% line => line                                        ! link input to buffer

   case( output_unit)                                                ! only output unit

      this_file% line => line

   case( log_unit)                                                   ! only logfile unit

      this_file% line => log_line

   end select current_input_only                                     ! the current file points to an input file

! ----------------------------------------------------------------------

!  open_file() exit

return

! **********************************************************************

!  open_file()

end subroutine open_file                                             ! open_file()

! **********************************************************************
! **********************************************************************

!  open_scratch() open an unformatted scratch file

subroutine open_scratch( this_file)

! **********************************************************************

!  open_scratch() interface

! ----------------------------------------------------------------------

!  the scratch file to be opened

type( file_t), target, intent( inout) :: this_file

! **********************************************************************

!  open_scratch() text

! ----------------------------------------------------------------------

continue                                                             ! open_scratch()

! ----------------------------------------------------------------------

!  open the file

   open( unit= this_file% logical_unit, &                            ! specify unit
         status= 'SCRATCH', &                                        ! specify current status
         action= 'READWRITE', &                                      ! specify direction
         form= 'UNFORMATTED', &                                      ! scratch file is unformatted
         iostat= this_file% io_status)                               ! check result status

   scratch_status: if( this_file% io_status > 0 )then                ! open error

      current_file => this_file                                      ! always point to correct current file

      call msg_quit( "can't open scratch file: <scratch>")

   endif scratch_status                                              ! open error

!  link to line buffer

   this_file% line => line

! ----------------------------------------------------------------------

!  open_scratch() exit

return                                                               ! open_scratch()

! **********************************************************************

!  open_scratch()

end subroutine open_scratch

! **********************************************************************
! **********************************************************************

!  close_file() close a file and remark

subroutine close_file( this_file)

! **********************************************************************

!  close_file() interface

! ----------------------------------------------------------------------

!  the file to be closed

type( file_t), target, intent( inout) :: this_file

! **********************************************************************

!  close_file() text

! ----------------------------------------------------------------------

continue                                                             ! close_file()

! ----------------------------------------------------------------------

!  close the named file

   close_named: if( this_file% named_file )then                      ! do not close preconnected files

      close( unit= this_file% logical_unit, &                        ! specify unit
             status= 'KEEP', &                                       ! specify current status
             iostat= this_file% io_status)                           ! check result status

      logfile_close: if( this_file% logical_unit == log_unit )then

         this_file% logical_unit = error_unit                        ! can't use logfile

      endif logfile_close                                            ! logfile is open

      current_file => this_file                                      ! always report correct current file

      close_status: if( this_file% io_status > 0 )then               ! close error

         call msg_quit( "can't close file: " // trim( this_file% name_str) )

      elseif( options% verbose_mode )then close_status               ! remark on close

        call msg_continue( "closed file: " // trim( this_file% name_str) )

      endif close_status                                             ! close error

   endif close_named                                                 ! do not close preconnected files

!  file is not connected

   nullify( current_file)

! ----------------------------------------------------------------------

!  close_file() exit

return                                                               ! close_file()

! **********************************************************************

!  close_file()

end subroutine close_file

! **********************************************************************
! **********************************************************************

!  close_scratch() close a file and remark

subroutine close_scratch( this_file)

! **********************************************************************

!  close_scratch() interface

! ----------------------------------------------------------------------

!  the scratch file to be closed

type( file_t), target, intent( inout) :: this_file

! **********************************************************************

!  close_scratch() text

! ----------------------------------------------------------------------

continue                                                             ! close_scratch()

! ----------------------------------------------------------------------

!  close the scratch file

   close( unit= this_file% logical_unit, &                           ! specify unit
          status= 'DELETE', &                                        ! specify current status
          iostat= this_file% io_status)                              ! check result status

   close_status: if( this_file% io_status > 0 )then                  ! close error

      current_file => this_file                                      ! always report corrent current file

      call msg_quit( "can't close scratch file: <scratch>")

   endif close_status                                                ! close error

! ----------------------------------------------------------------------

!  close_scratch() exit

return                                                               ! close_scratch()

! **********************************************************************

!  close_scratch()

end subroutine close_scratch                                         ! close_scratch()

! **********************************************************************
! **********************************************************************

!  set_option_defaults() set options to their default values

subroutine set_option_defaults

! **********************************************************************

!  Some options are initially set to absurd values in order to allow
!  the command line option to override the corresponding setfile directive.
!  These options need to be set to useful values after the setfile
!  has been initially read but before coco further executes.

!  These options are: the alter mode and the wrap length.

!  The options selected are also made mutually consistent.

! **********************************************************************

!  set_option_defaults() text

! ----------------------------------------------------------------------

continue                                                             ! set_option_defaults()

! ----------------------------------------------------------------------

!  if the command line or the setfile hasn't set the alter state, set it to the default

   alter_default: if( options% alter_state == alter_none )then       ! initial

      options% alter_state = alter_shift_3                           ! default

   endif alter_default                                               ! alter state not set

!  if the command line or the setfile hasn't set the wrap length, set it to the default

   wrap_default: if( options% wrap_length == wrap_none )then         ! initial

      options% wrap_length = free_format_len                         ! default

   endif wrap_default                                                ! wrap length not set

! ----------------------------------------------------------------------

!  ensure the correct relationship among the options

! ----------------------------------------------------------------------

!  edit specific items only if editing generally

   options% edit_date = options% edit_date .and. options% edit_source
   options% edit_file = options% edit_file .and. options% edit_source
   options% edit_integers = options% edit_integers .and. options% edit_source
   options% edit_macros = options% edit_macros .and. options% edit_source

! ----------------------------------------------------------------------

!  report specific items only if reporting generally

   options% args_in_parens = options% args_in_parens .and. options% print_summary
   options% report_extensions = options% report_extensions .and. options% print_summary
   options% warn_undeclared = options% warn_undeclared .and. options% print_summary
   options% verbose_mode = options% verbose_mode .and. options% print_summary

! ----------------------------------------------------------------------

!  set_option_defaults() exit

return                                                               ! set_option_defaults()

! **********************************************************************

!  set_option_defaults()

end subroutine set_option_defaults

! **********************************************************************
! **********************************************************************

!  msg_quit() process error and stop

subroutine msg_quit( msg)

! **********************************************************************

!  msg_quit() interface

! ----------------------------------------------------------------------

!  the error message

character( len= *), intent( in) :: msg

! **********************************************************************

!  msg_quit() local

! ----------------------------------------------------------------------

   character( len= conversion_len) :: number_str                     ! character line number

   character( len= conversion_len) :: iostat_str                     ! character iostat code

! **********************************************************************

!  msg_quit() text

! ----------------------------------------------------------------------

continue                                                             ! msg_quit()

! ----------------------------------------------------------------------

!  if file is associated with this error

   file_msg: if( associated( current_file) )then                     ! if there is a current file

!  if a line is associated with this error

      line_msg: if( associated( current_file% line) )then            ! if there is a current line

         write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( current_file% line)

      endif line_msg                                                 ! if there is a current line

!  if io error caused this error

      io_error: if( current_file% io_status > 0 )then                ! if there is an io error

!  decode line number & iostat

         write( unit= number_str, fmt= conversion_fmt) current_file% lines_transfered

         write( unit= iostat_str, fmt= conversion_fmt) current_file% io_status

!  write error message with file data

         write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( 'coco error: file: ' &
                // trim( current_file% name_str) // ', line: ' // trim( adjustl( number_str)) &
                // ', ' // ', iostat: ' // trim( adjustl( iostat_str)) // ': ' // msg)

!  if io error caused not this error

      else io_error                                                  ! if there is an io error

!  decode line number

         write( unit= number_str, fmt= conversion_fmt) current_file% lines_transfered

!  write error message with file data

         write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( 'coco error: file: ' &
                // trim( current_file% name_str) // ', line: ' // trim( adjustl( number_str)) &
                // ', ' // msg)

      endif io_error                                                 ! if there is an io error

!  if file associated not with this error

   else file_msg                                                     ! if there is a current file

!  write error message without file data

      write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( 'coco error: ' // msg)

   endif file_msg                                                    ! if there is a current file

! ----------------------------------------------------------------------

!  msg_quit() exit

#ifdef KEY /* Bug 5087 */
if (options%integrated) call exit(1)
#endif /* KEY Bug 5087 */

stop 'coco error exit'                                               ! msg_quit()

! **********************************************************************

!  msg_quit()

end subroutine msg_quit

! **********************************************************************
! **********************************************************************

!  msg_continue() print message or continue processing

subroutine msg_continue( msg)

! **********************************************************************

!  msg_continue() interface

! ----------------------------------------------------------------------

!  the warning or informational message

character( len= *), intent( in) :: msg

! **********************************************************************

!  msg_continue() local

! ----------------------------------------------------------------------

   character( len= conversion_len) :: number_str                     ! character line number

! **********************************************************************

!  msg_continue() text

! ----------------------------------------------------------------------

continue                                                             ! msg_continue()

! ----------------------------------------------------------------------

!  if file associated with this message

   file_msg: if( associated( current_file) )then                     ! if there is a current file

!  decode line number

      write( unit= number_str, fmt= conversion_fmt) current_file% lines_transfered

!  write message with file data

      write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( 'coco message: file: ' &
             // trim( current_file% name_str) // ', line: ' // trim( adjustl( number_str)) &
             // ': ' // msg)

!  if file associated not with this message

   else file_msg                                                     ! if there is a current file

!  write message without file data

      write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( 'coco message: ' // msg)

   endif file_msg                                                    ! if there is a current file

! ----------------------------------------------------------------------

!  msg_continue() exit

return                                                               ! msg_continue()

! **********************************************************************

!  msg_continue()

end subroutine msg_continue                                       

! **********************************************************************
! **********************************************************************

!  process_command_line() process command line

subroutine process_command_line

! **********************************************************************

!  process_command_line calls getopt() to get any options, then
!  process_command_line gets filenames from the command line

! **********************************************************************

!  default coco filenames constants

! **********************************************************************

!  input filename constants

! ----------------------------------------------------------------------

!  suffix used to construct input filename if one name is on the command line

   character( len= *), parameter :: input_suffix = dot // 'fpp'      ! read basename.fpp

! **********************************************************************

!  output filename constants

! ----------------------------------------------------------------------

!  suffix used to construct output filename if one name is on the command line

   character( len= *), parameter :: output_suffix = dot // 'f90'     ! write basename.f90

! **********************************************************************

!  setfile constants

! ----------------------------------------------------------------------

!  suffix used to construct setfile name if name is on the command line

   character( len= *), parameter :: set_suffix = dot // 'set'        ! setfile basename.set

! **********************************************************************

!  suffix length

   integer, parameter :: suffix_len = &                              ! must fit all suffixes
         max( len( input_suffix), len( output_suffix), len( set_suffix) )

! **********************************************************************

!  other command line constants

! ----------------------------------------------------------------------

!  coco usage (error message)

   character( len= *), parameter :: usage_msg = &                    ! print when command line error
      'usage: coco [ -V  | -h | [[opts] [ basename | output input [...]]]'

! **********************************************************************

!  coco communicate with getopt()

! ----------------------------------------------------------------------

!  valid option letters

#ifdef KEY /* bug 5087 */
   character( len= *), parameter :: opt_letters = 'E#;a:dD:efF:hiI:l:mnprsuvVw: '
#else /* KEY bug 5087 */
   character( len= *), parameter :: opt_letters = 'a:dD:efF:hiI:l:mnprsuvVw: '
#endif /* KEY bug 5087 */

! **********************************************************************

!  process_command_line local

! ----------------------------------------------------------------------

!  getopt() option letter

   integer :: optltr                                                 ! getopt return value

!  input filenames

   character( len= buffer_len) :: argword

!  dot divides basename and suffix

   integer :: basename_len                                           ! index of dot

!  loop through input filenames

   integer :: this_word

!  allocation status

   integer :: astat

! **********************************************************************

!  process_command_line() text

! ----------------------------------------------------------------------

continue                                                             ! process_command_line()

!  get number of command line args

   nargs = command_argument_count()                                  ! from module f2kcli

!  do until end of args is returned

   optltr = getopt( opt_letters)                                     ! try to get an option

!  process options

   cl_options: do while( optltr /= end_of_args)                      ! get '-*' until '-?'

!  select which option

      which_option: select case( char( optltr))                      ! process option

! ----------------------------------------------------------------------

!  set the alter state

      case( 'a') which_option                                        ! -a

         call process_alter_option( optarg)                          ! set alter state

!  turn off ?date? & ?time? editing

      case( 'd') which_option                                        ! -d

         options% edit_date = .false.                                ! process not date and time macros

!  declare a symbol

      case( 'D') which_option                                        ! -D name[=value]

         call process_symbol_option( optarg)                         ! declare a symbol

!  turn off source editing

      case( 'e') which_option                                        ! -e

         options% edit_source = .false.                              ! edit not source lines

!  turn off ?file? & ?line? editing

      case( 'f') which_option                                        ! -f

         options% edit_file = .false.                                ! process not file and line macros

!  write a documentation file

      case( 'F') which_option                                        ! -F filename

         doc_file% name_str = optarg                                 ! name is option string

         doc_file% named_file = .true.                               ! file now has a name

         call open_file( doc_file)                                   ! open the file

!  help

      case( 'h') which_option                                        ! -h

         call print_help                                             ! print help message

#ifdef KEY /* Bug 5087 */
if (options%integrated) call exit(0)
#endif /* KEY Bug 5087 */

         stop 'coco normal exit'                                     ! and quit

!  turn off ?integer? & ?logical? editing

      case( 'i') which_option                                        ! -i

         options% edit_integers = .false.                            ! process not integer and logical as macros

!  set directories to search for include files

      case( 'I') which_option                                        ! -I dir

         call process_include_option( optarg)                        ! go set up directory

!  set logfile (NOTE: optarg has len= filename_len, so no overflow can occur.)

      case( 'l') which_option                                        ! -l name

         log_file% logical_unit = log_unit                           ! no longer error_unit

         log_file% name_str = optarg                                 ! logfile name

         log_file% named_file = .true.                               ! file now has a name

         call open_file( log_file)                                   ! open the logfile

!  turn off ?macro? editing

      case( 'm') which_option                                        ! -m

         options% edit_macros = .false.                              ! process not macros

!  turn on line numbers

      case( 'n') which_option                                        ! -n

         options% number_source = .true.                             ! put "! file: line" on source lines

!  turn on (arg) checking

      case( 'p') which_option                                        ! -p

         options% args_in_parens = .true.                            ! warn when args not enclosed in parenthesis

!  turn on reporting extensions

      case( 'r') which_option                                        ! -r

         options% report_extensions = .true.                         ! report macros &c

!  turn off summary report

      case( 's') which_option                                        ! -s

         options% print_summary = .false.                            ! work silently

!  turn off undefined report

      case( 'u') which_option                                        ! -u

         options% warn_undeclared = .true.                           ! report setfile symbols not declared in source

!  turn on verbose

      case( 'v') which_option                                        ! -v

         options% verbose_mode = .true.                              ! report open/close files &c

!  print coco version data

      case( 'V') which_option                                        ! -V

         write( unit= error_unit, fmt= string_fmt) coco_rcs_id       ! print rcs id

#ifdef KEY /* Bug 5087 */
if (options%integrated) call exit(0)
#endif /* KEY Bug 5087 */

         stop 'coco normal exit'                                     ! and quit

!  turn off wrapping source output

      case( 'w') which_option                                        ! -w #

         call process_wrap_value( optarg)                            ! set wrap_length

#ifdef KEY /* Bug 5087 */
      case ('#') which_option
         options%integrated = .true.
	 ! If it's empty, we'll discover that later
	 set_file%name_str =  optarg

      case ('E') which_option
         options%write_stdout = .true.
#endif /* KEY Bug 5087 */

!  command line error

      case default which_option                                      ! any other

         write( unit= error_unit, fmt= string_fmt) usage_msg         ! complain

#ifdef KEY /* Bug 5087 */
! Bad usage should return error status code
if (options%integrated) call exit(1)
#endif /* KEY Bug 5087 */

         stop 'coco normal exit'                                     ! quit

      end select which_option                                        ! process option

! ----------------------------------------------------------------------

      optltr = getopt( opt_letters)                                  ! get next option

   enddo cl_options                                                  ! get -?

! ----------------------------------------------------------------------

!  the rest of the command line words (if any) must be file names

! ----------------------------------------------------------------------

!  number of command line args left unprocessed

   args_left: if( optarg == unknown_option )then                     ! unprocessed args

      number_of_names = nargs - optind                               ! number of args less seen so far

      optind = optind + 1                                            ! get next word

      no_more_args: if( number_of_names > 0 )then

         call get_command_argument( number= optind, value= optarg)   ! from module f2kcli

      endif no_more_args

   else args_left                                                    ! unprocessed args

      number_of_names = nargs - optind + 1                           ! found a filename

   endif args_left                                                   ! unprocessed args

#ifdef KEY /* Bug 5087 */
   check_dash_E: if (options%write_stdout .and. number_of_names /= 1) then
      call msg_quit( '-E allows only one filename')
   endif check_dash_E
#endif /* KEY Bug 5087 */

! ----------------------------------------------------------------------

!  process filenames

   filenames: select case( number_of_names)                          ! setup filenames

! ----------------------------------------------------------------------

!  one filename arg

   case( 1) filenames                                                ! setup filenames
#ifdef KEY /* Bug 5087 */
     write_stdout: if (options%write_stdout) then
	input_file% logical_unit = read_unit
	input_file% named_file = .true.
	input_file% name_str = trim(optarg)
     else write_stdout
#endif /* KEY Bug 5087 */

!  check that basename is not too long

	base_too_long: if( ( len_trim( optarg) + suffix_len) > filename_len )then

	   call msg_quit( 'filename too long: ' // trim( optarg) )     ! complain if so

	endif base_too_long                                            ! check that suffix can be added

!  use basename to make input filename

	input_file% logical_unit = read_unit                           ! connect input

	input_file% named_file = .true.                                ! has name

	input_file% name_str = trim( optarg) // input_suffix           ! make input name

!  use basename to make output filename

	output_file% logical_unit = write_unit                         ! connect output

	output_file% named_file = .true.                               ! has name

	output_file% name_str = trim( optarg) // output_suffix         ! make output name

!  use basename to make setfile filename

	set_file% logical_unit = set_unit                              ! connect setfile

	set_file% named_file = .true.                                  ! check name

	set_file% name_str = trim( optarg) // set_suffix               ! make set name

#ifdef KEY /* Bug 5087 */
     endif write_stdout
#endif /* KEY Bug 5087 */

! ----------------------------------------------------------------------

!  more than one filename arg

   case( 2: ) filenames                                              ! setup names

!  read source from read_unit

      input_file% logical_unit = read_unit                           ! must connect input

      input_file% named_file = .true.                                ! input is named

!  allocate source file list

      allocate( source_file_list( 2: number_of_names), stat= astat)

      alloc_error: if( astat > 0 )then

         call msg_quit( "can't allocate input file array")

      endif alloc_error

!  check that output name is not too long

      output_too_long: if( len_trim( optarg) > filename_len )then    ! name too long

         call msg_quit( 'output filename too long: ' // trim( optarg) )

      endif output_too_long                                          ! name too long

!  set up output file

      output_file% logical_unit = write_unit                         ! connect output

      output_file% named_file = .true.                               ! has name

      output_file% name_str = optarg                                 ! output file is arg 1

!  compute setfile name

#ifdef KEY /* Bug 5087 */
      ! Name from commandline has priority
      integrated: if (0 /= len(trim(set_file%name_str))) then

	set_file% named_file = .true.
	set_file% logical_unit = set_unit

      ! Don't compute setfile name based on output name if we're integrated
      ! with the Fortran front end, because output is a tempfile whose name
      ! is generated on the fly
      else if (.not. options%integrated) then integrated
#endif /* KEY Bug 5087 */
	basename_len = index( output_file% name_str, dot, back= .true.)

	no_dot: if( basename_len == 0 )then                            ! no dot in filename

	   basename_len = len_trim( output_file% name_str) + len( dot) ! entire string is basename

	endif no_dot                                                   ! no dot in filename

  !  check that setfile name is not too long

	set_too_long: if( basename_len + suffix_len > filename_len )then

	   call msg_quit( 'setfile name too long: ' // trim( output_file% name_str) // set_suffix )

	endif set_too_long                                             ! name too long

  !  set up setfile

	set_file% logical_unit = set_unit                              ! connect setfile

	set_file% named_file = .true.                                  ! check name

	set_file% name_str = output_file% name_str( : basename_len - len( dot)) // set_suffix

#ifdef KEY /* Bug 5087 */
      endif integrated
#endif /* KEY Bug 5087 */

!  record input files in source file list

      list_inputs: do this_word = 2, number_of_names

!  establish the components of this input file except the name

         source_file_list( this_word) = input_file

!  get next arg string

         optind = optind + 1

         call get_command_argument( number= optind, value= argword)  ! from module f2kcli

!  check that output name is not too long

         next_too_long: if( len_trim( argword) > filename_len )then  ! name too long

            call msg_quit( 'input filename too long: ' // trim( optarg) )

         endif next_too_long                                         ! name too long

         source_file_list( this_word)% name_str = argword

      enddo list_inputs

!  only possible values

   end select filenames                                              ! setup names

! ----------------------------------------------------------------------

!  process_command_line() exit

return                                                               ! process_command_line()

! **********************************************************************

!  process_command_line()

end subroutine process_command_line

! **********************************************************************
! **********************************************************************

!  getopt() return next known option from command line or unknown

integer function getopt( optstring)

! **********************************************************************

!  getopt() interface

! ----------------------------------------------------------------------

!  the string of valid option letters

character( len= *), intent( in) :: optstring

! **********************************************************************

!  getopt() constants

! ----------------------------------------------------------------------

!  special characters

   character( len= *), parameter :: dash = '-'                       ! starts an option

   character( len= *), parameter :: colon = ':'                      ! if option must have a word

! **********************************************************************

!  getopt() local

! ----------------------------------------------------------------------

!  argument buffer

   character( len= filename_len) :: optword                          ! from get_command_argument()

!  index in optstring

   integer :: index_optstring                                        ! which option

! **********************************************************************

!  getopt() text

continue                                                             ! getopt()

! ----------------------------------------------------------------------

!  initialize for next option

   check_inc: if( optind >= nargs )then                              ! if no unread options

      optarg = unknown_option                                        ! complain
      getopt = end_of_args                                           ! signal end of options

      return                                                         ! quit

   endif check_inc                                                   ! if no unread options

! ----------------------------------------------------------------------

!  get next option

   optind = optind + 1                                               ! next arg to get

   call get_command_argument( number= optind, value= optword)        ! from module f2kcli

!  if word is not -?

   not_an_option: if( optword( 1: 1) /= dash )then                   ! if word is not an option

      optarg = optword                                               ! return word
      getopt = end_of_args                                           ! signal end of options

      return                                                         ! quit

!  if word is --

   elseif( optword( 2: 2) == dash )then not_an_option                ! if --

      optarg = unknown_option                                        ! return --
      getopt = end_of_args                                           ! signal end of options

      return                                                         ! quit

   endif not_an_option

! ----------------------------------------------------------------------

!  optword is -x (not --)

   index_optstring = index( optstring, optword( 2: 2))               ! find optletter in string

   is_opt: if( index_optstring > 0 )then                             ! if found in optstring

!  if this optltr must have another word

      opt_string: if( optstring( index_optstring + 1: index_optstring + 1) == colon )then

!  it can be separated by a blank

         next_word: if( optword( 3: 3) == blank )then                ! separate words or one word

            optind = optind + 1                                      ! increment index
            call get_command_argument( number= optind, value= optarg)

!  or not be separated by a blank

         else next_word                                              ! separate words or one word

            optarg = optword( 3: )                                   ! option field is rest of word

         endif next_word                                             ! separate words or one word

      endif opt_string

#ifdef KEY /* Bug 5087 */
      ! The -#setfile option doesn't have a blank between # and setfile, and
      ! must not consume the next argument if setfile is the empty string
      nonextarg: if( optstring( index_optstring + 1: index_optstring + 1) == ';' )then
        optarg = optword( 3: )
      endif nonextarg
#endif /* KEY Bug 5087 */

      getopt = ichar( optword( 2: 2))                                ! option found

!  if this optltr must not have another word

   else is_opt                                                       ! if not found in optstring

      optarg = optword                                               ! return word and
      getopt = ichar( unknown_option)                                ! complain

   endif is_opt                                                      ! if found/not found in optstring

! ----------------------------------------------------------------------

!  getopt() exit

return                                                               ! getopt()

! **********************************************************************

!  getopt()

end function getopt                                               

! **********************************************************************
! **********************************************************************

!  %%% process particular command line options

! **********************************************************************
! **********************************************************************

!  process_alter_option() process alter arguments

subroutine process_alter_option( alter_opt)

! **********************************************************************

!  process_alter_option() interface

! ----------------------------------------------------------------------

!  the alter option from the command line

character( len= *), intent( in) :: alter_opt                        

! **********************************************************************

!  entry: alter_opt is command line arg following -a
!         "d" | "b" | "0" | "1" | "3"

!  exit: alter_opt is processed or error exit

! **********************************************************************

!  process_alter_option() constants

! ----------------------------------------------------------------------

!  possible alter option strings

   character( len= *), parameter :: delete_str = 'd'                 ! delete line

   character( len= *), parameter :: blank_str = 'b'                  ! blank line

   character( len= *), parameter :: shift0_str = '0'                 ! shift zero

   character( len= *), parameter :: shift1_str = '1'                 ! shift one

   character( len= *), parameter :: shift3_str = '3'                 ! shift three

! **********************************************************************

!  process_alter_option() local

! ----------------------------------------------------------------------

!  decoding the option is done in lower case which may require a case change

   character( len= 1) :: lower_case_opt

! **********************************************************************

!  process_alter_option() text

continue                                                             ! process_alter_option()

! ----------------------------------------------------------------------

!  check for unknown option

   too_long: if( len_trim( alter_opt) > 1 )then                      ! must have -a.

      call msg_quit( "unknown -a option: " // trim( alter_opt) )

   endif too_long                                                    ! must have -a.

!  force arg to lower case

   fix_case: select case( alter_opt( 1: 1))                          ! only first character is meaningful

   case( 'A': 'Z') fix_case                                          ! upper case

      lower_case_opt = achar( iachar( alter_opt( 1: 1)) + change_case)

   case default fix_case                                             ! otherwise

      lower_case_opt = alter_opt                                     ! copy

   end select fix_case                                               ! first character

! ----------------------------------------------------------------------

!  legal alter argument or error

   alter_value_str: select case( lower_case_opt)                     ! which alter state

!  alter delete

   case( delete_str) alter_value_str                                 ! delete string

      options% alter_state = alter_delete                            ! set state to delete

!  alter blank

   case( blank_str) alter_value_str                                  ! blank string

      options% alter_state = alter_blank                             ! set state to blank

!  alter shift1

   case( shift1_str) alter_value_str                                 ! shift1 string

      options% alter_state = alter_shift_1                           ! set state to shift 1

!  alter shift0

   case( shift0_str) alter_value_str                                 ! shift0 string

      options% alter_state = alter_shift_0                           ! set state to shift 0

!  alter shift3

   case( shift3_str) alter_value_str                                 ! shift3 string

      options% alter_state = alter_shift_3                           ! set state to shift 3

!  unknown alter code

   case default alter_value_str                                      ! some other string

      call msg_quit( "unknown -a option: " // trim( alter_opt) )     ! not in { b, d, 0, 1, 3 }

!  legal alter statement or error

   end select alter_value_str                                        ! which alter state

! ----------------------------------------------------------------------

!  process_alter_option() exit

return                                                               ! process_alter_option()

! **********************************************************************

!  process_alter_option()

end subroutine process_alter_option

! **********************************************************************
! **********************************************************************

!  process_symbol_option() process define arguments

subroutine process_symbol_option( symbol_opt)

! **********************************************************************

!  process_symbol_option() interface

! ----------------------------------------------------------------------

!  the symbol string from the command line

character( len= *), intent( in) :: symbol_opt                       

! **********************************************************************

!  entry: symbol_opt is string following -D { log | int=val }

!  exit: symbol_opt is processed or error exit

! **********************************************************************

!  process_symbol_option() constants

! ----------------------------------------------------------------------

   character( len= *), parameter :: log_value_str = '=.true.'        ! -D logicals are true

! **********************************************************************

!  process_symbol_option() local

! ----------------------------------------------------------------------

!  find characters

   integer :: next_char                                              ! point to character

!  construct a declaration string to process

   character( len= filename_len) :: decl_str                         ! words are filename long

! **********************************************************************

!  process_symbol_option() text

continue                                                             ! process_symbol_option()

! ----------------------------------------------------------------------

!  force names to lower case

   each_char: do next_char = 1, len( symbol_opt)

      to_lower: select case( symbol_opt( next_char: next_char))

      case( 'A': 'Z') to_lower

         decl_str( next_char: next_char) = achar( iachar( symbol_opt( next_char: next_char)) + change_case)

      case default to_lower

         decl_str( next_char: next_char) = symbol_opt( next_char: next_char)

      end select to_lower

   enddo each_char

! ----------------------------------------------------------------------

!  an equal sign must separate a value from the name

   next_char = index( decl_str, equals)                              ! find equals

!  if there's an equals, it's an integer

   int_or_log: if( next_char > 0 )then                               ! got equals

!  declare the integer constant

      call process_integer_constant( decl_str)                       ! process as if from setfile

!  if there's no equals, it's a logical ( = .true.)

   else int_or_log                                                   ! got equals

!  construct the logical (it's true)

      decl_str = trim( decl_str) // log_value_str                    ! logical constant string

!  declare the logical constant

      call process_logical_constant( decl_str)                       ! process as if from setfile

!  integer or logical

   endif int_or_log                                                  ! got equals

! ----------------------------------------------------------------------

!  if reporting use of extensions

   extensions: if( options% report_extensions )then                  ! extensions

      call msg_continue( "defined symbol from command line: " // trim( symbol_opt))

   endif extensions                                                  ! extensions

! ----------------------------------------------------------------------

!  process_symbol_option() exit

return                                                               ! process_symbol_option()

! **********************************************************************

!  process_symbol_option()

end subroutine process_symbol_option

! **********************************************************************
! **********************************************************************

!  print_help() write summary report to specified unit

subroutine print_help

! **********************************************************************

!  entry: in response to -h command line option

!  exit: print help message

! **********************************************************************

!  print_help() constants

! ----------------------------------------------------------------------

!  the help message

   character( len= *), dimension( 20), parameter :: help_msg = (/ &
                      ' -a ?           set alter state, ? = { b, d, 0, 1, 3}       ', &
                      ' -d             turn off ?date? & ?time? editing            ', &
                      ' -D name[=n]    declare integer or logical constant         ', &
                      ' -e             turn off all source editing                 ', &
                      ' -f             turn off ?file? & ?line? editing            ', &
                      ' -F file        write documentation text to file            ', &
                      ' -h             print this help message and quit            ', &
                      ' -i             turn off ?integer? & ?logical? editing      ', &
                      ' -I dir         search dir for include files (after .)      ', &
                      ' -l file        write log messages to file (default stderr) ', &
                      ' -m             turn off ?macro? editing                    ', &
                      ' -n             print line numbers on source lines          ', &
                      ' -p             warn when actual args might need parenthesis', &
                      ' -r             report all use of extensions                ', &
                      ' -s             work silently, print no summary             ', &
                      ' -u             report setfile symbols undefined in source  ', &
                      ' -v             report file opening and closing             ', &
                      ' -V             print coco version and quit                 ', &
                      ' -w n           wrap lines to n columns (0= off, 72= fixed) ', &
                      ' --             optionally separate options from file names ' /)

! **********************************************************************

!  print_help() local

! ----------------------------------------------------------------------

!  implied do variable

   integer :: do_idx                                                 ! print_help()

! **********************************************************************

!  print_help() text

! ----------------------------------------------------------------------

continue                                                             ! print_help()

! ----------------------------------------------------------------------

   write( unit= error_unit, fmt= string_fmt) ( trim( help_msg( do_idx)), do_idx= 1, size( help_msg))

! ----------------------------------------------------------------------

!  print_help() exit

return                                                               ! print_help()

! **********************************************************************

!  print_help()

end subroutine print_help

! **********************************************************************
! **********************************************************************

!  process_include_option() process include directory options

subroutine process_include_option( directory_opt)

! **********************************************************************

!  process_include_option() interface

! ----------------------------------------------------------------------

!  the directory string from the command line

character( len= *), intent( in) :: directory_opt                   

! **********************************************************************

!  entry: directory_opt is a directory to be added to the list
!         of directories to be searched for inlcude files

!  exit: directory_opt is on the list

! **********************************************************************

!  process_include_option() local

! ----------------------------------------------------------------------

!  point to a directory type

   type( path_t), pointer :: path_ptr                                ! add directory to list

!  unquote the directory name if needed

   character( len= filename_len) :: directory_str

!  lengths of quoted string

   integer :: quoted_len

!  lengths of unquoted string

   integer :: unquoted_len

! **********************************************************************

!  process_include_option() text

continue                                                             ! process_include_option()

! ----------------------------------------------------------------------

!  if the directory is quoted, unquote it

   is_quoted: select case( directory_opt( 1: 1) )

   case( single_quote, double_quote) is_quoted

      call unquote_string( directory_opt, directory_str, quoted_len, unquoted_len)

      null_unquoted: if( quoted_len == 0 .or. unquoted_len == 0 )then

         call msg_quit( "null name passed to -I option")

      endif null_unquoted

   case default is_quoted

      directory_str = directory_opt

   end select is_quoted

! ----------------------------------------------------------------------

!  if name is already on the path

   nullify( path_ptr)                                                ! prepare pointer for use

   call seek_directory( directory_str, path_ptr)                     ! check the path

   on_list_or_add: if( associated( path_ptr) )then                   ! already on path

      call msg_continue( "redundant include directory ignored: " // trim( directory_opt) )

   else on_list_or_add                                               ! not on path

      call add_directory( directory_str)                             ! so add it

!  if reporting use of extensions

      extensions: if( options% report_extensions )then               ! extensions

         call msg_continue( "added include directory from command line: " // trim( directory_opt) )

      endif extensions                                               ! extensions

   endif on_list_or_add                                              ! already on path or not

! ----------------------------------------------------------------------

!  process_include_option() exit

return                                                               ! process_include_option()

! **********************************************************************

!  process_include_option()

end subroutine process_include_option

! **********************************************************************
! **********************************************************************

!  write_options() write summary report to specified unit

subroutine write_options

! **********************************************************************

!  write_options() constants

! ----------------------------------------------------------------------

!  possible alter states

integer, parameter :: lower_alter = min( alter_delete, alter_blank, alter_shift_1, alter_shift_0, alter_shift_3)

integer, parameter :: upper_alter = max( alter_delete, alter_blank, alter_shift_1, alter_shift_0, alter_shift_3)

!  possible alter state labels

character( len= 16), dimension( lower_alter: upper_alter), parameter :: alter_labels = (/ &
                                       'deleted         ', &
                                       'blank line      ', &
                                       'initial !       ', &
                                       'shifted 1 + !   ', &
                                       'shifted 3 + !?> ' /)

! **********************************************************************

!  write_options() local

! ----------------------------------------------------------------------

!  construct output lines

   character( len= source_line_len) :: output_line

! **********************************************************************

!  write_options() text

! ----------------------------------------------------------------------

continue                                                             ! write_options()

! ----------------------------------------------------------------------

!  write a header

   write( unit= log_file% logical_unit, fmt= log_file% format_str) "coco options:"

! ----------------------------------------------------------------------

!  identify the alter state

   check_index: select case( options% alter_state)

   case( lower_alter: upper_alter) check_index

      output_line = 'alter state causes lines to be ' // alter_labels( options% alter_state)

   case default check_index

      output_line = 'alter state is undefined'

   end select check_index

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether editing date & time

   edit_date_time: if( options% edit_date )then

      output_line = 'editing ' // date_str // ' and ' // time_str // ' strings'

   else edit_date_time

      output_line = 'not editing ' // date_str // ' and ' // time_str // ' strings'

   endif edit_date_time

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether editing file & line

   edit_file_line: if( options% edit_file )then

      output_line = 'editing ' // file_str // ' and ' // line_str // ' strings'

   else edit_file_line

      output_line = 'not editing ' // file_str // ' and ' // line_str // ' strings'

   endif edit_file_line

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether editing at all

   edit_control: if( options% edit_source )then

      output_line = 'editing source lines'

   else edit_control

      output_line = 'not editing source lines'

   endif edit_control

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether editing integers and logicals

   edit_ints_logs: if( options% edit_integers )then

      output_line = 'editing integer and logicals'

   else edit_ints_logs

      output_line = 'not editing integer and logicals'

   endif edit_ints_logs

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether editing macros

   edit_macro: if( options% edit_macros )then

      output_line = 'editing macros'

   else edit_macro

      output_line = 'not editing macros'

   endif edit_macro

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether warning whether args have enclosing parens

   edit_parens: if( options% args_in_parens )then

      output_line = 'warning when macro & text actual args are not enclosed in parenthesis'

   else edit_parens

      output_line = 'not warning when macro & text actual args are not enclosed in parenthesis'

   endif edit_parens

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether reporting extensions

   rpt_exten: if( options% report_extensions )then

      output_line = 'reporting coco extensions'

   else rpt_exten

      output_line = 'not reporting coco extensions'

   endif rpt_exten

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether printing coco summary

   rpt_prt: if( options% print_summary )then

      output_line = 'printing coco report'

   else rpt_prt

      output_line = 'not printing coco report'

   endif rpt_prt

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether printing "! file: line" on source lines

   rpt_number: if( options% number_source )then

      output_line = 'numbering source lines'

   else rpt_number

      output_line = 'not numbering source lines'

   endif rpt_number

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether warning when symbol is in setfile but not source

   rpt_warn: if( options% warn_undeclared )then

      output_line = 'warning when setfile symbols not in source'

   else rpt_warn

      output_line = 'not warning when setfile symbols not in source'

   endif rpt_warn

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether verbose mode is on

   rpt_verbose: if( options% verbose_mode )then

      output_line = 'verbose mode is on'

   else rpt_verbose

      output_line = 'verbose mode is off'

   endif rpt_verbose

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line)

!  identify whether verbose mode is on

   rpt_wrap: if( options% wrap_length /= wrap_off )then

      output_line = 'wrapping source lines at length '

   else rpt_wrap

      output_line = 'not wrapping source lines'

   endif rpt_wrap

   write( unit= log_file% logical_unit, fmt= log_file% format_str) trim( output_line) // blank, options% wrap_length

! ----------------------------------------------------------------------

!  write_options() exit

return                                                               ! write_options()

! **********************************************************************

!  write_options()

end subroutine write_options

! **********************************************************************
! **********************************************************************

!  write_report() write summary report to specified unit

subroutine write_report

! **********************************************************************

!  write_report() local

! ----------------------------------------------------------------------

!  print date and time in header

   character( len= 8) :: today                                       ! today's date

   character( len= 10) :: now                                        ! now's time

! ----------------------------------------------------------------------

!  print include path

   type( path_t), pointer :: path_ptr                                ! search include path

!  search lists for symbols not defined in the coco program proper

   type( symbol_t), pointer :: symbol_ptr                            ! search symbol list

! ----------------------------------------------------------------------

!  print input files

   integer :: this_file                                              ! search include path

! **********************************************************************

!  write_report() text

! ----------------------------------------------------------------------

continue                                                             ! write_report()

! ----------------------------------------------------------------------

!  banner includes the date and time

   call date_and_time( date= today, time= now)

   write( unit= log_file% logical_unit, fmt= log_file% format_str) 'coco executed: ' // today // blank // now

! ----------------------------------------------------------------------

!  identify the setfile

   write( unit= log_file% logical_unit, fmt= log_file% format_str) 'setfile: ' // trim( set_file% name_str)

! ----------------------------------------------------------------------

!  identify the output file

   write( unit= log_file% logical_unit, fmt= log_file% format_str) 'output: ' // trim( output_file% name_str)

! ----------------------------------------------------------------------

!  identify the input file(s)

   one_or_more: if( allocated( source_file_list) )then

      write( unit= log_file% logical_unit, fmt= log_file% format_str, advance= 'NO') 'input:'

      more_than_one: do this_file = 2, number_of_names

         write( unit= log_file% logical_unit, fmt= log_file% format_str, advance= 'NO') &
                blank // trim( source_file_list( this_file)% name_str)

      enddo more_than_one

      write( unit= log_file% logical_unit, fmt= log_file% format_str)

   else one_or_more

      write( unit= log_file% logical_unit, fmt= log_file% format_str) 'input: ' // trim( input_file% name_str)

   endif one_or_more

! ----------------------------------------------------------------------

!  identify the document file, if there is one

   got_doc: if( doc_file% named_file )then

      write( unit= log_file% logical_unit, fmt= log_file% format_str) &
         'docfile: ' // trim( doc_file% name_str), doc_file% lines_transfered

   endif got_doc

! ----------------------------------------------------------------------

!  identify the include path

   write( unit= log_file% logical_unit, fmt= log_file% format_str, advance= 'NO') 'include path: .'

   nullify( path_ptr)                                                ! prepare pointer for use

   path_ptr => first_directory                                       ! initialize loop

   inc_path: do while( associated( path_ptr) )                       ! include path

      write( unit= log_file% logical_unit, fmt= directory_fmt, advance= 'NO') &
             blank // trim( path_ptr% name_str) // open_paren, path_ptr% times_accessed, close_paren

      path_ptr => path_ptr% next                                     ! check next directory

   enddo inc_path                                                    ! include path

!  end line using null string

   write( unit= log_file% logical_unit, fmt= log_file% format_str)

! ----------------------------------------------------------------------

!  if undefined symbols report is requested

   undefined_complaints: if( options% warn_undeclared )then          ! request undefined report

!  complain about any integers or logicals declared in the setfile but not in source

      nullify( symbol_ptr)                                           ! prepare pointer for use

      symbol_ptr => first_symbol                                     ! search list

      search_syms: do while( associated( symbol_ptr))                ! while elements to check

         found_sym: if( symbol_ptr% predefined )then                 ! check element

            write( unit= log_file% logical_unit, fmt= log_file% format_str) &
               'symbol declared in setfile but not in any source file: ' // trim( symbol_ptr% name_str)

         endif found_sym                                             ! check element

         symbol_ptr => symbol_ptr% next                              ! check next element

      enddo search_syms                                              ! while elements to check

   endif undefined_complaints                                        ! request undefined report

! ----------------------------------------------------------------------

!  number of files read

   write( unit= log_file% logical_unit, fmt= log_file% format_str) &
          'files read: ', total% input_files

   write( unit= log_file% logical_unit, fmt= log_file% format_str) &
          'include files read: ', total% include_files

!  number of setfile lines read

   write( unit= log_file% logical_unit, fmt= log_file% format_str) &
          'set lines read: ', set_file% lines_transfered

!  number of coco lines read

   write( unit= log_file% logical_unit, fmt= log_file% format_str) &
          'coco lines read: ', total% coco_lines

!  number of source lines read

   write( unit= log_file% logical_unit, fmt= log_file% format_str) &
          'source lines read: ', total% input_lines

!  number of lines written

   write( unit= log_file% logical_unit, fmt= log_file% format_str) &
          'source lines written: ', output_file% lines_transfered

!  number of selected lines written

   write( unit= log_file% logical_unit, fmt= log_file% format_str) &
          'selected source lines: ', total% selected_lines

!  number of elided lines

   write( unit= log_file% logical_unit, fmt= log_file% format_str) &
          'elided source lines: ', total% elided_lines

!  number of text blocks read

   write( unit= log_file% logical_unit, fmt= log_file% format_str) &
          'text blocks read: ', total% text_blocks

!  number of text lines read

   write( unit= log_file% logical_unit, fmt= log_file% format_str) &
          'text lines read: ', total% text_lines

!  number of text lines written

   write( unit= log_file% logical_unit, fmt= log_file% format_str) &
          'text lines written: ', total% copied_lines

! ----------------------------------------------------------------------

!  write_report() exit

return                                                               ! write_report()

! **********************************************************************

!  write_report()

end subroutine write_report

! **********************************************************************
! **********************************************************************

!  process_wrap_value() set the wrap length option length

subroutine process_wrap_value( number_str)

! **********************************************************************

!  process_wrap_value() interface

! ----------------------------------------------------------------------

!  the wrap length string from the command line

character( len= *), intent( in) :: number_str

! **********************************************************************

!  process_wrap_value() local

! ----------------------------------------------------------------------

!  index of a non-digit

   integer :: char_idx                                               ! seek bad character

!  convert string to characters

   character( len= conversion_len) :: conversion_str

!  test proposed wrap length

   integer :: wrap_len

! **********************************************************************

!  process_wrap_value() text

! ----------------------------------------------------------------------

continue                                                             ! process_wrap_value()

! ----------------------------------------------------------------------

!  legal wrap string value or error

   bad_char: if( char_idx > 0 )then

      char_idx = verify( number_str, digit_chars)                       ! scan for a non-digit

      call msg_quit( "bad character in wrap length: " // trim( number_str))

   endif bad_char

! ----------------------------------------------------------------------

!  convert chacacters to integer

   conversion_str = number_str                                       ! convert ten characters

   conversion_str = adjustr( conversion_str)

   read( unit= conversion_str, fmt= conversion_fmt) wrap_len         ! decode string

! ----------------------------------------------------------------------

!  check proposed wrap length

   set_within_bounds: select case( wrap_len)                         ! 0 or 72-132

!  do no wrapping

   case( 0) set_within_bounds                                        ! 0 means

      options% wrap_length = wrap_off                                ! no wrapping

!  within bounds so accept as if

   case( fixed_format_len: free_format_len) set_within_bounds        ! valid as is

      options% wrap_length = wrap_len                                ! do wrapping

!  out of bounds so set within bounds

   case default set_within_bounds                                    ! out of bounds

      call msg_continue( "invalid wrap length set within bounds: " // trim( number_str))

      options% wrap_length = min( free_format_len, max( fixed_format_len, wrap_len))

   end select set_within_bounds                                      ! 0 or 72-132

! ----------------------------------------------------------------------

!  process_wrap_value() exit

return                                                               ! process_wrap_value()

! **********************************************************************

!  process_wrap_value()

end subroutine process_wrap_value

! **********************************************************************
! **********************************************************************

!  %%% seek and process the setfile (if any)

! **********************************************************************
! **********************************************************************

!  seek_setfile() write summary report to specified unit

subroutine seek_set_file                                            

! **********************************************************************

!  seek_set_file constants

! ----------------------------------------------------------------------

!  default set_file name

   character( len= *), parameter :: default_name = 'coco.set'        ! if named setfile not found

! **********************************************************************

!  seek_set_file() text

! ----------------------------------------------------------------------

continue                                                             ! seek_set_file()

! ----------------------------------------------------------------------

!  see if there is name for the set_file

   named_set_file: if( set_file% named_file )then                    ! if named file expected

!  inquire by file looking for named setfile

      inquire( file= set_file% name_str, exist= set_file% named_file, iostat= set_file% io_status)

      inq_named: if( set_file% io_status > 0 )then                   ! inquire error

         call msg_quit( "can't inquire setfile: " // trim( set_file% name_str))

      endif inq_named                                                ! inquire error

!  done checking setfile name

   endif named_set_file                                              ! there is a (possible) setfile

! ----------------------------------------------------------------------

!  no named setfile so try to find the default setfile

   default_set_file: if( .not. set_file% named_file )then            ! try the default

!  inquire by file looking for default setfile

      inquire( file= default_name, exist= set_file% named_file, iostat= set_file% io_status)

      inq_default: if( set_file% io_status > 0 )then                 ! inquire error

         call msg_quit( "can't inquire default setfile: " // default_name)

      endif inq_default                                              ! inquire error

!  if found the default setfile ensure the variable correctly specifies it

      use_default: if( set_file% named_file )then                    ! prepare setfile

         set_file% logical_unit = set_unit                           ! use a valid unit

         set_file% name_str = default_name                           ! use the default name

      endif use_default                                              ! prepare setfile

   endif default_set_file                                            ! try the default

! ----------------------------------------------------------------------

!  if have setfile, open it, process it, close it

   read_set_file: if( set_file% named_file )then                     ! if setfile exists

      call process_set_file                                          ! read setfile

   else read_set_file                                                ! if setfile exists

      set_file% name_str = '<no setfile>'                            ! mark no setfile

   endif read_set_file                                               ! if setfile exists

   processing_set_file = .false.                                     ! no longer processing setfile (or command line)

! ----------------------------------------------------------------------

!  seek_set_file() exit

return                                                               ! seek_set_file()

! **********************************************************************

!  seek_set_file()

end subroutine seek_set_file

! **********************************************************************
! **********************************************************************

!  process_sefile() open, process, close the coco setfile

subroutine process_set_file                                         

! **********************************************************************

!  process_set_file() steps

!  1. open the setfile
!  2. open the set scratch file
!  2. read the setfile line by line
!  3. call blank_compress_lower_case() to construct a coco statement
!  4. ignore coco comments
!  5. call process_set_statement() to process coco set statement
!  6. close setfile

! **********************************************************************

!  process_set_file() local

! ----------------------------------------------------------------------

!  process the setfile statement by statement

   character( len= buffer_len) :: set_statement                      ! statements of the setfile

! ----------------------------------------------------------------------

!  signal complete statement

   logical :: complete                                               ! true when whole statement read

! **********************************************************************

!  process_set_file() text

continue                                                             ! process_set_file()

! ----------------------------------------------------------------------

!  open the setfile for reading

   call open_file( set_file)                                         ! and link current_file

! ----------------------------------------------------------------------

!  count files processed

   total% input_files = total% input_files + 1                       ! one file per process_set_file()

!  as if finished a complete statement at beginning of file

   complete = .true.                                                 ! at beginning of file

! ----------------------------------------------------------------------

!  main read setfile lines loop

   read_lines: do                                                    ! read setfile line by line

! ----------------------------------------------------------------------

!  read a setfile line

      read( unit= set_file% logical_unit, fmt= set_file% format_str, iostat= set_file% io_status) set_file% line

      read_set: if( set_file% io_status > 0 )then                    ! read error

         call msg_quit( "can't read setfile: " // trim( set_file% name_str))

      endif read_set                                                 ! read error

! ----------------------------------------------------------------------

!  read until end of file

      read_eof: if( set_file% io_status < 0 )then                    ! eof

!  reset statement processing for the next file

         call blank_compress_lower_case( set_statement, null_string) ! reset for next file

!  if in a statement continuation sequence

         premature_eof: if( .not. complete )then                     ! in continuation sequence

            call msg_quit( "end of file encountered within a continuation sequence")

         endif premature_eof                                         ! in continuation sequence

!  exit the read lines loop

         exit read_lines                                             ! read the last line

      endif read_eof                                                 ! eof

!  count setfile lines

      set_file% lines_transfered = set_file% lines_transfered + 1    ! one more line

! ----------------------------------------------------------------------

!  process setfile lines or error if source lines

      coco_line: if( line( : len( coco_key)) == coco_key )then       ! coco lines start with '??'

!  count coco lines

         total% coco_lines = total% coco_lines + 1                   ! one more coco line

!  process setfile lines, ignore coco comments

         coco_statement: if( is_coco_statement( line( len( coco_key) + 1: )) )then

! ----------------------------------------------------------------------

!  read a complete statement line by line

            call gather_coco_statement( line, set_statement, complete)

!  if not yet a complete statement go get the rest of it

            get_statement: if( .not. complete )then                  ! if not yet a complete statement

               cycle read_lines                                      ! read another line

            endif get_statement                                      ! if not yet a complete statement

!  process the complete setfile statement

            call process_set_statement( set_statement)               ! process statement

!  process setfile lines, ignore coco comments

         endif coco_statement                                        ! non-comment line

!  source line in setfile

      else coco_line                                                 ! error

         call msg_quit( "source lines are not allowed in the setfile")

!  end processing set statements

      endif coco_line                                                ! setfile statements

! ----------------------------------------------------------------------

!  end main read setfile lines loop

   enddo read_lines                                                  ! read setfile line by line

   total% input_lines = total% input_lines + set_file% lines_transfered

! ----------------------------------------------------------------------

!  close the setfile

   call close_file( set_file)                                        ! and unlink current_file

! ----------------------------------------------------------------------

!  process_set_file() exit

return                                                               ! process_set_file()

! **********************************************************************

!  process_set_file()

end subroutine process_set_file

! **********************************************************************
! **********************************************************************

!  copy_set_file() copy setfile to output file

subroutine copy_set_file                                             

! **********************************************************************

!  copy_set_file() text

! ----------------------------------------------------------------------

continue                                                             ! copy_set_file()

! ----------------------------------------------------------------------

!  open the set file

   call open_file( set_file)                                         ! and link current_file

! ----------------------------------------------------------------------

!  copy each line

   copy_lines: do                                                    ! line by line

!  read a line

      read( unit= set_file% logical_unit, fmt= set_file% format_str, &
            iostat= set_file% io_status) set_file% line

      read_set_file: if( set_file% io_status > 0 )then               ! read error

         call msg_quit( "can't copy setfile")

      endif read_set_file                                            ! read error

!  read entire scratch file

      set_eof: if( set_file% io_status < 0 )then                     ! exit at end of file

         exit copy_lines                                             ! end of setfile

      endif set_eof                                                  ! exit at end of file

!  write a line

      call write_coco_line( output_file)                             ! to output

   enddo copy_lines                                                  ! line by line

! ----------------------------------------------------------------------

!  close the setfile

   call close_file( set_file)                                        ! and unlink current_file

! ----------------------------------------------------------------------

!  copy_set_file() exit

return                                                               ! copy_set_file()

! **********************************************************************

!  copy_set_file()

end subroutine copy_set_file

! **********************************************************************
! **********************************************************************

!  %%% process statements many of which may appear only in the setfile

! **********************************************************************
! **********************************************************************

!  process_set_statement() process set line

subroutine process_set_statement( set_stmt)

! **********************************************************************

!  process_set_statement() interface

! ----------------------------------------------------------------------

!  the statement string from the set file

character( len= *), intent( in) :: set_stmt                       

! **********************************************************************

!  entry: set_stmt is blank_compress_lower_case set statement past the coco key
!         "alter:..." | "integer..." | "logical..." | "directory'...'" |
!         "wrap:..." | "edit:..." | "warn:..." | "logfile'...'" |
!         "summary:..." | "verbose:..." | "doc'...'" | "parens:..."

!  exit: set_stmt is processed or error exit

! **********************************************************************

!  process_set_statement() text

continue                                                             ! process_set_statement()

! ----------------------------------------------------------------------

!  catergorize setfile statement: alter, integer, logical, directory, wrap

! ----------------------------------------------------------------------

!  if the directive is an alter directive

   which_directive: if( set_stmt( : len( alter_str)) == alter_str )then

      call process_alter_directive( set_stmt( len( alter_str) + 1: ) )

! ----------------------------------------------------------------------

!  if the directive is a setfile integer declaration

   elseif( set_stmt( : len( integer_str)) == integer_str )then which_directive

      call process_integer_declaration( set_stmt( len( integer_str) + 1: ) )

! ----------------------------------------------------------------------

!  integer constant declaration

   elseif( set_stmt( : len( integer_constant_str)) == integer_constant_str )then which_directive

      call process_integer_constant( set_stmt( len( integer_constant_str) + 1: ))

! ----------------------------------------------------------------------

!  if the directive is a setfile logical declaration

   elseif( set_stmt( : len( logical_str)) == logical_str )then which_directive

      call process_logical_declaration( set_stmt( len( logical_str) + 1: ) )

! ----------------------------------------------------------------------

!  logical constant declaration

   elseif( set_stmt( : len( logical_constant_str)) == logical_constant_str )then which_directive

      call process_logical_constant( set_stmt( len( logical_constant_str) + 1: ))

! ----------------------------------------------------------------------

!  if the directive is a setfile directory directive

   elseif( set_stmt( : len( directory_str)) == directory_str )then which_directive

      call process_directory_directive( set_stmt( len( directory_str) + 1: ) )

! ----------------------------------------------------------------------

!  if the directive is a setfile docfile directive

   elseif( set_stmt( : len( docfile_str)) == docfile_str )then which_directive

      call process_docfile_directive( set_stmt( len( docfile_str) + 1: ) )

! ----------------------------------------------------------------------

!  if the directive is a setfile logfile directive

   elseif( set_stmt( : len( logfile_str)) == logfile_str )then which_directive

      call process_logfile_directive( set_stmt( len( logfile_str) + 1: ) )

! ----------------------------------------------------------------------

!  if the directive is a setfile wrap directive

   elseif( set_stmt( : len( wrap_str)) == wrap_str )then which_directive

      call process_wrap_directive( set_stmt( len( wrap_str) + 1: ) )

! ----------------------------------------------------------------------

!  if the directive is a setfile edit directive

   elseif( set_stmt( : len( edit_str)) == edit_str )then which_directive

      call process_edit_directive( set_stmt( len( edit_str) + 1: ) )

! ----------------------------------------------------------------------

!  if the directive is a setfile summary directive

   elseif( set_stmt( : len( summary_str)) == summary_str )then which_directive

      call process_summary_directive( set_stmt( len( summary_str) + 1: ) )

! ----------------------------------------------------------------------

!  if the directive is a setfile number: directive

   elseif( set_stmt( : len( number_str)) == number_str )then which_directive

      call process_number_directive( set_stmt( len( number_str) + 1: ) )

! ----------------------------------------------------------------------

!  if the directive is a setfile parens directive

   elseif( set_stmt( : len( parens_str)) == parens_str )then which_directive

      call process_paren_directive( set_stmt( len( parens_str) + 1: ) )

! ----------------------------------------------------------------------

!  if the directive is a setfile warn directive

   elseif( set_stmt( : len( warn_str)) == warn_str )then which_directive

      call process_warn_directive( set_stmt( len( warn_str) + 1: ) )

! ----------------------------------------------------------------------

!  if the directive is a setfile verbose directive

   elseif( set_stmt( : len( verbose_str)) == verbose_str )then which_directive

      call process_verbose_directive( set_stmt( len( verbose_str) + 1: ) )

! ----------------------------------------------------------------------

!  otherwise complain about the unknown directive

   else which_directive                                              ! directive was not recognized

      call msg_quit( "unknown setfile directive: " // trim( set_stmt) )

!  catergorize setfile statement: alter or integer or logical

   endif which_directive

! ----------------------------------------------------------------------

!  process_set_statement() exit

return                                                               ! process_set_statement()

! **********************************************************************

!  process_set_statement()

end subroutine process_set_statement

! **********************************************************************
! **********************************************************************

!  process_alter_directive() process alter directives

subroutine process_alter_directive( alter_dir)

! **********************************************************************

!  process_alter_directive() interface

! ----------------------------------------------------------------------

!  the alter directive from the setfile

character( len= *), intent( in) :: alter_dir                        

! **********************************************************************

!  entry: alter_dir is blank_compress_lower_case alter directive past the colon
!         "delete" | "blank" | "shift0" | "shift1" | "shift3"

!  exit: alter_dir is processed or error exit

! **********************************************************************

!  process_alter_directive() constants

! ----------------------------------------------------------------------

!  possible alter strings

   character( len= *), parameter :: delete_str = 'delete'            ! delete line

   character( len= *), parameter :: blank_str = 'blank'              ! blank line

   character( len= *), parameter :: shift0_str = 'shift0'            ! shift zero

   character( len= *), parameter :: shift1_str = 'shift1'            ! shift one

   character( len= *), parameter :: shift3_str = 'shift3'            ! shift three

! **********************************************************************

!  process_alter_directive() local

! ----------------------------------------------------------------------

!  count number of some statements to disallow more than one

   logical, save :: too_many_alter_statements = .false.              ! allow one alter statement

! **********************************************************************

!  process_alter_directive() text

continue                                                             ! process_alter_directive()

! ----------------------------------------------------------------------

!  only one alter directive per setfile

   too_many_alters: if( too_many_alter_statements )then              ! one alter per setfile

      call msg_quit( "too many alter statements")

   else too_many_alters                                              ! one alter per setfile

      too_many_alter_statements = .true.

   endif too_many_alters                                             ! one alter per setfile

!  if the alter state has not been set from the command line

   not_set: if( options% alter_state == alter_none )then             ! not set yet

! ----------------------------------------------------------------------

!  legal alter statement or error

! ----------------------------------------------------------------------

!  decode alter state

      alter_value_str: select case( alter_dir)                       ! which alter state

 !  alter delete

     case( delete_str) alter_value_str                               ! delete string

         options% alter_state = alter_delete                         ! set state to delete

!  alter blank

      case( blank_str) alter_value_str                               ! blank string

         options% alter_state = alter_blank                          ! set state to blank

!  alter shift1

      case( shift1_str) alter_value_str                              ! shift1 string

         options% alter_state = alter_shift_1                        ! set state to shift 1

!  alter shift0

      case( shift0_str) alter_value_str                              ! shift0 string

         options% alter_state = alter_shift_0                        ! set state to shift 0

!  alter shift3

      case( shift3_str) alter_value_str                              ! shift3 string

         options% alter_state = alter_shift_3                        ! set state to shift 3

!  unknown alter

      case default alter_value_str                                   ! some other string

         call msg_quit( "unknown alter directive: " // trim( alter_dir))

!  legal alter statement or error

      end select alter_value_str                                     ! which alter state

   endif not_set                                                     ! not set yet

! ----------------------------------------------------------------------

!  process_alter_directive() exit

return                                                               ! process_alter_directive()

! **********************************************************************

!  process_alter_directive()

end subroutine process_alter_directive

! **********************************************************************
! **********************************************************************

!  process_directory_directive() process include directory options

subroutine process_directory_directive( directory_dir)

! **********************************************************************

!  process_directory_directive() interface

! ----------------------------------------------------------------------

!  the directory directive from the setfile

character( len= *), intent( in) :: directory_dir                    

! **********************************************************************

!  entry: directory_opt is a directory to be added to the list
!         of directories to be searched for inlcude files

!  exit: directory_opt is on the list

! **********************************************************************

!  process_directory_directive() local

! ----------------------------------------------------------------------

!  point to a directory type

   type( path_t), pointer :: directory_ptr                           ! where to add to list

!  the name of a directory

   character( len= filename_len) :: name_str                         ! name to add to list

!  count length of quoted string

   integer :: directive_len                                          ! quoted in directive

!  count length of unquoted string

   integer :: name_len                                               ! unquote string

! **********************************************************************

!  process_directory_directive() text

continue                                                             ! process_directory_directive()

! ----------------------------------------------------------------------

!  unquote string to find path string

   call unquote_string( directory_dir, name_str, directive_len, name_len )

   no_name_str: if( name_len == 0 .or. directive_len == 0 )then      ! no directory name

      call msg_quit( "no directory name: " // trim( directory_dir) )

   endif no_name_str                                                 ! no directory name

!  verify no extra characters beyond name

   extra_chars: if( directory_dir( directive_len + 1: ) /= blank )then

      call msg_quit( "extra characters after directory path name: " // trim( directory_dir))

   endif extra_chars

! ----------------------------------------------------------------------

!  if name is already on the path

   call seek_directory( name_str, directory_ptr)                     ! check the path

   on_list_or_add: if( associated( directory_ptr) )then              ! already on path

      call msg_continue( "redundant include directory ignored: " // trim( directory_dir) )

!  if name is not already on the path

   else on_list_or_add                                               ! not on path

      call add_directory( name_str)                                  ! so add it

!  if reporting use of extensions

      extensions: if( options% report_extensions )then               ! extensions

         call msg_continue( "added include directory from setfile: " // trim( directory_dir) )

      endif extensions                                               ! extensions

   endif on_list_or_add                                              ! already on path or not

! ----------------------------------------------------------------------

!  process_directory_directive() exit

return                                                               ! process_directory_directive()

! **********************************************************************

!  process_directory_directive()

end subroutine process_directory_directive

! **********************************************************************
! **********************************************************************

!  seek_directory() return a pointer to directory_str or null()

subroutine seek_directory( name_str, directory_ptr)

! **********************************************************************

!  seek_directory() interface

! ----------------------------------------------------------------------

!  the name of the directory to seek

character( len= *), intent( in) :: name_str

!  a pointer to the directory entry if found or null()

type( path_t), pointer :: directory_ptr

! **********************************************************************

!  entry: directory_str is a directory to be added to the list
!         of directories to be searched for inlcude files

!  exit: directory_str is on the list

! **********************************************************************

!  seek_directory() text

continue                                                             ! seek_directory()

! ----------------------------------------------------------------------

!  search from beginning to end of path list

   nullify( directory_ptr)                                           ! start from known state

   directory_ptr => first_directory                                  ! head of path list

!  if the name is already in the path

   scan_path: do while( associated( directory_ptr) )                 ! until null() is encountered

      found_name: if( name_str == directory_ptr% name_str )then      ! names match

         exit scan_path                                              ! end of search

      endif found_name                                               ! names match

      directory_ptr => directory_ptr% next                           ! check next directory

   enddo scan_path                                                   ! until null() is encountered

! ----------------------------------------------------------------------

!  seek_directory() exit

return                                                               ! seek_directory()

! **********************************************************************

!  seek_directory()

end subroutine seek_directory

! **********************************************************************
! **********************************************************************

!  add_directory() return a pointer to directory_str or null()

subroutine add_directory( directory_str)

! **********************************************************************

!  add_directory() interface

! ----------------------------------------------------------------------

!  the name of the directory to add to the directory list

character( len= *), intent( in) :: directory_str                    

! **********************************************************************

!  entry: directory_str is a directory to be added to the list
!         of directories to be searched for inlcude files

!  exit: directory_str is on the list

! **********************************************************************

!  add_directory() local

! ----------------------------------------------------------------------

!  end of linked list, null() if no linked list yet

   type( path_t), save, pointer :: current_directory => null()       ! last directory in path

!  check allocation status

   integer :: astat                                                  ! allocate stat=

! **********************************************************************

!  add_directory() text

continue                                                             ! add_directory()

! ----------------------------------------------------------------------

!  append to list

   start_or_append: if( associated( first_directory) )then           ! start or append to path

      allocate( current_directory% next, stat= astat)                ! append a new directory

      append_status: if( astat > 0 )then                             ! allocate error

         call msg_quit( "can't append to path list: " // trim( directory_str) )

      endif append_status                                            ! allocate error

      current_directory => current_directory% next                   ! keep last at the end

!  start list

   else start_or_append                                              ! start or append to path

      allocate( first_directory, stat= astat)                        ! start the list

      start_status: if( astat > 0 )then                              ! allocate error

         call msg_quit( "can't start path list: " // trim( directory_str) )

      endif start_status                                             ! allocate error

      current_directory => first_directory                           ! first = last

   endif start_or_append                                             ! start or append to path

!  update new entry

   current_directory% name_str = directory_str                       ! add name to list

   current_directory% times_accessed = 0                             ! no file found here yet

   nullify( current_directory% next)                                 ! initialize pointer

! ----------------------------------------------------------------------

!  add_directory() exit

return                                                               ! add_directory()

! **********************************************************************

!  add_directory()

end subroutine add_directory

! **********************************************************************
! **********************************************************************

!  process_docfile_directive() process include docfile options

subroutine process_docfile_directive( docfile_dir)

! **********************************************************************

!  process_docfile_directive() interface

! ----------------------------------------------------------------------

!  the docfile directive from the setfile

character( len= *), intent( in) :: docfile_dir                      

! **********************************************************************

!  entry: docfile_opt is a docfile to be added to the list
!         of directories to be searched for inlcude files

!  exit: docfile_opt is on the list

! **********************************************************************

!  process_docfile_directive() local

! ----------------------------------------------------------------------

!  the name of the file to be opened

   character( len= filename_len) :: docfile_name

!  the length of the quoted string

   integer :: quoted_len

!  the length of the unquoted string

   integer :: unquoted_len

!  count number of some statements to disallow more than one

   logical, save :: too_many_docfile_statements = .false.            ! allow one docfile statement

! **********************************************************************

!  process_docfile_directive() text

continue                                                             ! process_docfile_directive()

! ----------------------------------------------------------------------

!  only one docfile statement per setfile

   too_many_docfiles: if( too_many_docfile_statements )then          ! one docfile per setfile

      call msg_quit( "too many docfile statements")

   else too_many_docfiles                                            ! one docfile per setfile

      too_many_docfile_statements = .true.

   endif too_many_docfiles                                           ! one docfile per setfile

!  unquote string on directive

   call unquote_string( docfile_dir, docfile_name, unquoted_len, quoted_len)

   no_name: if( unquoted_len == 0 .or. quoted_len == 0 )then

      call msg_quit( "no name found on docfile directive: " // trim( docfile_dir) )

   endif no_name

!  verify no extra characters beyond name

   extra_chars: if( docfile_dir( unquoted_len + 1: ) /= blank )then

      call msg_quit( "extra characters after docfile file name: " // trim( docfile_dir))

   endif extra_chars

! ----------------------------------------------------------------------

!  if docfile named on command line ignore the directive

   already_named: if( doc_file% named_file )then

      call msg_continue( "command line overrides setfile, docfile directive ignored: " // trim( docfile_dir) )

!  if docfile not named on command line open the named file

   else already_named

      doc_file% name_str = docfile_name                              ! docfile name from directive

      doc_file% named_file = .true.                                  ! now has a name

      call open_file( doc_file)                                      ! open the docfile

   endif already_named

! ----------------------------------------------------------------------

!  process_docfile_directive() exit

return                                                               ! process_docfile_directive()

! **********************************************************************

!  process_docfile_directive()

end subroutine process_docfile_directive

! **********************************************************************
! **********************************************************************

!  process_logfile_directive() process include logfile options

subroutine process_logfile_directive( logfile_dir)

! **********************************************************************

!  process_logfile_directive() interface

! ----------------------------------------------------------------------

!  the logfile directive from the setfile

character( len= *), intent( in) :: logfile_dir                      

! **********************************************************************

!  entry: logfile_opt is a logfile to be added to the list
!         of directories to be searched for inlcude files

!  exit: logfile_opt is on the list

! **********************************************************************

!  process_logfile_directive() local

! ----------------------------------------------------------------------

!  the name of the file to be opened

   character( len= filename_len) :: logfile_name

!  the length of the quoted string

   integer :: quoted_len

!  the length of the unquoted string

   integer :: unquoted_len

!  count number of some statements to disallow more than one

   logical, save :: too_many_logfile_statements = .false.            ! allow one logfile statement

! **********************************************************************

!  process_logfile_directive() text

continue                                                             ! process_logfile_directive()

! ----------------------------------------------------------------------

!  only one logfile statement per setfile

   too_many_logfiles: if( too_many_logfile_statements )then          ! one logfile per setfile

      call msg_quit( "too many logfile statements")

   else too_many_logfiles                                            ! one logfile per setfile

      too_many_logfile_statements = .true.

   endif too_many_logfiles                                           ! one logfile per setfile

!  unquote string on directive

   call unquote_string( logfile_dir, logfile_name, unquoted_len, quoted_len)

   no_name: if( unquoted_len == 0 .or. quoted_len == 0 )then

      call msg_quit( "no name found on logfile directive: " // trim( logfile_dir) )

   endif no_name

!  verify no extra characters beyond name

   extra_chars: if( logfile_dir( unquoted_len + 1: ) /= blank )then

      call msg_quit( "extra characters after logfile file name: " // trim( logfile_dir))

   endif extra_chars

! ----------------------------------------------------------------------

!  if logfile named on command line ignore the directive

   already_named: if( log_file% named_file )then

      call msg_continue( "command line overrides setfile, logfile directive ignored: " // trim( logfile_dir) )

!  if logfile not named on command line open the named file

   else already_named

      log_file% logical_unit = log_unit                              ! switch unit from stderr

      log_file% name_str = logfile_name                              ! logfile name from directive

      log_file% named_file = .true.                                  ! now has a name

      call open_file( log_file)                                      ! open the logfile

   endif already_named

! ----------------------------------------------------------------------

!  process_logfile_directive() exit

return                                                               ! process_logfile_directive()

! **********************************************************************

!  process_logfile_directive()

end subroutine process_logfile_directive

! **********************************************************************
! **********************************************************************

!  process_wrap_directive() process wrap directives

subroutine process_wrap_directive( wrap_dir)

! **********************************************************************

!  process_wrap_directive() interface

! ----------------------------------------------------------------------

!  the wrap directive from the setfile

character( len= *), intent( in) :: wrap_dir                         

! **********************************************************************

!  entry: wrap_dir is blank_compress_lower_case wrap directive
!         it must be a number string

!  exit: wrap_dir is processed or error exit

! **********************************************************************

!  process_wrap_directive() local

! ----------------------------------------------------------------------

!  count number of some statements to disallow more than one

   logical, save :: too_many_wrap_statements = .false.               ! allow one wrap statement

! **********************************************************************

!  process_wrap_directive() text

continue                                                             ! process_wrap_directive()

! ----------------------------------------------------------------------

!  only one wrap statement per setfile

   too_many_wraps: if( too_many_wrap_statements )then                ! one wrap per setfile

      call msg_quit( "too many wrap statements")

   else too_many_wraps                                               ! one wrap per setfile

      too_many_wrap_statements = .true.

   endif too_many_wraps                                              ! one wrap per setfile

! ----------------------------------------------------------------------

!  process wrap value if not already set on command line

   set_on_cl: if( options% wrap_length == wrap_none )then

      call process_wrap_value( wrap_dir)

   endif set_on_cl

!  if reporting use of extensions

   extensions: if( options% report_extensions )then                  ! extensions

      call msg_continue( "processed wrap directive from setfile: " // trim( wrap_dir) )

   endif extensions                                                  ! extensions

! ----------------------------------------------------------------------

!  process_wrap_directive() exit

return                                                               ! process_wrap_directive()

! **********************************************************************

!  process_wrap_directive()

end subroutine process_wrap_directive

! **********************************************************************
! **********************************************************************

!  process_edit_directive() process edit directives

subroutine process_edit_directive( edit_dir)

! **********************************************************************

!  process_edit_directive() interface

! ----------------------------------------------------------------------

!  the edit directive from the setfile

character( len= *), intent( in) :: edit_dir

! **********************************************************************

!  entry: edit_dir is blank_compress_lower_case edit directive
!         it must be "on" or "off"

!  exit: edit_dir is processed or error exit

! **********************************************************************

!  process_edit_directive() local

! ----------------------------------------------------------------------

!  count number of some statements to disallow more than one

   logical, save :: too_many_edit_statements = .false.               ! allow one edit statement

! **********************************************************************

!  process_edit_directive() text

continue                                                             ! process_edit_directive()

! ----------------------------------------------------------------------

!  only one edit statement per setfile

   too_many_edits: if( too_many_edit_statements )then                ! one edit per setfile

      call msg_quit( "too many edit statements")

   else too_many_edits                                               ! one edit per setfile

      too_many_edit_statements = .true.

   endif too_many_edits                                              ! one edit per setfile

! ----------------------------------------------------------------------

!  process edit switch

   on_off: if( edit_dir == on_str )then

      options% edit_source = .true.

   elseif( edit_dir == off_str)then on_off

      options% edit_source = .false.

   else on_off

      call msg_quit( "unknown option on edit directive: " // trim( edit_dir) )

   endif on_off

!  if reporting use of extensions

   extensions: if( options% report_extensions )then                  ! extensions

      call msg_continue( "processed edit directive from setfile: " // trim( edit_dir) )

   endif extensions                                                  ! extensions

! ----------------------------------------------------------------------

!  process_edit_directive() exit

return                                                               ! process_edit_directive()

! **********************************************************************

!  process_edit_directive()

end subroutine process_edit_directive

! **********************************************************************
! **********************************************************************

!  process_number_directive() process number directives

subroutine process_number_directive( number_dir)

! **********************************************************************

!  process_number_directive() interface

! ----------------------------------------------------------------------

!  the number directive from the setfile

character( len= *), intent( in) :: number_dir

! **********************************************************************

!  entry: number_dir is blank_compress_lower_case number directive
!         it must be "on" or "off"

!  exit: number_dir is processed or error exit

! **********************************************************************

!  process_number_directive() local

! ----------------------------------------------------------------------

!  count number of some statements to disallow more than one

   logical, save :: too_many_number_statements = .false.             ! allow one edit statement

! **********************************************************************

!  process_number_directive() text

continue                                                             ! process_number_directive()

! ----------------------------------------------------------------------

!  only one number statement per setfile

   too_many_number: if( too_many_number_statements )then             ! one number per setfile

      call msg_quit( "too many number statements")

   else too_many_number                                              ! one number per setfile

      too_many_number_statements = .true.

   endif too_many_number                                             ! one number per setfile

! ----------------------------------------------------------------------

!  process number switch

   on_off: if( number_dir == on_str )then

      options% number_source = .true.

   elseif( number_dir == off_str )then on_off

      options% number_source = .false.

   else on_off

      call msg_quit( "unknown option on number directive: " // trim( number_dir) )

   endif on_off

!  if reporting use of extensions

   extensions: if( options% report_extensions )then                  ! extensions

      call msg_continue( "processed number directive from setfile: " // trim( number_dir) )

   endif extensions                                                  ! extensions

! ----------------------------------------------------------------------

!  process_number_directive() exit

return                                                               ! process_number_directive()

! **********************************************************************

!  process_number_directive()

end subroutine process_number_directive

! **********************************************************************
! **********************************************************************

!  process_paren_directive() process paren directives

subroutine process_paren_directive( paren_dir)

! **********************************************************************

!  process_paren_directive() interface

! ----------------------------------------------------------------------

!  the paren directive from the setfile

character( len= *), intent( in) :: paren_dir

! **********************************************************************

!  entry: paren_dir is blank_compress_lower_case paren directive
!         it must be "on" or "off"

!  exit: paren_dir is processed or error exit

! **********************************************************************

!  process_paren_directive() local

! ----------------------------------------------------------------------

!  count number of some statements to disallow more than one

   logical, save :: too_many_paren_statements = .false.              ! allow one edit statement

! **********************************************************************

!  process_paren_directive() text

continue                                                             ! process_paren_directive()

! ----------------------------------------------------------------------

!  only one paren statement per setfile

   too_many_paren: if( too_many_paren_statements )then               ! one paren per setfile

      call msg_quit( "too many paren statements")

   else too_many_paren                                               ! one paren per setfile

      too_many_paren_statements = .true.

   endif too_many_paren                                              ! one paren per setfile

! ----------------------------------------------------------------------

!  process paren switch

   on_off: if( paren_dir == on_str )then

      options% args_in_parens = .true.

   elseif( paren_dir == off_str )then on_off

      options% args_in_parens = .false.

   else on_off

      call msg_quit( "unknown option on paren directive: " // trim( paren_dir) )

   endif on_off

!  if reporting use of extensions

   extensions: if( options% report_extensions )then                  ! extensions

      call msg_continue( "processed paren directive from setfile: " // trim( paren_dir) )

   endif extensions                                                  ! extensions

! ----------------------------------------------------------------------

!  process_paren_directive() exit

return                                                               ! process_paren_directive()

! **********************************************************************

!  process_paren_directive()

end subroutine process_paren_directive

! **********************************************************************
! **********************************************************************

!  process_summary_directive() process summary directives

subroutine process_summary_directive( summary_dir)

! **********************************************************************

!  process_summary_directive() interface

! ----------------------------------------------------------------------

!  the srap directive from the setfile

character( len= *), intent( in) :: summary_dir

! **********************************************************************

!  entry: summary_dir is blank_compress_lower_case summary directive
!         it must be "on" or "off"

!  exit: summary_dir is processed or error exit

! **********************************************************************

!  process_summary_directive() local

! ----------------------------------------------------------------------

!  count number of some statements to disallow more than one

   logical, save :: too_many_summary_statements = .false.            ! allow one summary statement

! **********************************************************************

!  process_summary_directive() text

continue                                                             ! process_summary_directive()

! ----------------------------------------------------------------------

!  only one summary statement per setfile

   too_many_summarys: if( too_many_summary_statements )then          ! one summary per setfile

      call msg_quit( "too many summary statements")

   else too_many_summarys                                            ! one summary per setfile

      too_many_summary_statements = .true.

   endif too_many_summarys                                           ! one summary per setfile

! ----------------------------------------------------------------------

!  process summary switch

   on_off: if( summary_dir == on_str )then

      options% print_summary = .true.

   elseif( summary_dir == off_str)then on_off

      options% print_summary = .false.

   else on_off

      call msg_quit( "unknown option on summary directive: " // trim( summary_dir) )

   endif on_off

!  if reporting use of extensions

   extensions: if( options% report_extensions )then                  ! extensions

      call msg_continue( "processed summary directive from setfile: " // trim( summary_dir) )

   endif extensions                                                  ! extensions

! ----------------------------------------------------------------------

!  process_summary_directive() exit

return                                                               ! process_summary_directive()

! **********************************************************************

!  process_summary_directive()

end subroutine process_summary_directive

! **********************************************************************
! **********************************************************************

!  process_verbose_directive() process verbose directives

subroutine process_verbose_directive( verbose_dir)

! **********************************************************************

!  process_verbose_directive() interface

! ----------------------------------------------------------------------

!  the verbose directive from the setfile

character( len= *), intent( in) :: verbose_dir

! **********************************************************************

!  entry: verbose_dir is blank_compress_lower_case verbose directive
!         it must be "on" or "off"

!  exit: verbose_dir is processed or error exit

! **********************************************************************

!  process_verbose_directive() local

! ----------------------------------------------------------------------

!  count number of some statements to disallow more than one

   logical, save :: too_many_verbose_statements = .false.            ! allow one verbose statement

! **********************************************************************

!  process_verbose_directive() text

continue                                                             ! process_verbose_directive()

! ----------------------------------------------------------------------

!  only one verbose statement per setfile

   too_many_verboses: if( too_many_verbose_statements )then          ! one verbose per setfile

      call msg_quit( "too many verbose statements")

   else too_many_verboses                                            ! one verbose per setfile

      too_many_verbose_statements = .true.

   endif too_many_verboses                                           ! one verbose per setfile

! ----------------------------------------------------------------------

!  process verbose switch if not already set on command line

   on_off: if( verbose_dir == on_str )then

      options% verbose_mode = .true.

   elseif( verbose_dir == off_str)then on_off

      options% verbose_mode = .false.

   else on_off

      call msg_quit( "unknown option on verbose directive: " // trim( verbose_dir) )

   endif on_off

!  if reporting use of extensions

   extensions: if( options% report_extensions )then                  ! extensions

      call msg_continue( "processed verbose directive from setfile: " // trim( verbose_dir) )

   endif extensions                                                  ! extensions

! ----------------------------------------------------------------------

!  process_verbose_directive() exit

return                                                               ! process_verbose_directive()

! **********************************************************************

!  process_verbose_directive()

end subroutine process_verbose_directive

! **********************************************************************
! **********************************************************************

!  process_warn_directive() process warn directives

subroutine process_warn_directive( warn_dir)

! **********************************************************************

!  process_warn_directive() interface

! ----------------------------------------------------------------------

!  the warn directive from the setfile

character( len= *), intent( in) :: warn_dir

! **********************************************************************

!  entry: warn_dir is blank_compress_lower_case warn directive
!         it must be a "on" or "off"

!  exit: warn_dir is processed or error exit

! **********************************************************************

!  process_warn_directive() local

! ----------------------------------------------------------------------

!  count number of some statements to disallow more than one

   logical, save :: too_many_warn_statements = .false.               ! allow one warn statement

! **********************************************************************

!  process_warn_directive() text

continue                                                             ! process_warn_directive()

! ----------------------------------------------------------------------

!  only one warn statement per setfile

   too_many_warns: if( too_many_warn_statements )then                ! one warn per setfile

      call msg_quit( "too many warn statements")

   else too_many_warns                                               ! one warn per setfile

      too_many_warn_statements = .true.

   endif too_many_warns                                              ! one warn per setfile

! ----------------------------------------------------------------------

!  process warn switch

   on_off: if( warn_dir == on_str )then

      options% warn_undeclared = .true.

   elseif( warn_dir == off_str)then on_off

      options% warn_undeclared = .false.

   else on_off

      call msg_quit( "unknown option on warn directive: " // trim( warn_dir) )

   endif on_off

!  if reporting use of extensions

   extensions: if( options% report_extensions )then                  ! extensions

      call msg_continue( "processed warn directive from setfile: " // trim( warn_dir) )

   endif extensions                                                  ! extensions

! ----------------------------------------------------------------------

!  process_warn_directive() exit

return                                                               ! process_warn_directive()

! **********************************************************************

!  process_warn_directive()

end subroutine process_warn_directive

! **********************************************************************
! **********************************************************************

!  %%% read and write files, and process coco lines and statements

! **********************************************************************
! **********************************************************************

!  process_input_file() reads a coco source file, recurse upon include files

recursive subroutine process_input_file( this_file)

! **********************************************************************

!  process_input_file() interface

! ----------------------------------------------------------------------

!  the file to be processed

type( file_t), target, intent( inout) :: this_file

! **********************************************************************

!  entry: source file to be processed

!  exit: source file has been processed or error

! **********************************************************************

!  process_input_file() local

! ----------------------------------------------------------------------

!  statement buffer

   character( len= buffer_len) :: statement                          ! store a statement

!  signal complete statement

   logical :: complete                                               ! true when complete

! **********************************************************************

!  process_input_file() text

continue                                                             ! process_input_file()

!  open source file

   call open_file( this_file)                                        ! open & link current_file

!  count files

   total% input_files = total% input_files + 1                       ! one file per process_input_file()

!  as if finished a complete statement at beginning of file

   complete = .true.                                                 ! at start

! ----------------------------------------------------------------------

#ifdef KEY /* Bug 5087 */
     ! Generate cpp-like file/linenumber directive
     if (options%integrated) then
	write(output_file%logical_unit, '(3a)') '# 1 "', &
	   trim(this_file%name_str), '" 1'
     endif
#endif /* KEY Bug 5087 */

!  main read lines loop

   read_lines: do                                                    ! process line by line

!  read from input file

      read( unit= this_file% logical_unit, fmt= this_file% format_str, iostat= this_file% io_status) &
            this_file% line

      read_source: if( this_file% io_status > 0 )then                ! read error

         call msg_quit( "can't read input file: " // trim( this_file% name_str))

      endif read_source                                              ! read error

! ----------------------------------------------------------------------

!  read until end of file or complete statement

      read_eof: if( this_file% io_status < 0 )then                   ! eof

         call blank_compress_lower_case( statement, null_string)     ! reset for next file

         continuation_eof: if( .not. complete )then                  ! if within a continued statement

            call msg_quit( "end of file encountered in a continued coco statement")

         endif continuation_eof                                      ! if within a continued statement

         exit read_lines                                             ! read no more

      endif read_eof                                                 ! eof

! ----------------------------------------------------------------------

!  count lines

      this_file% lines_transfered = this_file% lines_transfered + 1

!  process coco lines or source lines

      process_coco: if( line( : len( coco_key)) == coco_key )then

!  count lines

         total% coco_lines = total% coco_lines + 1                   ! one line per iteration

!  write coco line to the output

         call write_coco_line( output_file)                          ! as per the alter state

!  if line is not a coco comment

         coco_statement: if( is_coco_statement( line( len( coco_key) + 1: )) )then

!  read a complete statement

            call gather_coco_statement( line, statement, complete)

!  if not yet a complete statement go get the rest of it

            get_statement: if( .not. complete )then

               cycle read_lines

            endif get_statement

! ----------------------------------------------------------------------

!  process coco directives

! ----------------------------------------------------------------------

!  directive is a coco include directive

            process_directive: if( statement( : len( include_str)) == include_str )then

!  process (possibly recursive) include directives (include 'a' --> include 'b' &c)

               call process_include_directive( statement( len( include_str) + 1: ))

#ifdef KEY /* Bug 5087 */
	       ! Generate cpp-like file/linenumber directive
               cpp_directive: if (options%integrated) then
		 write (output_file%logical_unit, '(a,i0,3a)') '# ', &
		   this_file%lines_transfered + 1, ' "', trim(this_file%name_str), '" 2'
	       endif cpp_directive
#endif /* KEY Bug 5087 */

!  directive is a coco endfile directive

            elseif( statement == endfile_str )then process_directive

!  if line is active read no more from this input file

               active_endfile: if( if_construct% now_selected )then

                  exit read_lines

               endif active_endfile

!  process any other directive

            else process_directive                                   ! recursive or oridnary directives

!  process other (not possibly recursive) coco directives

               call process_coco_statement( statement)               ! check for other directives

            endif process_directive                                  ! recursive or oridnary directives

!  reset current file

            current_file => this_file                                ! closing the include unlinked current

!  if line is not a coco comment

         endif coco_statement

! ----------------------------------------------------------------------

!  process source lines

      else process_coco                                              ! coco line or source line

!  error if a source line is mixed into a continued coco statement

         continuation_error: if( .not. complete )then

            call msg_quit( "source line encountered in a continued coco statement")

         endif continuation_error

!  if within the active block of a coco if construct

         active_source: if( if_construct% now_selected )then         ! if coco statement active source

!  editing source lines is enabled

            edit_source: if( options% edit_source )then              ! edit source lines

!  if ? present, edit source line

               edit_line: if( index( line, arg_key) > 0 )then        ! if have ?name?

                  call edit_source_line( line)                       ! edit integers, logicals, macros

               endif edit_line                                       ! if have ?name?

            endif edit_source                                        ! edit source lines

         endif active_source                                         ! active source

!  copy source lines

         call write_source_line( output_file)                        ! to output file

!  end processing coco lines

      endif process_coco                                             ! coco line or source line

!  end main read lines loop

   enddo read_lines                                                  ! process line by line

   total% input_lines = total% input_lines + this_file% lines_transfered

! ----------------------------------------------------------------------

!  end of file

   call close_file( this_file)                                       ! close & unlink current_file

! ----------------------------------------------------------------------

!  process_input_file() exit

return                                                               ! process_input_file()

! **********************************************************************

!  process_input_file()

end subroutine process_input_file

! **********************************************************************
! **********************************************************************

!  gather_coco_statement() examine lines and signal a complete statement

subroutine gather_coco_statement( line, statement, complete)

! **********************************************************************

!  gather_coco_statement() interface

! ----------------------------------------------------------------------

!  the current input file

character( len= *), intent( in) :: line

!  the statement as it is built

character( len= *), intent( inout) :: statement

!  true when a complete statement has been seen

logical, intent( out) :: complete                                   

! **********************************************************************

!  entry: statement is a line
!         "..."

!  exit: statement is accumulated, complete is true when whole

! **********************************************************************

!  gather_coco_statement() local

! ----------------------------------------------------------------------

!  count continuation lines

   integer, save :: continuation_lines = 0                           ! max 39 continuations

! ----------------------------------------------------------------------

!  number of characters processed so far

   integer, save :: statement_len = 0                                ! last character in statement

! **********************************************************************

!  gather_coco_statement() text

continue                                                             ! gather_coco_statement()

! ----------------------------------------------------------------------

!  blank compress lower case

   call blank_compress_lower_case( statement, line( len( coco_key) + 1: ) )

!  if statement length hasn't changed and statement is not a comment

   null_stmt: if( statement_len == len_trim( statement) )then

      call msg_quit( "null statement encountered in continuation sequence")

   endif null_stmt

!  if not a complete statement yet

   statement_len = len_trim( statement)                              ! check last character

!  last character is continuation means more to read to get a complete statement

   incomplete: if( statement( statement_len: statement_len) == continuation )then

!  if too many continuation lines

      too_many: if( continuation_lines > max_continuations )then

         call msg_quit( "too many continuations")                    ! complain or quit

      endif too_many                                                 ! overflow buffer

!  count continuation lines

      continuation_lines = continuation_lines + 1

!  go get the rest of the statement

      complete = .false.                                             ! get rest of statement

      return

   endif incomplete                                                  ! read rest of directive

!  coco statement is complete

   continuation_lines = 0                                            ! no continuations (now)

   statement_len = 0

   complete = .true.

! ----------------------------------------------------------------------

!  gather_coco_statement() exit

return                                                               ! gather_coco_statement()

! **********************************************************************

!  gather_coco_statement()

end subroutine gather_coco_statement

! **********************************************************************
! **********************************************************************

!  is_coco_statement() true if coco statement is a coco construct and not a coco comment

logical function is_coco_statement( coco_stmt)

! **********************************************************************

!  is_coco_statement() interface

! ----------------------------------------------------------------------

!  the coco statement to be categorized

character( len= *), intent( in) :: coco_stmt                       

! **********************************************************************

!  entry: coco_stmt is coco statement past the coco key
!         "..."

!  exit: true if statement is a coco construct,
!        false if statement is a coco comment

! **********************************************************************

!  is_coco_statement() local

! ----------------------------------------------------------------------

!  locations of specific characters

   integer :: char_idx                                               ! character pointer

! **********************************************************************

!  is_coco_statement() text

continue                                                             ! is_coco_statement()

! ----------------------------------------------------------------------

!  scan from first character (past coco key) for non-whitespace

   char_idx = verify( coco_stmt, white_space)                        ! seek non-whitespace

!  if found other than whitespace

   white_space_or_comment: if( char_idx > 0 )then                    ! found first non-whitespace

!  is construct if first character past the white space is not comment

      is_coco_statement = coco_stmt( char_idx: char_idx) /= comment  ! first character is comment

!  all whitespace

   else white_space_or_comment                                       ! first non-whitespace

!  is not a construct

      is_coco_statement = .false.                                    ! all whitespace is a comment

   endif white_space_or_comment                                      ! first non-whitespace

! ----------------------------------------------------------------------

!  is_coco_statement() exit

return                                                               ! is_coco_statement()

! **********************************************************************

!  is_coco_statement()

end function is_coco_statement

! **********************************************************************
! **********************************************************************

!  write_coco_line() write a coco line of output

subroutine write_coco_line( this_file)

! **********************************************************************

!  write_coco_line() interface

! ----------------------------------------------------------------------

!  the file to receive the output

type( file_t), intent( inout) :: this_file                        

! **********************************************************************

!  entry: out_unit is the logical unit connected to the output file
!         coco_line is a line to be written as per the current alter state

!  exit: line has been written or error exit

! **********************************************************************

!  write_coco_line() constants

! ----------------------------------------------------------------------

!  line prefix when alter state is shift 3

   character( len= *), parameter :: shift3_prefix = '!?>'            ! alter: shift3

! **********************************************************************

!  write_coco_line() local

! ----------------------------------------------------------------------

!  maximum line length

   character( len= source_line_len) :: line_buffer                   ! construct output line

!  detect lines longer than free_format_len

   logical :: long_lines                                             ! mark long lines

! **********************************************************************

!  write_coco_line() text

continue                                                             ! write_coco_line()

! ----------------------------------------------------------------------

!  write output as per alter state

   long_lines = .false.                                              ! no long lines yet

! ----------------------------------------------------------------------

   alter_print: select case( options% alter_state)                   ! as per alter state

!  delete the line

   case( alter_delete) alter_print                                   ! do nothing

      return                                                         ! line is deleted

!  blank line

   case( alter_blank) alter_print                                    ! print nothing

      write( unit= this_file% logical_unit, fmt= this_file% format_str, iostat= this_file% io_status) &
             null_string

!  comment the line

   case( alter_shift_0) alter_print                                  ! change first character to print a comment

      line_buffer = comment // this_file% line( len( comment): )     ! construct printed line

      write( unit= this_file% logical_unit, fmt= this_file% format_str, iostat= this_file% io_status) &
             trim( line_buffer)

!  shift one or comment the line

   case( alter_shift_1) alter_print                                  ! add a character to print a comment

      line_buffer = comment // this_file% line                       ! construct printed line

      long_lines = len_trim( line_buffer) > free_format_len          ! check for overlength

      write( unit= this_file% logical_unit, fmt= this_file% format_str, iostat= this_file% io_status) &
             trim( line_buffer)

!  shift three or comment the line

   case( alter_shift_3) alter_print                                  ! add three characters to print a comment

      line_buffer = shift3_prefix // this_file% line                 ! construct printed line

      long_lines = len_trim( line_buffer) > free_format_len          ! check for overlength

      write( unit= this_file% logical_unit, fmt= this_file% format_str, iostat= this_file% io_status) &
             trim( line_buffer)

   end select alter_print                                            ! as per alter state

! ----------------------------------------------------------------------

!  check write iostat

   write_this_file: if( this_file% io_status > 0 )then               ! write error

      call msg_quit( "error writing source output: " // trim( this_file% name_str) )

   endif write_this_file                                             ! write error

! ----------------------------------------------------------------------

!  complain if source line too long

   len_warning: if( long_lines )then                                 ! len_trim( line) > 132

      call msg_continue( "line longer than 132 characters:")         ! warn and continue

      call msg_continue( trim( line_buffer))                         ! show the line

   endif len_warning                                                 ! len_trim( line) > 132

! ----------------------------------------------------------------------

!  write_coco_line() exit

return                                                               ! write_coco_line()

! **********************************************************************

!  write_coco_line()

end subroutine write_coco_line

! **********************************************************************
! **********************************************************************

!  write_source_line() write a line of output

subroutine write_source_line( this_file)

! **********************************************************************

!  write_source_line() interface

! ----------------------------------------------------------------------

!  the file to receive the output

type( file_t), intent( inout) :: this_file

! **********************************************************************

!  entry: out_unit is the logical unit connected to the output file
!         source_line is the line of Fortran source to be written

!  exit: the line is written or error exit

! **********************************************************************

!  write_source_line() constants

! ----------------------------------------------------------------------

!  column to start line number

   integer, parameter :: number_len = 75                             ! past the end of fixed format + any '&' in column 73

! **********************************************************************

!  write_source_line() local

! ----------------------------------------------------------------------

!  character line number

   character( len= conversion_len) :: number_str                     ! line number as string

! **********************************************************************

!  write_source_line() text

continue                                                             ! write_source_line()

! ----------------------------------------------------------------------

!  if currently printing output

   process_line: if( if_construct% now_selected )then                ! as source or alter state

!  check whether to number the source line

      want_numbers: if( options% number_source )then

!  only number lines if there's nothing there now

         can_number: if( this_file% line( number_len: ) == blank )then

            write( unit= number_str, fmt= conversion_fmt) current_file% lines_transfered

            this_file% line( number_len: ) = '! ' // trim( current_file% name_str) // ': ' // adjustl( number_str)

         endif can_number

      endif want_numbers

!  write source output

      write( unit= this_file% logical_unit, fmt= this_file% format_str, iostat= this_file% io_status) &
             trim( this_file% line)

!  check for write error

      write_error: if( this_file% io_status > 0 )then                ! write error

         call msg_quit( "error writing source output: " // trim( this_file% name_str) )

      endif write_error                                              ! write error

!  count lines written

      this_file% lines_transfered = this_file% lines_transfered + 1  ! lines of output

      total% selected_lines = total% selected_lines + 1

   else process_line                                                 ! as source or alter state

!  otherwise print as per the alter state

      call write_coco_line( this_file)                               ! print as coco line

      total% elided_lines = total% elided_lines + 1

   endif process_line                                                ! as source or alter state

! ----------------------------------------------------------------------

!  write_source_line() exit

return                                                               ! write_source_line()

! **********************************************************************

!  write_source_line()

end subroutine write_source_line

! **********************************************************************
! **********************************************************************

!  process_include_directive() process an include directive

recursive subroutine process_include_directive( include_dir)

! **********************************************************************

!  process_include_directive() interface

! ----------------------------------------------------------------------

!  the include directive

character( len= *), intent( in) :: include_dir                      

! **********************************************************************

!  entry: inc_name is inlcude file name

!  exit: inc_name is inlcude file name with directory prepended

! **********************************************************************

!  process_include_directive() constants

! ----------------------------------------------------------------------

!  mark the beginning and end of include files

   character( len= *), parameter :: begin_inc = '??! INCLUDE '       ! as per the standard

   character( len= *), parameter :: end_inc = '??! END INCLUDE '     ! as per the standard

! **********************************************************************

!  process_include_directive() local

! ----------------------------------------------------------------------

!  file variable of file named on the include directive

   type( file_t), target :: include_file                             ! target of current_file

! ----------------------------------------------------------------------

!  length of quoted include file name

   integer :: construct_len                                          ! length of coco directive

!  length of unquoted include file name

   integer :: name_len                                               ! length of name

! **********************************************************************

!  process_include_directive() text

continue                                                             ! process_include_directive()

! ----------------------------------------------------------------------

!  check the include syntax: unquote the include filename

   call unquote_string( include_dir, include_file% name_str, construct_len, name_len )

   no_name_str: if( name_len == 0 .or. construct_len == 0 )then      ! no include file name

      call msg_quit( "no include file name: " // trim( include_dir))

   endif no_name_str                                                 ! no inlcude file name

! ----------------------------------------------------------------------

!  if active block, process include directive

   active_inc: if( if_construct% now_selected )then                  ! if active include line

!  see if the include file exists

      inquire( file= include_file% name_str, &
               exist= include_file% named_file, iostat= include_file% io_status)

      inquire_error: if( include_file% io_status > 0 )then           ! inquire failed

         call msg_quit( "can't inquire include file: " // trim( include_file% name_str) )

      endif inquire_error                                            ! inquire failed

!  if not found, check directories

      seek_inc: if( .not. include_file% named_file )then             ! haven't found the file yet

         call seek_include_file( include_file)                       ! check the include list

      endif seek_inc                                                 ! haven't found the file yet

!  if still not found, complain and quit

      no_name: if( .not. include_file% named_file )then              ! still haven't found the file

         call msg_quit( "can't find include file: " // trim( include_file% name_str) )

      endif no_name                                                  ! still haven't found the file

! ----------------------------------------------------------------------

!  build include_file to pass to process_input_file()

! ----------------------------------------------------------------------

!  get new unit

      include_unit: if( current_file% logical_unit == input_unit )then

         include_file% logical_unit = read_unit                      ! start using read_unit, etc.

      else include_unit                                              ! otherwise

         include_file% logical_unit = current_file% logical_unit + 1 ! next unit

      endif include_unit                                             ! find the new logical_unit

!  include file components

      include_file% format_str = current_file% format_str            ! use same format

      include_file% line => null()                                   ! linked to buffer in process_input_file()

      include_file% io_status = 0                                    ! no errors yet

      include_file% lines_transfered = 0                             ! no lines yet

      include_file% named_file = .true.                              ! file has a name

      include_file% create = .false.                                 ! read only

! ----------------------------------------------------------------------

!  mark include file in output

      line = begin_inc // include_file% name_str                     ! build remark string

      call write_coco_line( output_file)                             ! write remarck string

!  prepare to process include file

      total% include_files = total% include_files + 1                ! one more include

!  process include file

      call process_input_file( include_file)                         ! recursive call

!  relink pointers

      output_file% line => line                                      ! was linked by process_input_file()

!  mark include file in output

      line = end_inc // include_file% name_str

      call write_coco_line( output_file)

! ----------------------------------------------------------------------

!  if active block, process include directive

   endif active_inc                                                  ! if active include line

!  end processing include statement

! ----------------------------------------------------------------------

!  process_include_directive() exit

return                                                               ! process_include_directive()

! **********************************************************************

!  process_include_directive()

end subroutine process_include_directive

! **********************************************************************
! **********************************************************************

!  seek_include_file() seek inlcude file in directories

subroutine seek_include_file( include_file)

! **********************************************************************

!  seek_include_file() interface

! ----------------------------------------------------------------------

!  the include file name to be sought

type( file_t), intent( inout) :: include_file

! **********************************************************************

!  entry: include_file is inlcude file name

!  exit: include_file is inlcude file name with directory prepended

! **********************************************************************

!  seek_include_file() local

! ----------------------------------------------------------------------

!  pointer to directories on path

   type( path_t), pointer :: directory                               ! search directory list

!  construct path/names to check for existance

   character( len= filename_len) :: trial_name                       ! dir/file

#ifdef KEY /* Bug 5087 */
   integer :: dirlen
#endif /* KEY Bug 5087 */

! **********************************************************************

!  seek_include_file() text

continue                                                             ! seek_include_file()

! ----------------------------------------------------------------------

!  search list for directory/file

   nullify( directory)                                               ! prepare pointer for use

   directory => first_directory                                      ! start at head of list

!  last directory in path is not associated

   search_list: do while( associated( directory) )                   ! find name

!  construct full name <directory-name><file-name>

#ifdef KEY /* Bug 5087 */
      ! Provide directory separator if directory doesn't have one at end
      dirlen = len(trim(directory%name_str))
      trial_name = trim( directory% name_str) // include_file% name_str
      if (0 /= dirlen) then
         if (directory_separator /= directory%name_str(dirlen:dirlen)) then
	    trial_name = trim( directory% name_str) // directory_separator // &
	       include_file% name_str
	 endif
      endif
#else /* KEY Bug 5087 */
      trial_name = trim( directory% name_str) // include_file% name_str
#endif /* KEY Bug 5087 */

!  check file existence

      inquire( file= trial_name, &
               exist= include_file% named_file, iostat= include_file% io_status)

      inquire_error: if( include_file% io_status > 0 )then           ! inquire error

         call msg_quit( "can't inquire include file: " // trim( trial_name))

      endif inquire_error                                            ! inquire error

!  found file name

      name_match: if( include_file% named_file )then                 ! if found it

!  found a file in this directory

         directory% times_accessed = directory% times_accessed + 1   ! one more time

!  rewrite include file name to include directory

         include_file% name_str = trial_name                         ! name to use

         extensions: if( options% report_extensions )then            ! remark new name

            call msg_continue( "found include file in directory: " // trim( trial_name))

         endif extensions                                            ! remark new name

         exit search_list                                            ! eureka

      endif name_match                                               ! if names match

!  filename not yet found so try next directory

      directory => directory% next                                   ! check next symbol on list

   enddo search_list                                                 ! find name

! ----------------------------------------------------------------------

!  seek_include_file() exit

return                                                               ! seek_include_file()

! **********************************************************************

!  seek_include_file()

end subroutine seek_include_file

! **********************************************************************
! **********************************************************************

!  %%% edit Fortran source lines

! **********************************************************************
! **********************************************************************

!  edit_source_line() edit source lines

subroutine edit_source_line( source_line)

! **********************************************************************

!  edit_source_line() interface

! ----------------------------------------------------------------------

!  source line to be edited

character( len= *), intent( inout) :: source_line

! **********************************************************************

!  entry: line is a line of Fortran source
!         with (possibly) ?file?, ?line?, ?date?, ?time?, ?integer?, ?logical?, ?macro?

!  exit: line has any ?macro? etc. strings replaced with their values

! **********************************************************************

!  edit_source_line() local

! ----------------------------------------------------------------------

!  copy of line since editing may expand the line beyond its length

   character( len= buffer_len) :: edit_line

!  make lower case copy of line

   character( len= buffer_len) :: lower_case_line                    ! search this copy

   integer :: this_char                                              ! point to target

! **********************************************************************

!  edit_source_line() text

! ----------------------------------------------------------------------

continue                                                             ! edit_source_line()

! ----------------------------------------------------------------------

!  the line may be expanded by editing, so edit a long buffer

   edit_line = source_line

!  make lower case copy of line

   scan_line: do this_char = 1, len( line)                           ! check each char

      to_lower: select case( edit_line( this_char: this_char))       ! check char by char

      case( 'A': 'Z') to_lower                                       ! if upper case

         lower_case_line( this_char: this_char) = achar( iachar( edit_line( this_char: this_char)) &
                                                         + change_case)

      case default to_lower                                          ! otherwise

         lower_case_line( this_char: this_char) = edit_line( this_char: this_char)

      end select to_lower                                            ! check char by char

   enddo scan_line                                                   ! check each char

! ----------------------------------------------------------------------

!  process ?coco?

   call edit_coco_strings( edit_line, lower_case_line)

! ----------------------------------------------------------------------

!  if processing ?file? & ?line?

   edit_file_and_line: if( options% edit_file )then                  ! if processing ?file? & ?line?

      call edit_file_line_strings( edit_line, lower_case_line)

   endif edit_file_and_line                                          ! if processing ?file? & ?line?

! ----------------------------------------------------------------------

!  if processing ?date? & ?time?

   edit_date_and_time: if( options% edit_date )then                  ! processing ?date? & ?time?

      call edit_date_time_strings( edit_line, lower_case_line)

   endif edit_date_and_time                                          ! processing ?date? & ?time?

! ----------------------------------------------------------------------

!  replace ?name? with the current integer value of name

   edit_integers: if( options% edit_integers )then                   ! if editing integers

      call edit_integer_strings( edit_line, lower_case_line)         ! process ?integer?

      call edit_logical_strings( edit_line, lower_case_line)         ! process ?logical?

   endif edit_integers                                               ! if editing integers

! ----------------------------------------------------------------------

!  replace ?name? with the current string value of name

   edit_macros: if( options% edit_macros )then                       ! if editing macros

      call edit_macro_strings( edit_line, lower_case_line)           ! process ?macro?

   endif edit_macros                                                 ! if editing macros

! ----------------------------------------------------------------------

!  remove any line length overflow

   wrap_lines: if( options% wrap_length /= wrap_off )then            ! if wrapping source

      call wrap_source_line( edit_line)                              ! wrap source lines if len > 132

   endif wrap_lines                                                  ! if wrapping source

   source_line = edit_line

! ----------------------------------------------------------------------

!  edit_source_line() exit

return                                                               ! edit_source_line()

! **********************************************************************

!  edit_source_line()

end subroutine edit_source_line

! **********************************************************************
! **********************************************************************

!  edit_coco_strings() process ?coco? strings

subroutine edit_coco_strings( edit_line, lower_case_line)

! **********************************************************************

!  edit_coco_strings() interface

! ----------------------------------------------------------------------

!  the source line to be edited

character( len= *), intent( inout) :: edit_line

!  the source line in lower case to enable searches

character( len= *), intent( inout) :: lower_case_line

! **********************************************************************

!  entry: line is a line of Fortran source with (possibly) ?integer

!  exit: line has any ?name strings replaced with their values

! **********************************************************************

!  edit_coco_strings() local

! ----------------------------------------------------------------------

!  find substring

   integer :: search_idx                                             ! go if needed

! **********************************************************************

!  edit_coco_strings() text

! ----------------------------------------------------------------------

continue                                                             ! edit_coco_strings()

! ----------------------------------------------------------------------

!  replace ?coco? with the coco rcs id

   search_idx = index( lower_case_line, coco_str)

   go_coco: if( search_idx > 0 )then

      call replace_substring( edit_line, lower_case_line, coco_str, coco_rcs_id, search_idx)

      remark_coco: if( options% report_extensions )then

         call msg_continue( "edited coco: " // coco_rcs_id)

      endif remark_coco

   endif go_coco

! ----------------------------------------------------------------------

!  edit_coco_strings() exit

return                                                               ! edit_coco_strings()

! **********************************************************************

!  edit_coco_strings()

end subroutine edit_coco_strings

! **********************************************************************
! **********************************************************************

!  edit_file_line_strings() process ?file? & ?line? strings

subroutine edit_file_line_strings( edit_line, lower_case_line)

! **********************************************************************

!  edit_file_line_strings() interface

! ----------------------------------------------------------------------

!  the source line to be edited

character( len= *), intent( inout) :: edit_line

!  the source line in lower case to enable searches

character( len= *), intent( inout) :: lower_case_line

! **********************************************************************

!  entry: line is a line of Fortran source with (possibly) ?integer

!  exit: line has any ?name strings replaced with their values

! **********************************************************************

!  edit_file_line_strings() local

! ----------------------------------------------------------------------

!  strings to be edited into the line

   character( len= conversion_len) :: line_number_str                ! character line number

!  find substring

   integer :: search_idx                                             ! go if needed

! **********************************************************************

!  edit_file_line_strings() text

! ----------------------------------------------------------------------

continue                                                             ! edit_file_line_strings()

! ----------------------------------------------------------------------

!  replace ?file? with the current filename

   search_idx = index( lower_case_line, file_str)

   go_file: if( search_idx > 0 )then

      call replace_substring( edit_line, lower_case_line, file_str, trim( current_file% name_str), &
                              search_idx)

      remark_file: if( options% report_extensions )then

         call msg_continue( "edited file: " // trim( current_file% name_str) )

      endif remark_file

   endif go_file

!  replace ?line? with the current line number

   search_idx = index( lower_case_line, line_str)

   go_line: if( search_idx > 0 )then

      write( unit= line_number_str, fmt= conversion_fmt) current_file% lines_transfered

      line_number_str = adjustl( line_number_str)

      call replace_substring( edit_line, lower_case_line, line_str, trim( line_number_str), &
                              search_idx)

      remark_line: if( options% report_extensions )then

         call msg_continue( "edited line: " // trim( line_number_str) )

      endif remark_line

   endif go_line

! ----------------------------------------------------------------------

!  edit_file_line_strings() exit

return                                                               ! edit_file_line_strings()

! **********************************************************************

!  edit_file_line_strings()

end subroutine edit_file_line_strings

! **********************************************************************
! **********************************************************************

!  edit_date_time_strings() process ?date? & ?time? strings

subroutine edit_date_time_strings( edit_line, lower_case_line)

! **********************************************************************

!  edit_date_time_strings() interface

! ----------------------------------------------------------------------

!  the source line to be edited

character( len= *), intent( inout) :: edit_line

!  the source line in lower case to enable searches

character( len= *), intent( inout) :: lower_case_line

! **********************************************************************

!  entry: line is a line of Fortran source with (possibly) ?integer

!  exit: line has any ?name strings replaced with their values

! **********************************************************************

!  edit_date_time_strings() local

! ----------------------------------------------------------------------

!  strings to be edited into the line- they are the exact length needed

   character( len= 8) :: today_str                                   ! date string

   character( len= 10) :: now_str                                    ! time string

!  find substring

   integer :: search_idx                                             ! go if needed

! **********************************************************************

!  edit_date_time_strings() text

! ----------------------------------------------------------------------

continue                                                             ! edit_date_time_strings()

! ----------------------------------------------------------------------

!  replace ?date? with the current date

   search_idx = index( lower_case_line, date_str)

   go_date: if( search_idx > 0 )then

      call date_and_time( date= today_str)                           ! make call once

      call replace_substring( edit_line, lower_case_line, date_str, today_str, search_idx)

      remark_date: if( options% report_extensions )then

         call msg_continue( "edited date: " // trim( today_str) )

      endif remark_date

   endif go_date

!  replace ?time? with the current time

   search_idx = index( lower_case_line, time_str)

   go_time: if( search_idx > 0 )then

      call date_and_time( time= now_str)                             ! make call once

      call replace_substring( edit_line, lower_case_line, time_str, now_str, search_idx)

      remark_time: if( options% report_extensions )then

         call msg_continue( "edited time: " // trim( now_str) )

      endif remark_time

   endif go_time

! ----------------------------------------------------------------------

!  edit_date_time_strings() exit

return                                                               ! edit_date_time_strings()

! **********************************************************************

!  edit_date_time_strings()

end subroutine edit_date_time_strings

! **********************************************************************
! **********************************************************************

!  edit_integer_strings() process ?integer? strings

subroutine edit_integer_strings( edit_line, lower_case_line)

! **********************************************************************

!  edit_integer_strings() interface

! ----------------------------------------------------------------------

!  the source line to be edited

character( len= *), intent( inout) :: edit_line

!  the source line in lower case to enable searches

character( len= *), intent( inout) :: lower_case_line                

! **********************************************************************

!  entry: line is a line of Fortran source with (possibly) ?integer?

!  exit: line has any ?name? strings replaced with their values

! **********************************************************************

!  edit_integer_strings() local

! ----------------------------------------------------------------------

!  string containing integer value

   character( len= conversion_len) :: value_str                      ! convert integer value to string

!  target string to be replaced

   character( len= target_len) :: search_str

!  length of target string

   integer :: search_len

!  point to integers on symbol list

   type( symbol_t), pointer :: symbol_ptr                            ! location of symbol in list

!  point to search_str location in line

   integer :: search_idx

! **********************************************************************

!  edit_integer_strings() text

! ----------------------------------------------------------------------

continue                                                             ! edit_integer_strings()

! ----------------------------------------------------------------------

!  replace ?integer? with the current string value of integer

   nullify( symbol_ptr)                                              ! start search at head of list

!  test the occurance of each integer on symbol list

   each_integer: do                                                  ! check each integer

      call get_next_integer( symbol_ptr)                             ! next integer symbol

      end_of_list: if( .not. associated( symbol_ptr) )then           ! until end of list

         exit each_integer                                           ! exit when checked all integers

      endif end_of_list                                              ! until end of list

!  does ?integer? appear on line

      search_str = arg_key // trim( symbol_ptr% name_str) // arg_key

      search_len = len_trim( search_str)

      search_idx = index( lower_case_line, search_str( : search_len))

!  if found the target, try to replace it with its value

      go_integer: if( search_idx > 0 )then

!  if integer has a value

         defined_integer: if( symbol_ptr% defined )then              ! integer has a value

            write( unit= value_str, fmt= conversion_fmt) symbol_ptr% integer_value

            value_str = adjustl( value_str)

!  if integer has no value

         else defined_integer                                        ! integer has a value

            call msg_quit( "edit integer symbol not defined: " // trim( symbol_ptr% name_str) )

         endif defined_integer                                       ! integer has a value

!  go replace the string with its value

         call replace_substring( edit_line, lower_case_line, search_str( : search_len), trim( value_str), &
                                 search_idx)

!  if reporting extensions

         report_integers: if( options% report_extensions )then       ! ?integer? is an extension

            call msg_continue( "edited integer: " // trim( symbol_ptr% name_str) )

         endif report_integers                                       ! ?integer? is an extension

      endif go_integer

   enddo each_integer                                                ! check each integer

! ----------------------------------------------------------------------

!  edit_integer_strings() exit

return                                                               ! edit_integer_strings()

! **********************************************************************

!  edit_integer_strings()

end subroutine edit_integer_strings

! **********************************************************************
! **********************************************************************

!  edit_logical_strings() process ?logical? strings

subroutine edit_logical_strings( edit_line, lower_case_line)

! **********************************************************************

!  edit_logical_strings() interface

! ----------------------------------------------------------------------

!  the source line to be edited

character( len= *), intent( inout) :: edit_line

!  the source line in lower case to enable searches

character( len= *), intent( inout) :: lower_case_line                     

! **********************************************************************

!  entry: line is a line of Fortran source with (possibly) ?logical

!  exit: line has any ?name strings replaced with their values

! **********************************************************************

!  edit_logical_strings() local

! ----------------------------------------------------------------------

!  string containing logical value

   character( len= conversion_len) :: value_str                      ! '.true.' or '.false.'

!  target string to be replaced

   character( len= target_len) :: search_str                         ! '?logical?'

!  length of target string

   integer :: search_len                                             ! len_trim( '?logical?')

!  point to logicals on symbol list

   type( symbol_t), pointer :: symbol_ptr                            ! location of symbol in list

!  point to search_str location in line

   integer :: search_idx                                             ! where '?logical?' was found

! **********************************************************************

!  edit_logical_strings() text

! ----------------------------------------------------------------------

continue                                                             ! edit_logical_strings()

! ----------------------------------------------------------------------

!  replace ?logical? with the current string value of logical

   nullify( symbol_ptr)                                              ! start at head of list

   each_logical: do                                                  ! check each logical

      call get_next_logical( symbol_ptr)

      end_of_list: if( .not. associated( symbol_ptr) )then

         exit each_logical

      endif end_of_list

!  does ?logical? appear on line

      search_str = arg_key // trim( symbol_ptr% name_str) // arg_key

      search_len = len_trim( search_str)

      search_idx = index( lower_case_line, search_str( : search_len))

      go_logical: if( search_idx > 0 )then

!  if logical has a value

         defined_logical: if( symbol_ptr% defined )then              ! has a value

            decode: if( symbol_ptr% logical_value )then              ! choose value string

               value_str = true_str

            else decode                                              ! choose value string

               value_str = false_str

            endif decode                                             ! choose value string

            call replace_substring( edit_line, lower_case_line, search_str( : search_len), &
                                    trim( value_str), search_idx)

!  if logical has no value

         else defined_logical                                        ! has a value

            call msg_quit( "edit logical symbol not defined: " // trim( symbol_ptr% name_str) )

         endif defined_logical                                       ! has a value

!  if reporting extensions

         report_logicals: if( options% report_extensions )then       ! ?logical? is an extension

            call msg_continue( "edited logical: " // trim( symbol_ptr% name_str) )

         endif report_logicals                                       ! ?logical? is an extension

      endif go_logical

   enddo each_logical                                                ! check each integer

! ----------------------------------------------------------------------

!  edit_logical_strings() exit

return                                                               ! edit_logical_strings()

! **********************************************************************

!  edit_logical_strings()

end subroutine edit_logical_strings

! **********************************************************************
! **********************************************************************

!  edit_macro_strings() process ?macro? strings

subroutine edit_macro_strings( edit_line, lower_case_line)

! **********************************************************************

!  edit_macro_strings() interface

! ----------------------------------------------------------------------

!  the osurce line to be edited

character( len= *), intent( inout) :: edit_line

!  the source line in lower case to enable searches

character( len= *), intent( inout) :: lower_case_line             

! **********************************************************************

!  entry: line is a line of Fortran source with (possibly) ?macro

!  exit: line has any ?macro strings replaced with their values

! **********************************************************************

!  edit_macro_strings() local

! ----------------------------------------------------------------------

!  string containing macro value

   character( len= target_len) :: search_str

!  length of ?macro?

   integer :: search_len

!  point to ?macro?

   integer :: search_idx

!  argument strings

   character( len= buffer_len) :: actual_args                        ! actual args

   character( len= buffer_len) :: value_str                          ! value after arg substitution

!  scan for macros

   type( symbol_t), pointer :: macro_ptr                             ! point to target

!  end of substrings

   integer :: close_paren_idx                                        ! point to close paren

   integer :: open_paren_idx                                         ! where there should be an open paren

   integer :: value_len                                              ! length of macro value

! **********************************************************************

!  edit_macro_strings() text

! ----------------------------------------------------------------------

continue                                                             ! edit_macro_strings()

! ----------------------------------------------------------------------

!  replace ?macro? with the current string value of macro

   nullify( macro_ptr)                                               ! start at head of list

   each_macro: do                                                    ! check each macro

      call get_next_macro( macro_ptr)                                ! check macros only

      end_of_list: if( .not. associated( macro_ptr) )then            ! quit when end of list reached

         exit each_macro                                             ! no more macros to check

      endif end_of_list                                              ! quit when end of list reached

! ----------------------------------------------------------------------

!  does ?macro? appear on line

      search_str = arg_key // trim( macro_ptr% name_str) // arg_key

      search_len = len_trim( search_str)

      search_idx = index( lower_case_line, search_str( : search_len))

! ----------------------------------------------------------------------

!  if macro appears on line

      found_macro: if( search_idx > 0 )then

!  macro definition has a dummy arg list

         have_arg_list: if( associated( macro_ptr% dummy_args) )then

!  must rebuild the macro value with each new set of actual args

            next_dummy_args: do while( search_idx > 0 )

!  check for actual arg list

               open_paren_idx = search_idx + search_len

               no_actual_args: if( edit_line( open_paren_idx: open_paren_idx) /= open_paren )then

                  call msg_quit( "macro args missing: " // trim( macro_ptr% name_str) )

               endif no_actual_args                                  ! dummy but no actuals

!  have an actual arg list, find the close parenthesis

               call seek_close_paren( edit_line, open_paren_idx, close_paren_idx)

               actual_args = lower_case_line( open_paren_idx + 1: close_paren_idx - 1)

!  build the new macro value

               call process_actual_arglist( actual_args( : close_paren_idx - open_paren_idx - 1), &
                                            value_str, macro_ptr)

!  substitute it

               value_len = len_trim( value_str)                      ! length of edited macro value

!  replace whole "?macro?(args)" with computed macro value

               edit_line = edit_line( : search_idx - 1) // value_str( : value_len) &
                           // edit_line( close_paren_idx + 1: )

               lower_case_line = lower_case_line( : search_idx - 1) // value_str( : value_len) &
                                 // lower_case_line( close_paren_idx + 1: )

!  find the next occurance of ?macro?

               search_idx = index( lower_case_line, trim( search_str( : search_len)) )

            enddo next_dummy_args                                    ! ?macro?( new )

! ----------------------------------------------------------------------

!  no arg list so macro value doesn't change

         else have_arg_list                                          ! arglist or not

!  insert macro into the line

            value_len = len_trim( macro_ptr% macro_value)            ! length of macro value

            call replace_substring( edit_line, lower_case_line, search_str( : search_len), &
                                    macro_ptr% macro_value( : value_len), search_idx)

         endif have_arg_list                                         ! arglist or not

!  if reporting extensions

         report_macros: if( options% report_extensions )then         ! macros are extensions

            call msg_continue( "edited macro: " // trim( macro_ptr% name_str) )

         endif report_macros                                         ! macros are extensions

! ----------------------------------------------------------------------

!  done with this macro

      endif found_macro

 ! ----------------------------------------------------------------------

!  go try the next macro

  enddo each_macro                                                  ! check each macro

! ----------------------------------------------------------------------

!  edit_macro_strings() exit

return                                                               ! edit_macro_strings()

! **********************************************************************

!  edit_macro_strings()

end subroutine edit_macro_strings

! **********************************************************************
! **********************************************************************

!  process_actual_arglist() process macro dummy arglist strings

subroutine process_actual_arglist( actual_args, value_str, symbol_ptr)

! **********************************************************************

!  process_actual_arglist() interface

! ----------------------------------------------------------------------

!  the comma separated actual args from the macro instance

character( len= *), intent( in) :: actual_args

!  the value of the macro after editing

character( len= *), intent( out) :: value_str

!  a pointer to the macro

type( symbol_t), pointer :: symbol_ptr                              

! **********************************************************************

!  entry: arg_list is an actual argument list
!         macro is a macro variable

!  exit: value_buf has the macro's value with all dummy args replaced by actuals

! **********************************************************************

!  process_actual_arglist() local

! ----------------------------------------------------------------------

!  construct macro text with actual args

   character( len= buffer_len) :: actual_str                         ! macro buffer

   character( len= target_len) :: search_str

   integer :: search_len

! ----------------------------------------------------------------------

!  character pointers

   integer :: this_dummy

   integer :: this_char                                              ! index dummy_args

   integer :: dummy_idx                                              ! dummy arg in macro value

   integer :: close_paren_idx

! **********************************************************************

!  process_actual_arglist() text

! ----------------------------------------------------------------------

continue                                                             ! process_actual_arglist()

! ----------------------------------------------------------------------

!  the value will be edited with the actual args

   value_str = symbol_ptr% macro_value                               ! macro value

   actual_str = actual_args                                          ! work with local

!  put a comma at the end of the actual arglist to process all the actual args

   actual_str( len( actual_args) + 1: len( actual_args) + 1) = comma

!  if checking, verify that all actual args are enclosed in parenthesis

   check_no_parens: if( options% args_in_parens )then

      call verify_actual_parens( actual_str)

   endif check_no_parens

!  loop thru each dummy arg

   each_arg: do this_dummy = 1, size( symbol_ptr% dummy_args)

!  prepare the ?dummy? string

      search_str = arg_key // trim( symbol_ptr% dummy_args( this_dummy) ) // arg_key

      search_len = len_trim( search_str)

!  scan through the actual arguments string to find a comma outside parenthesis

      this_char = 1                                                  ! start with first character

      each_char: do                                                  ! scan actual string

!  find a comma outside parenthesis

         find_actual: select case( actual_str( this_char: this_char))

!  at open paren, skip to matching paren

         case( open_paren) find_actual                               ! found '('

            call seek_close_paren( actual_str, this_char, close_paren_idx)

            this_char = close_paren_idx + 1                          ! next after matching ')'

!  actual argument is isolated at comma outside parenthesis

         case( comma) find_actual                                    ! found the entire actual arg

            dummy_idx = index( value_str, search_str( : search_len) )

            substitute: if( dummy_idx > 0 )then                      ! replace dummy with actual

               call replace_substring( lower_case_str= value_str, search_str= search_str( : search_len), &
                                       replace_str= actual_str( : this_char - 1), first_idx= dummy_idx)

               dummy_idx = index( value_str, search_str( : search_len))

            endif substitute                                         ! replace dummy with actual

            actual_str = adjustl( actual_str( this_char + 1: ))      ! shift off that actual arg & comma

            exit each_char                                           ! done with this arg

!  otherwise, keep checking characters

         case default find_actual                                    ! any other char

            this_char = this_char + 1                                ! next character

         end select find_actual                                      ! which character

      enddo each_char                                                ! check each character

   enddo each_arg                                                    ! process each dummy arg

! ----------------------------------------------------------------------

!  process_actual_arglist() exit

return                                                               ! process_actual_arglist()

! **********************************************************************

!  process_actual_arglist()

end subroutine process_actual_arglist

! **********************************************************************
! **********************************************************************

!  verify_actual_parens() verify that all actual arguments have enclosing parenthesis

subroutine verify_actual_parens( actual_args)

! **********************************************************************

!  verify_actual_parens() interface

! ----------------------------------------------------------------------

!  the comma separated actual args from the macro instance

character( len= *), intent( in) :: actual_args

! **********************************************************************

!  entry: actual_args

!  exit: if all actual args are enclosed in paren, or need not be

! **********************************************************************

!  verify_actual_parens() local

! ----------------------------------------------------------------------

!  construct macro text with actual args

   character( len= buffer_len) :: actual_str                         ! all actual args

   character( len= buffer_len) :: one_actual_arg                     ! one suspect actual arg

! ----------------------------------------------------------------------

!  character pointers

   integer :: this_char                                              ! index dummy_args

   logical :: all_chars_ok                                           ! so far anyway

   integer :: end_skip_idx

! **********************************************************************

!  verify_actual_parens() text

! ----------------------------------------------------------------------

continue                                                             ! verify_actual_parens()

! ----------------------------------------------------------------------

!  the value will be edited with the actual args

   actual_str = actual_args                                          ! work with local

!  loop thru each dummy arg

   each_arg: do while( actual_str /= blank)

!  scan through the actual arguments string to find a comma outside parenthesis

      all_chars_ok = .true.                                          ! so far so good

      this_char = 1                                                  ! start at the beginning

      each_char: do                                                  ! scan through string

!  find a comma outside parenthesis

         check_char: select case( actual_str( this_char: this_char))

!  at open paren, skip to matching paren

         case( open_paren) check_char                                ! found '('

            call seek_close_paren( actual_str, this_char, end_skip_idx)

            this_char = end_skip_idx + 1                             ! next after matching ')'

!  at quote, skip to matching quote

         case( single_quote, double_quote) check_char                ! found '"' or "'"

            call seek_match_quote( actual_str, this_char, end_skip_idx)

            this_char = end_skip_idx                                 ! next after matching '"' or "'"

!  at allowable character, skip it

         case( 'a': 'z', '0': '9', '_', '%', blank) check_char       ! found ok char

            this_char = this_char + 1                                ! next after ok char

!  actual argument is isolated at comma outside parenthesis

         case( comma) check_char                                     ! found the entire actual arg

            one_actual_arg = actual_str( : this_char - 1)            ! save for possible message

            actual_str = actual_str( this_char + 1: )                ! shift off this actual arg

            exit each_char                                           ! done with this arg

!  otherwise, keep checking characters

         case default check_char                                     ! any other char

            all_chars_ok = .false.

            this_char = this_char + 1

         end select check_char                                       ! which character

      enddo each_char                                                ! check each character

      got_bad_char: if( .not. all_chars_ok )then

         call msg_continue( 'actual argument may need parenthesis: ' // trim( one_actual_arg))

      endif got_bad_char

   enddo each_arg                                                    ! process each dummy arg

! ----------------------------------------------------------------------

!  verify_actual_parens() exit

return                                                               ! verify_actual_parens()

! **********************************************************************

!  verify_actual_parens()

end subroutine verify_actual_parens

! **********************************************************************
! **********************************************************************

!  wrap_source_line() ensure lines are not too long

subroutine wrap_source_line( wrap_line)

! **********************************************************************

!  wrap_source_line() interface

! ----------------------------------------------------------------------

!  the line to be wrapped

character( len= *), target, intent( inout) :: wrap_line           

! **********************************************************************

!  entry: line is a line of Fortran source with (possibly) more than 132 characters

!  exit: line has continuations written and fewer than 132 source lines

! **********************************************************************

!  wrap_source_line() constants

! ----------------------------------------------------------------------

!  break after one of these

   character( len= *), parameter :: operators = '+-*/=,()%'          ! operators & separators

   character( len= *), parameter :: break_point = operators // white_space

   character( len= *), parameter :: blank_str = repeat( blank, fixed_format_len)

!  start in the right column for fixed format

   integer, parameter :: start_col = 5                               ! put continuation char after here

! **********************************************************************

!  wrap_source_line() local

! ----------------------------------------------------------------------

!  length of source line

   integer :: output_len                                             ! length of output line

!  location to wrap lines

   integer :: break_idx                                              ! find break character

!  flag wrapping

   logical :: did_wrap                                               ! if actually wrapped a line

! **********************************************************************

!  wrap_source_line() text

! ----------------------------------------------------------------------

continue                                                             ! wrap_source_line()

! ----------------------------------------------------------------------

!  initialize

   did_wrap = .false.                                                ! haven't yet

   output_len = len_trim( wrap_line)                                 ! compute line length

!  while line is too long

   wrap_lines: do while( output_len > options% wrap_length )         ! while line is over length

!  break at a break point before the maximum length

      break_idx = scan( wrap_line( : options% wrap_length - 1), break_point, back= .true.)

      no_break: if( break_idx == 0 )then                             ! if can't find a break

         break_idx = options% wrap_length                            ! break at the end

      endif no_break                                                 ! if can't find a break

!  process fixed format differently

      fix_length: if( options% wrap_length == fixed_format_len )then ! fixed or free format wrapping

!  fixed format line up to the breakpoint, then blanks to column 72, then & in column 73

         line = wrap_line( : break_idx - 1) // blank_str( break_idx: fixed_format_len) // continuation

!  blanks up to column 6, then &, then the rest of the line

         wrap_line = blank_str( : start_col) // continuation // line( break_idx: )

!  process free format

      else fix_length                                                ! fixed or free format wrapping

!  free format line up to the breakpoint, then the continuation character

         line = wrap_line( : break_idx - 1) // continuation          ! make new line

! the &, then the rest of the line

         wrap_line = continuation // line( break_idx: )              ! the remainder of the line

      endif fix_length                                               ! fixed or free format wrapping

!  write lines as they are made

      call write_source_line( output_file)                           ! write the continued line

      output_len = len_trim( line)                                   ! update line length

      did_wrap = .true.                                              ! actually wrapped a line

!  while line is too long

   enddo wrap_lines                                                  ! while line is over length

! ----------------------------------------------------------------------

!  if reporting extensions

   extensions: if( did_wrap .and. options% report_extensions )then   ! remark wrapping

      call msg_continue( "wrapped source line")                      ! file: line: in msg_continue()

   endif extensions                                                  ! remark wrapping

! ----------------------------------------------------------------------

!  wrap_source_line() exit

return                                                               ! wrap_source_line()

! **********************************************************************

!  wrap_source_line()

end subroutine wrap_source_line

! **********************************************************************
! **********************************************************************

!  blank_compress_lower_case() blank compress or convert to lower case

subroutine blank_compress_lower_case( out_str, in_str)

! **********************************************************************

!  blank_compress_lower_case() interface

! ----------------------------------------------------------------------

!  a coco line with blanks, quoted strings and comments

character( len= *), intent( in) :: in_str

!  a blank compressed lower case coco statement

character( len= *), intent( out) :: out_str                         

! **********************************************************************

!  entry: in_str is a coco line

!  exit: out_str is a coco statement, possibly complete only up to hwm

! **********************************************************************

!  blank_compress_lower_case() local

! ----------------------------------------------------------------------

!  quote used for the current quoted string

   character( len= 1), save :: quote                                 ! either single quote or double quote

!  length of in_str

   integer :: in_str_len                                             ! len_trim( in_str)

!  input pointer reset for each line

   integer :: in_idx                                                 ! input character pointer

!  output pointer reset for each statement

   integer, save :: out_idx = 0                                      ! output character pointer

!  character pointer reset by each intrinsic use

   integer :: char_idx                                               ! intrinsic result

! **********************************************************************

!  blank_compress_lower_case() text

continue                                                             ! blank_compress_lower_case()

! ----------------------------------------------------------------------

!  null input signals end-of-file reached on input file

   cleanup: if( in_str == null_string )then                          ! reset at eof

      out_idx = 0                                                    ! set flag

      return                                                         ! quit

   endif cleanup                                                     ! reset at eof

! ----------------------------------------------------------------------

!  initialize line length

   in_str_len = len_trim( in_str)                                    ! quit when at end of input string

!  setup pointers

   initialize: if( out_idx == 0 )then                                ! initial line

      in_idx = 1                                                     ! scan whole line

      out_idx = 1                                                    ! write whole statement

      out_str = null_string                                          ! initialize statement

      quote = null_string                                            ! not in quoted string

   else initialize                                                   ! continuation line

      char_idx = verify( in_str, white_space)                        ! skip any whitespace

!  check whether first character is a continuation

      skip_contin: if( in_str( char_idx: char_idx) == continuation )then

         in_idx = char_idx + 1                                       ! pass continuation character

      else skip_contin                                               ! continuation

         in_idx = char_idx                                           ! use copy first character

      endif skip_contin                                              ! continuation

   endif initialize                                                  ! initial or continuation

! **********************************************************************

!  scan each character until end of input string

   scan_line: do while( in_idx <= in_str_len)                        ! process initial line

! ----------------------------------------------------------------------

!  if in quoted string

      char_literal: select case( quote)                              ! within quoted string or not

! ----------------------------------------------------------------------

      case( single_quote, double_quote) char_literal                 ! within quoted string

!  if found matching quote

         end_of_string: if( in_str( in_idx: in_idx) == quote )then   ! matching quote

!  out of string so set quote to null

            quote = null_string                                      ! found matching quote, reset

         endif end_of_string                                         ! found matching single quote

!  copy the character

         out_str( out_idx: out_idx) = in_str( in_idx: in_idx)        ! copy character

!  update the pointers

         in_idx = in_idx + 1                                         ! update in pointer

         out_idx = out_idx + 1                                       ! update out pointer

! ----------------------------------------------------------------------

!  not in quoted string

      case default char_literal                                      ! within quoted string or not

!  white space is not copied

         skip_ws: select case( in_str( in_idx: in_idx) )             ! check each character

!  blanks or tabs

         case( blank, tab) skip_ws                                   ! found white space

            in_idx = in_idx + 1                                      ! skip white space

!  all others

         case default skip_ws                                        ! all non white space

!  check for special characters
 
            spec_char: select case( in_str( in_idx: in_idx) )        ! check each character

!  found quoted string

            case( single_quote, double_quote) spec_char              ! quote starts a quoted string

               quote = in_str( in_idx: in_idx)                       ! match this quote

!  found coco comment

            case( comment) spec_char                                 ! comment starts a comment

               exit scan_line                                        ! quit when comment found

            end select spec_char                                     ! check each character

! ----------------------------------------------------------------------

!  copy non-blank characters

            copy_char: select case( in_str( in_idx: in_idx) )        ! check each character

!  copy the upper case character as lower case characters

            case( 'A': 'Z') copy_char                                ! upper case to lower case copy

               out_str( out_idx: out_idx) = achar( iachar( in_str( in_idx: in_idx)) + change_case)

            case default copy_char                                   ! upper case to lower case copy

!  copy the other non-blank character unchanged

               out_str( out_idx: out_idx) = in_str( in_idx: in_idx)  ! others unchanged

            end select copy_char                                     ! upper case to lower case copy

!  update pointers

            in_idx = in_idx + 1                                      ! update in pointer

            out_idx = out_idx + 1                                    ! update out pointer

         end select skip_ws                                          ! skip white space

! ----------------------------------------------------------------------

      end select char_literal                                        ! within quoted string or not

!  process next character

   enddo scan_line                                                   ! process initial line

! ----------------------------------------------------------------------

!  check whether last character is continuation

   line_complete: if( out_str( out_idx - 1: out_idx - 1) == continuation )then

!  next line is a continuation line

      out_idx = out_idx - 1                                          ! if continuation, mark end of statement so far

   else line_complete                                                ! continue or not

!  next line is an initial line

      out_idx = 0                                                    ! ready for next statement

   endif line_complete                                               ! continue or not

! ----------------------------------------------------------------------

!  blank_compress_lower_case() exit

return                                                               ! blank_compress_lower_case()

! **********************************************************************

!  blank_compress_lower_case()

end subroutine blank_compress_lower_case

! **********************************************************************
! **********************************************************************

!  process_coco_statement() process a coco directive

subroutine process_coco_statement( coco_stmt)

! **********************************************************************

!  process_coco_statement() interface

! ----------------------------------------------------------------------

!  the coco statement to be processed

character( len= *), intent( in) :: coco_stmt

! **********************************************************************

!  entry: coco_stmt is a blank_compress_lower_case coco directive past the coco key
!         "stop..." | "message..." | "if..." | "elseif..." | "else..." |
!         "endif..." | "integer..." | "logical..." | "assert..." | "<name>=..." |
!         "text..." | "copy..." | "dump" | "options" | "report" |
!         "assertif(" | "copyif(" | "ifndef(" | "ifdef(" | "undefine:" | "doc" |
!         "output"

!  exit: the directive is processed or error exit

!  If a directive might have something after the keyword, the keyword
!  match is checked by "keyword( : len( keyword) ) == string", otheriwse,
!  if the directive must not have anything after the keyword, the
!  keyword match is checked by "keyword == string".  Thus, a directive
!  with unknown nonblank characters after the keyword is an unknown directive.

! **********************************************************************

!  process_coco_statement() local

! ----------------------------------------------------------------------

!  point to location of symbol

   type( symbol_t), pointer :: symbol_ptr                            ! symbol pointer

!  possible index of equals

   integer :: eq_idx                                                 ! seek the equals

!  expression string is after the equals

   integer :: expr_idx                                               ! expression follows the equals

! **********************************************************************

!  process_coco_statement() text

continue                                                             ! process_coco_statement()

! ----------------------------------------------------------------------

!  detect assignment statements assigning to variables named by keywords

   nullify( symbol_ptr)                                              ! initialize pointer

   eq_idx = scan( coco_stmt( : symbol_name_len + len( equals)), equals)

   got_equals: if( eq_idx > 0 )then                                  ! if found '='

      call seek_symbol_name( coco_stmt( : eq_idx - 1), symbol_ptr)   ! seek name of symbol

   endif got_equals                                                  ! if found '='

! ----------------------------------------------------------------------

!  which directive?

! ----------------------------------------------------------------------

!  assignment directive

   which_directive: if( associated( symbol_ptr) )then

!  up to the equals must be a declared name

      expr_idx = eq_idx + len( equals)                               ! start of expression

!  must be an integer or logical variable

      integer_or_logical: select case( symbol_ptr% type_code)        ! which type
         
      case( type_integer) integer_or_logical                         ! assign to integer

         call process_integer_assignment( coco_stmt( expr_idx: ), symbol_ptr)

      case( type_logical) integer_or_logical                         ! assign to logical

         call process_logical_assignment( coco_stmt( expr_idx: ), symbol_ptr)

      case default integer_or_logical                                ! must assign to integer or logical

         call msg_quit( "variable must be an integer or a logical: " // trim( symbol_ptr% name_str) )
         
      end select integer_or_logical                                  ! which type

      nullify( symbol_ptr)                                           ! done

! ----------------------------------------------------------------------

!  stop directive

   elseif( coco_stmt == stop_str )then which_directive

      call process_stop_directive( coco_stmt( len( stop_str) + 1: ) )

! ----------------------------------------------------------------------

!  message directive

   elseif( coco_stmt( : len( message_str)) == message_str )then which_directive

      call process_message_directive( coco_stmt( len( message_str) + 1: ) )

! ----------------------------------------------------------------------

!  if directive

   elseif( coco_stmt( : len( if_str)) == if_str )then which_directive

      call process_if_directive( coco_stmt( len( if_str) + 1: ) )    ! go process if

! ----------------------------------------------------------------------

!  elseif directive

   elseif( coco_stmt( : len( elseif_str)) == elseif_str )then which_directive

      call process_elseif_directive( coco_stmt( len( elseif_str) + 1: ) )

! ----------------------------------------------------------------------

!  else directive

   elseif( coco_stmt( : len( else_str)) == else_str )then which_directive

      call process_else_directive( coco_stmt( len( else_str) + 1: ) )

! ----------------------------------------------------------------------

!  endif directive

   elseif( coco_stmt( : len( endif_str)) == endif_str )then which_directive

      call process_endif_directive( coco_stmt( len( endif_str) + 1: ) )

! ----------------------------------------------------------------------

!  ifdef directive

   elseif( coco_stmt( : len( ifdef_str)) == ifdef_str )then which_directive

      call process_ifdef_directive( coco_stmt( len( ifdef_str) + 1: ) )

! ----------------------------------------------------------------------

!  ifndef directive

   elseif( coco_stmt( : len( ifndef_str)) == ifndef_str )then which_directive

      call process_ifndef_directive( coco_stmt( len( ifndef_str) + 1: ) )

! ----------------------------------------------------------------------

!  integer declaration

   elseif( coco_stmt( : len( integer_str)) == integer_str )then which_directive

      call process_integer_declaration( coco_stmt( len( integer_str) + 1: ))

! ----------------------------------------------------------------------

!  integer constant declaration

   elseif( coco_stmt( : len( integer_constant_str)) == integer_constant_str )then which_directive

      call process_integer_constant( coco_stmt( len( integer_constant_str) + 1: ))

! ----------------------------------------------------------------------

!  logical declaration

   elseif( coco_stmt( : len( logical_str)) == logical_str )then which_directive

      call process_logical_declaration( coco_stmt( len( logical_str) + 1: ))

! ----------------------------------------------------------------------

!  logical constant declaration

   elseif( coco_stmt( : len( logical_constant_str)) == logical_constant_str )then which_directive

      call process_logical_constant( coco_stmt( len( logical_constant_str) + 1: ))

! ----------------------------------------------------------------------

!  macro declaration

   elseif( coco_stmt( : len( macro_str)) == macro_str )then which_directive

      call process_macro_declaration( coco_stmt( len( macro_str) + 1: ))

! ----------------------------------------------------------------------

!  undefine declaration

   elseif( coco_stmt( : len( undefine_str)) == undefine_str )then which_directive

      call process_undefine_directive( coco_stmt( len( undefine_str) + 1: ))

! ----------------------------------------------------------------------

!  assertif directive -- assertif must come before assert due to leading substring

   elseif( coco_stmt( : len( assertif_str)) == assertif_str )then which_directive

      call process_assertif_directive( coco_stmt( len( assertif_str): ))

! ----------------------------------------------------------------------

!  assert directive

   elseif( coco_stmt( : len( assert_str)) == assert_str )then which_directive

      call process_assert_directive( coco_stmt( len( assert_str) + 1: ))

! ----------------------------------------------------------------------

!  dump directive

   elseif( coco_stmt == dump_str )then which_directive

      call process_dump_directive

! ----------------------------------------------------------------------

!  options directive

   elseif( coco_stmt == options_str )then which_directive

      call write_options

! ----------------------------------------------------------------------

!  report directive

   elseif( coco_stmt == report_str )then which_directive

      call write_report

! ----------------------------------------------------------------------

!  text directive

   elseif( coco_stmt( : len( text_str)) == text_str )then which_directive

      call process_text_directive( coco_stmt( len( text_str) + 1: ))

! ----------------------------------------------------------------------

!  copy directive

   elseif( coco_stmt( : len( copy_str)) == copy_str )then which_directive

      call process_copy_directive( coco_stmt( len( copy_str) + 1: ))

! ----------------------------------------------------------------------

!  copyif directive

   elseif( coco_stmt( : len( copyif_str)) == copyif_str )then which_directive

      call process_copyif_directive( coco_stmt( len( copyif_str): ))

! ----------------------------------------------------------------------

!  doc directive

   elseif( coco_stmt( : len( doc_str)) == doc_str )then which_directive

      call process_doc_directive( coco_stmt( len( doc_str) + 1: ))

! ----------------------------------------------------------------------

!  output directive

   elseif( coco_stmt( : len( output_str)) == output_str )then which_directive

      call process_output_directive( coco_stmt( len( output_str) + 1: ))

! ----------------------------------------------------------------------

!  cannot process this directive

   else which_directive                                              ! process directives

      call msg_quit( "unknown coco directive: " // trim( coco_stmt))

! ----------------------------------------------------------------------

!  which directive?

   endif which_directive                                             ! process directives

! ----------------------------------------------------------------------

!  process_coco_statement() exit

return                                                               ! process_coco_statement()

! **********************************************************************

!  process_coco_statement()

end subroutine process_coco_statement

! **********************************************************************
! **********************************************************************

!  process_integer_assignment() process a coco stop directive

subroutine process_integer_assignment( assign_dir, integer_ptr)

! **********************************************************************

!  process_integer_assignment() interface

! ----------------------------------------------------------------------

!  the integer assignment directive

character( len= *), intent( in) :: assign_dir

!  a pointer to the integer symbol

type( symbol_t), pointer :: integer_ptr

! **********************************************************************

!  entry: stop_dir is blank_compress_lower_case coco stop directive, past the coco key word

!  exit: coco processing stops

! **********************************************************************

!  process_integer_assignment() local

! ----------------------------------------------------------------------

!  value is an intent( out) variable

   integer :: value                                                  ! pass to eval_int_expr()

! **********************************************************************

!  process_integer_assignment() text

continue                                                             ! process_integer_assignment()

! ----------------------------------------------------------------------

!  process assignment directive if on an active line

   active_line: if( if_construct% now_selected ) then                ! the line is selected

!  do not allow redefinition of constants

      redefine_constant: if( integer_ptr% constant )then             ! is symbol a constant?

         call msg_quit( "attempt to redefine a constant: " // trim( integer_ptr% name_str) )

      endif redefine_constant                                        ! is symbol a constant?

!  assign the value

      call eval_int_expr( assign_dir, value)

      integer_ptr% integer_value = value

      integer_ptr% defined = .true.                                  ! has a value now

   endif active_line                                                 ! the line is selected

! ----------------------------------------------------------------------

!  process_integer_assignment() exit

return                                                               ! process_integer_assignment()

! **********************************************************************

!  process_integer_assignment()

end subroutine process_integer_assignment

! **********************************************************************
! **********************************************************************

!  process_logical_assignment() process a coco stop directive

subroutine process_logical_assignment( assign_dir, logical_ptr)

! **********************************************************************

!  process_logical_assignment() interface

! ----------------------------------------------------------------------

!  the logical assignment directive

character( len= *), intent( in) :: assign_dir

!  a pointer to the logical symbol

type( symbol_t), pointer :: logical_ptr

! **********************************************************************

!  entry: stop_dir is blank_compress_lower_case coco stop directive, past the coco key word

!  exit: coco processing stops

! **********************************************************************

!  process_logical_assignment() local

! ----------------------------------------------------------------------

!  value is an intent( out) variable

   logical :: value                                                  ! pass to eval_int_expr()

! **********************************************************************

!  process_logical_assignment() text

continue                                                             ! process_logical_assignment()

! ----------------------------------------------------------------------

!  process stop directive if on an active line

   active_line: if( if_construct% now_selected ) then                ! the line is selected

!  do not allow redefinition of constants

      redefine_constant: if( logical_ptr% constant )then             ! is symbol a constant?

         call msg_quit( "attempt to redefine a constant: " // trim( logical_ptr% name_str) )

      endif redefine_constant                                        ! is symbol a constant?

!  assign the value

      call eval_log_expr( assign_dir, value)

      logical_ptr% logical_value = value

      logical_ptr% defined = .true.                                  ! has a value now

   endif active_line                                                 ! the line is selected

! ----------------------------------------------------------------------

!  process_logical_assignment() exit

return                                                               ! process_logical_assignment()

! **********************************************************************

!  process_logical_assignment()

end subroutine process_logical_assignment

! **********************************************************************
! **********************************************************************

!  process_stop_directive() process a coco stop directive

subroutine process_stop_directive( stop_dir)

! **********************************************************************

!  process_stop_directive() interface

! ----------------------------------------------------------------------

!  the stop directive

character( len= *), intent( in) :: stop_dir                         

! **********************************************************************

!  entry: stop_dir is blank_compress_lower_case coco stop directive, past the coco key word

!  exit: coco processing stops

! **********************************************************************

!  process_stop_directive() text

continue                                                             ! process_stop_directive()

! ----------------------------------------------------------------------

!  process stop directive if on an active line

   active_line: if( if_construct% now_selected ) then                ! the line is selected

      verbose_output: if( options% verbose_mode )then                ! write to logfile

         call msg_continue( "coco stop directive encountered: " // trim( stop_dir))

      endif verbose_output                                           ! write to logfile

      output_file% line = stop_dir

      call write_coco_line( output_file)                             ! or as coco line

      stop 'coco stop directive encountered'                         ! stop with message

   endif active_line                                                 ! the line is selected

! ----------------------------------------------------------------------

!  process_stop_directive() exit

return                                                               ! process_stop_directive()

! **********************************************************************

!  process_stop_directive()

end subroutine process_stop_directive

! **********************************************************************
! **********************************************************************

!  process_message_directive() process a coco message directive

subroutine process_message_directive( message_dir)

! **********************************************************************

!  process_message_directive() interface

! ----------------------------------------------------------------------

!  the message directive

character( len= *), intent( in) :: message_dir                       

! **********************************************************************

!  entry: message_dir is blank_compress_lower_case coco message directive, past the message key word

!  exit: message is written to error unit

! **********************************************************************

!  process_message_directive() local

! ----------------------------------------------------------------------

   character( len= buffer_len) :: msg_buffer                         ! build the message

   integer :: in_idx                                                 ! input string index

   integer :: out_idx                                                ! output string index

   integer :: comma_idx                                              ! idndex of comma

   integer :: quoted_len                                             ! length of quoted strings

   integer :: unquoted_len                                           ! length of unquoted strings

   integer :: integer_value                                          ! value of integer expression

   integer :: istat                                                  ! iostat of integer conversion

   logical :: logical_value                                          ! value of logical expression

   logical :: is_integer                                             ! true if expression is an integer expression

   character( len= buffer_len) :: expr_str                           ! expression to be evaluated

   character( len= conversion_len) :: conversion_str                 ! string to receive integer conversion

! **********************************************************************

!  process_message_directive() text

continue                                                             ! process_message_directive()

! ----------------------------------------------------------------------

!  process if on active line

   active_line: if( if_construct% now_selected )then                 ! process active lines only

! ----------------------------------------------------------------------

!  initialize

      in_idx = 1                                                     ! read message list from the beginning
      out_idx = 1                                                    ! write output from the beginning

      msg_buffer = blank                                             ! no output yet

!  loop thru message list items

      list_items: do while( in_idx <= len_trim( message_dir) )       ! while items on list

! ----------------------------------------------------------------------

!  a list item can be a quoted string or an expression

         string_expr: select case( message_dir( in_idx: in_idx) )    ! look at first character

! ----------------------------------------------------------------------

!  process quoted strings

         case( single_quote, double_quote) string_expr               ! first character of a string is a quote

!  try to unquote the string

            call unquote_string( message_dir( in_idx: ), msg_buffer( out_idx: ), quoted_len, unquoted_len)

!  if the matching quote is found within the string

            got_string: if( quoted_len <= len( message_dir( in_idx: )) )then

!  found quote, update the character pointers

               in_idx = in_idx + quoted_len + index( message_dir( in_idx + quoted_len - 1: ), comma) - 1

               out_idx = out_idx + unquoted_len - 1                  ! next output character

!  found not quote, complain and quit

            else got_string                                          ! couldn't unquote string

               call msg_quit( "bad message string: " // trim( message_dir) )

            endif got_string                                         ! couldn't unquote string

! ----------------------------------------------------------------------

!  process expressions

         case( 'a': 'z', '0':'9', dot, plus, minus, open_paren) string_expr

!  expression ends at a comma

            comma_idx = scan( message_dir( in_idx: ), comma)         ! find next comma

!  find the comma or the end of the expression

            end_of_string: if( comma_idx == 0 )then                  ! no comma

               comma_idx = len_trim( message_dir) + 1                ! or end of statement

            else end_of_string                                       ! comma

               comma_idx = in_idx + comma_idx - 1                    ! point to comma

            endif end_of_string                                      ! comma or no comma

!  encode integer or logical

            call integer_or_logical( message_dir( in_idx: comma_idx - 1), is_integer)

!  an integer expression

            int_log: if( is_integer )then                            ! integer or logical

               expr_str = message_dir( in_idx: comma_idx - 1)

               call eval_int_expr( expr_str, integer_value)

               write( unit= conversion_str, fmt= conversion_fmt, iostat= istat) integer_value

               msg_buffer( out_idx: ) = adjustl( conversion_str)

!  a logical expression

            else int_log                                             ! integer or logical

               expr_str = message_dir( in_idx: comma_idx - 1)

               call eval_log_expr( expr_str, logical_value)

               t_or_f: if( logical_value )then

                  msg_buffer( out_idx: ) = '.true.'

               else t_or_f

                  msg_buffer( out_idx: ) = '.false.'

               endif t_or_f

            endif int_log                                            ! integer or logical

!  trap internal write errors

            encode: if( istat > 0 )then                              ! failed to encode

               call msg_quit( "can't encode: " // message_dir( in_idx: comma_idx - 1) )

            endif encode                                             ! failed to encode

!  update pointers and add to output buffer

            adjust: if( msg_buffer( out_idx: out_idx) == blank )then ! space out output

               msg_buffer( out_idx + 1: ) = adjustl( msg_buffer( out_idx + 1: ) )

            endif adjust                                             ! space out output

            in_idx = comma_idx + 1                                   ! input pointer

            out_idx = len_trim( msg_buffer) + 2                      ! leave one blank before and after

! ----------------------------------------------------------------------

!  list item isn't a string, a symbol or a literal

         case default string_expr                                    ! complain and quit

            call msg_quit( "bad message list item: " // message_dir( in_idx: ) )

! ----------------------------------------------------------------------

         end select string_expr                                      ! kind of output item

!  loop thru message list items

      enddo list_items                                               ! while items on list

! ----------------------------------------------------------------------

!  make the message available

      verbose_output: if( options% verbose_mode )then                ! verbose output

         call msg_continue( msg_buffer( : out_idx) )                 ! on stderr (or logfile)

      endif verbose_output                                           ! verbose output

      call wrap_source_line( msg_buffer( : out_idx) )

      line = msg_buffer

      call write_coco_line( output_file)                             ! as coco line

   endif active_line                                                 ! process active lines only

! ----------------------------------------------------------------------

!  process_message_directive() exit

return                                                               ! process_message_directive()

! **********************************************************************

!  process_message_directive()

end subroutine process_message_directive

! **********************************************************************
! **********************************************************************

!  %%% process declarations of integers, logicals, macros and texts

! **********************************************************************
! **********************************************************************

!  get_symbol_name() verify symbol name and determine its length

subroutine get_symbol_name( decl_stmt, symbol_name, name_len)

! **********************************************************************

!  get_symbol_name() interface

! ----------------------------------------------------------------------

!  a declaration statement with a symbol name

character( len= *), intent( in) :: decl_stmt

!  the name of the symbol

character( len= *), intent( out) :: symbol_name

!  the length of the symbol name

integer, intent( out) :: name_len

! **********************************************************************

!  entry: decl_stmt is blank_compress_lower_case declaration statement past the double colon
!         "name" | "name=..."

!  exit: a valid symbol name and its length or error exit

! **********************************************************************

!  get_symbol_name() constants

! ----------------------------------------------------------------------

!  characters which must end a symbol name

   character( len= *), parameter :: end_of_name = equals // blank    ! end of name

! **********************************************************************

!  get_symbol_name() local

! ----------------------------------------------------------------------

!  pointers to characters in decl_stmt

   integer :: char_idx                                               ! illegal character, if found

! **********************************************************************

!  get_symbol_name() text

continue                                                             ! get_symbol_name()

! ----------------------------------------------------------------------

!  look for equals following separator

   char_idx = scan( decl_stmt, end_of_name)                          ! '=' or ' ' must end the symbol

   name_error: if( char_idx == 0 )then                               ! complain if not found

      call msg_quit( "can't find name in declaration: " // trim( decl_stmt))

   endif name_error                                                  ! complain if not found

!  length of name is one less than first character past name

   name_len = char_idx - 1                                           ! point to last char in name

! ----------------------------------------------------------------------

!  check that initial character is alphabetic

   char_idx = verify( decl_stmt( 1: 1), alpha_chars)                 ! a-z

   initial_ok: if( char_idx /= 0 )then                               ! if initial not alphabetic

      call msg_quit( "illegal initial character in symbol name: " // trim( decl_stmt) )

   endif initial_ok                                                  ! if initial not alphabetic

!  check that following characters are legal

   char_idx = verify( decl_stmt( 2: name_len), alphanum_chars)       ! a-z 0-9 _

   name_ok: if( char_idx /= 0 )then                                  ! if character not legal

      call msg_quit( "illegal character in symbol name: " // trim( decl_stmt) )

   endif name_ok                                                     ! if character not legal

!  return name

   symbol_name = decl_stmt( : name_len)                              ! copy name from statement

! ----------------------------------------------------------------------

!  get_symbol_name() exit

return                                                               ! get_symbol_name()

! **********************************************************************

!  get_symbol_name()

end subroutine get_symbol_name

! **********************************************************************
! **********************************************************************

!  get_macro_name() verify macro name and determine its length

subroutine get_macro_name( decl_stmt, macro_name, name_len)

! **********************************************************************

!  get_macro_name() interface

! ----------------------------------------------------------------------

!  the directive containing the macro

character( len= *), intent( in) :: decl_stmt

!  the name of the macro

character( len= *), intent( out) :: macro_name

!  the length of the macro name

integer, intent( out) :: name_len

! **********************************************************************

!  entry: decl_stmt is blank_compress_lower_case declaration statement past the double colon
!         "name=..." | "name(..."

!  exit: name is valid and its length is known or error exit

! **********************************************************************

!  get_macro_name() constants

! ----------------------------------------------------------------------

!  equals or open parenthesis may end a macro name

   character( len= *), parameter :: end_of_name = equals // open_paren

! **********************************************************************

!  get_macro_name() local

! ----------------------------------------------------------------------

!  pointers to characters in decl_stmt

   integer :: char_idx                                               ! illegal character, if found

! **********************************************************************

!  get_macro_name() text

continue                                                             ! get_macro_name()

! ----------------------------------------------------------------------

!  look for equals following separator

   char_idx = scan( decl_stmt, end_of_name)                          ! where is '=' or '('?

!  if no equals or open paren found

   no_eq_op: if( char_idx == 0 )then                                 ! if didn't find '=' or '('

     call msg_quit( "can't find name in macro declaration: " // trim( decl_stmt))

   endif no_eq_op                                                    ! if didn't find end of name

   name_len = char_idx - 1                                           ! point to last char in name

! ----------------------------------------------------------------------

!  check that initial character is alphabetic

   char_idx = verify( decl_stmt( 1: 1), alpha_chars)                 ! a-z

   initial_ok: if( char_idx /= 0 )then                               ! if initial not alphabetic

      call msg_quit( "illegal initial character in macro name: " // trim( decl_stmt) )

   endif initial_ok                                                  ! if initial not alphabetic

!  check that following characters are legal

   char_idx = verify( decl_stmt( 2: name_len), alphanum_chars)       ! a-z 0-9 _

   name_ok: if( char_idx /= 0 )then                                  ! if character not legal

      call msg_quit( "illegal character in macro name: " // trim( decl_stmt) )

   endif name_ok                                                     ! if character not legal

   macro_name = decl_stmt( : name_len)                               ! copy name from statement

! ----------------------------------------------------------------------

!  get_macro_name() exit

return                                                               ! get_macro_name()

! **********************************************************************

!  get_macro_name()

end subroutine get_macro_name

! **********************************************************************
! **********************************************************************

!  get_text_name() verify text name and determine its length

subroutine get_text_name( decl_stmt, text_name, name_len)

! **********************************************************************

!  get_text_name() interface

! ----------------------------------------------------------------------

!  the statement containing the text name

character( len= *), intent( in) :: decl_stmt

!  the text name

character( len= *), intent( out) :: text_name

!  the length of the text name

integer, intent( out) :: name_len                                   

! **********************************************************************

!  entry: decl_stmt is blank_compress_lower_case declaration statement past the double colon
!         "name" | "name(..."

!  exit: name is valid and its length is known or error exit

! **********************************************************************

!  get_text_name() constants

! ----------------------------------------------------------------------

!  blank or open parenthesis may end a name

   character( len= *), parameter :: end_of_name = blank // open_paren

! **********************************************************************

!  get_text_name() local

! ----------------------------------------------------------------------

!  pointers to characters in decl_stmt

   integer :: char_idx                                               ! illegal character, if found

! **********************************************************************

!  get_text_name() text

continue                                                             ! get_text_name()

! ----------------------------------------------------------------------

!  look for equals following separator

   char_idx = scan( decl_stmt, end_of_name)                          ! where is ' ' or '('?

!  if no equals found

   no_eq_op: if( char_idx == 0 )then                                 ! if didn't find ' ' or '('

     call msg_quit( "can't find name in macro statement: " // trim( decl_stmt))

   endif no_eq_op                                                    ! if didn't find end of name

   name_len = char_idx - 1                                           ! point to last char in name

! ----------------------------------------------------------------------

!  check that initial character is alphabetic

   char_idx = verify( decl_stmt( 1: 1), alpha_chars)                 ! a-z

   initial_ok: if( char_idx /= 0 )then                               ! if initial not alphabetic

      call msg_quit( "illegal initial character in text name: " // trim( decl_stmt) )

   endif initial_ok                                                  ! if initial not alphabetic

!  check that following characters are legal

   char_idx = verify( decl_stmt( 2: name_len), alphanum_chars)       ! a-z 0-9 _

   name_ok: if( char_idx /= 0 )then                                  ! if character not legal

      call msg_quit( "illegal character in text name: " // trim( decl_stmt) )

   endif name_ok                                                     ! if character not legal

   text_name = decl_stmt( : name_len)                                ! copy name from statement

! ----------------------------------------------------------------------

!  get_text_name() exit

return                                                               ! get_text_name()

! **********************************************************************

!  get_text_name()

end subroutine get_text_name

! **********************************************************************
! **********************************************************************

!  process_integer_declaration() process integer declarations

subroutine process_integer_declaration( integer_stmt)

! **********************************************************************

!  process_integer_declaration() interface

! ----------------------------------------------------------------------

!  the statement containing the declaration

character( len= *), intent( in) :: integer_stmt                    

! **********************************************************************

!  entry: int_stmt is blank_compress_lower_case integer declaration past the integer keyword
!         "::..." | ",parameter::..."

!  exit: int_stmt is processed or error exit

! **********************************************************************

!  process_integer_declaration() constants

! ----------------------------------------------------------------------

!  mark the end of a definition

   character( len= *), parameter :: end_of_def = comma // blank      ! either may end a definition

! **********************************************************************

!  process_integer_declaration() local

! ----------------------------------------------------------------------

!  string containing a single symbol declaration symbol

   character( len= buffer_len) :: symbol_str                         ! process symbol by symbol

!  name of symbol

   character( len= symbol_name_len) :: symbol_name                          ! maximum length is 32 characters

!  results of decoding statement

   integer :: symbol_len                                             ! length of string decoded

! ----------------------------------------------------------------------

!  point to next character to be decoded

   integer :: next_char                                              ! next character to process

   integer :: def_len                                                ! length of a definition

! **********************************************************************

!  process_integer_declaration() text

continue                                                             ! process_integer_declaration()

   next_char = 1

! ----------------------------------------------------------------------

!  if active line, process the declaration

   active_line: if( if_construct% now_selected )then                 ! process active lines

!  extract all symbols on directive

      all_symbols: do                                                ! all symbols

!  one symbol at a time to the symbol string

         def_len = scan( integer_stmt( next_char: ), end_of_def) + next_char - 1

         symbol_str = integer_stmt( next_char: def_len - 1)          ! up to the comma or blank

!  extract symbol name

         call get_symbol_name( symbol_str, symbol_name, symbol_len)  ! name of integer symbol

!  store symbol in symbol list

         call add_integer( symbol_str( symbol_len + 1: def_len - 1), symbol_name, .false.)

!  comma separates symbols, blank is end of statement

         all_done: if( integer_stmt( def_len: def_len) == blank )then

            exit all_symbols                                         ! at end of statement

         endif all_done                                              ! end of statement

!  move to next symbol

         next_char = def_len + 1                                     ! one character after this one

!  extract all symbols on directive

      enddo all_symbols                                              ! all symbols

!  if active line, process the declaration

   endif active_line                                                 ! process active lines

! ----------------------------------------------------------------------

!  process_integer_declaration() exit

return                                                               ! process_integer_declaration()

! **********************************************************************

!  process_integer_declaration()

end subroutine process_integer_declaration

! **********************************************************************
! **********************************************************************

!  process_integer_constant() process integer declarations

subroutine process_integer_constant( integer_stmt)

! **********************************************************************

!  process_integer_constant() interface

! ----------------------------------------------------------------------

!  the statement containing the declaration

character( len= *), intent( in) :: integer_stmt                    

! **********************************************************************

!  entry: int_stmt is blank_compress_lower_case integer declaration past the integer keyword
!         "::..." | ",parameter::..."

!  exit: int_stmt is processed or error exit

! **********************************************************************

!  process_integer_constant() constants

! ----------------------------------------------------------------------

!  mark the end of a definition

   character( len= *), parameter :: end_of_def = comma // blank      ! either may end a definition

! **********************************************************************

!  process_integer_constant() local

! ----------------------------------------------------------------------

!  string containing a single symbol declaration symbol

   character( len= buffer_len) :: symbol_str                         ! process symbol by symbol

!  name of symbol

   character( len= symbol_name_len) :: symbol_name                          ! maximum length is 32 characters

!  results of decoding statement

   integer :: symbol_len                                             ! length of string decoded

! ----------------------------------------------------------------------

!  point to next character to be decoded

   integer :: next_char                                              ! next character to process

   integer :: def_len                                                ! length of a definition

! **********************************************************************

!  process_integer_constant() text

continue                                                             ! process_integer_constant()

   next_char = 1

! ----------------------------------------------------------------------

!  if active line, process the declaration

   active_line: if( if_construct% now_selected )then                 ! process active lines

!  extract all symbols on directive

      all_symbols: do                                                ! all symbols

!  one symbol at a time to the symbol string

         def_len = scan( integer_stmt( next_char: ), end_of_def) + next_char - 1

         symbol_str = integer_stmt( next_char: def_len - 1)          ! up to the comma or blank

!  extract symbol name

         call get_symbol_name( symbol_str, symbol_name, symbol_len)  ! name of integer symbol

!  store symbol in symbol list

         call add_integer( symbol_str( symbol_len + 1: def_len - 1), symbol_name, .true.)

!  comma separates symbols, blank is end of statement

         all_done: if( integer_stmt( def_len: def_len) == blank )then

            exit all_symbols                                         ! at end of statement

         endif all_done                                              ! end of statement

!  move to next symbol

         next_char = def_len + 1                                     ! one character after this one

!  extract all symbols on directive

      enddo all_symbols                                              ! all symbols

!  if active line, process the declaration

   endif active_line                                                 ! process active lines

! ----------------------------------------------------------------------

!  process_integer_constant() exit

return                                                               ! process_integer_constant()

! **********************************************************************

!  process_integer_constant()

end subroutine process_integer_constant

! **********************************************************************
! **********************************************************************

!  add_integer() store integer declaration in symbol table

subroutine add_integer( int_decl_str, symbol_name, is_const)

! **********************************************************************

!  add_integer() interface

! ----------------------------------------------------------------------

!  the statement containing the declaration

character( len= *), intent( in) :: int_decl_str

!  the symbol name

character( len= *), intent( in) :: symbol_name

!  true if the symbol is a constant

logical, intent( in) :: is_const                                    

! **********************************************************************

!  entry: int_decl_str is blank_compress_lower_case integer declaration statement past the name
!         "" | "=..."
!         sym_name is the symbol name
!         is_const is true if this is a constant declaration
!         processing_set_file is true if this was found in the setfile

!  exit: integer declaration is added to the integer symbol list or error exit

! **********************************************************************

!  add_integer() local

! ----------------------------------------------------------------------

!  pointer to pre-existing symbols and the pointer to the new one

   type( symbol_t), pointer :: symbol_ptr                            ! locate entries on symbol list

!  expression defining integer symbol

   character( len= buffer_len) :: expr_str                           ! to eval_int_expr()

! **********************************************************************

!  add_integer() text

continue                                                             ! add_integer()

! ----------------------------------------------------------------------

!  check if already on integer list

   call seek_symbol_name( symbol_name, symbol_ptr)                   ! search symbol list

   found_name: if( associated( symbol_ptr) )then                     ! if found this name
      
      name_type: select case( symbol_ptr% type_code)                 ! which type
   
      case( type_integer) name_type                                  ! integer
   
         set_defn: if( symbol_ptr% predefined )then                  ! defined in setfile

            call msg_continue( "integer predeclared in setfile: " // trim( symbol_name) )

            param_match: if( is_const .neqv. symbol_ptr% constant )then

               call msg_quit( "inconsistent definition of symbol (constant v. variable): " &
                              // trim( symbol_name) )

            endif param_match

            symbol_ptr% predefined = .false.                         ! only declare once per program

            return                                                   ! don't add second definition

         else set_defn                                               ! defined in setfile

            call msg_quit( "duplicate symbol declaration: " // trim( symbol_name) )

         endif set_defn                                              ! defined in setfile

      case( type_logical) name_type                                  ! name is already logical
   
         call msg_quit( "integer name already defined as logical: " // trim( symbol_name) )

      case( type_macro) name_type                                    ! name is already macro
   
         call msg_quit( "integer name already defined as macro: " // trim( symbol_name) )

      case( type_text) name_type                                     ! name is already text
   
         call msg_quit( "integer name already defined as text: " // trim( symbol_name) )

      end select name_type

   endif found_name

! ----------------------------------------------------------------------

!  insert a new symbol element into the symbol list

   call add_symbol( symbol_name, symbol_ptr)                         ! new entry on symbol list

! ----------------------------------------------------------------------

!  this symbol is an integer

   symbol_ptr% type_code = type_integer                              ! code integer

!  store whether symbol is a parameter

   symbol_ptr% constant = is_const                                   ! true if parameter

!  store whether symbol is declared in the setfile

   symbol_ptr% predefined = processing_set_file                      ! true if processing setfile

!  determine if declaration specifies a value

   got_eq: if( len( int_decl_str) > 0 )then                          ! expr might be a null string

      symbol_ptr% defined = int_decl_str( : len( equals)) == equals

   else got_eq

      symbol_ptr% defined = .false.

   endif got_eq

! ----------------------------------------------------------------------

!  constants must have values

   constant_value: if( symbol_ptr% constant &                        ! if it is a constant
            .and. .not. symbol_ptr% defined )then                    ! or it doesn't have a value

      call msg_quit( "an integer constant must have a value: " &
                     // trim( symbol_name) // trim( int_decl_str) )

   endif constant_value                                              ! if it is a constant

!  decode the value

   process_value: if( symbol_ptr% defined )then                      ! decode value

      all_constants = .true.                                         ! flag non constants

      expr_str = int_decl_str( len( equals) + 1: )                   ! evaluate long integer expression

      call eval_int_expr( expr_str, symbol_ptr% integer_value)

      non_const: if( symbol_ptr% constant .and. .not. all_constants )then

         call msg_quit( "non constant expression used to define integer constant: " &
                        // trim( symbol_name))

      endif non_const                                                ! constant must be defined by constants

   endif process_value                                               ! decode value

! ----------------------------------------------------------------------

!  add_integer() exit

return                                                               ! add_integer()

! **********************************************************************

!  add_integer()

end subroutine add_integer

! **********************************************************************
! **********************************************************************

!  add_symbol() allocate integer declaration in symbol table

subroutine add_symbol( symbol_name, symbol_ptr)

! **********************************************************************

!  add_symbol interface

! ----------------------------------------------------------------------

!  the name of the symbol to be added to the symbol list

character( len= *), intent( in) :: symbol_name

!  a pointer to the new entry

type( symbol_t), pointer :: symbol_ptr                              

! **********************************************************************

!  entry: none

!  exit: symbol_ptr points to a new element ready to be used

! **********************************************************************

!  add_symbol() local

! ----------------------------------------------------------------------

!  end of symbol list

   type( symbol_t), pointer, save :: current_symbol => null()        ! list is empty at start

! ----------------------------------------------------------------------

!  catch allocate errors

   integer :: astat                                                  ! allocation status

! **********************************************************************

!  add_symbol() text

continue                                                             ! add_symbol()

! ----------------------------------------------------------------------

!  if already have symbol list

   start_or_append: if( associated( first_symbol) )then              ! already have a list

      allocate( current_symbol% next, stat= astat)                   ! extend the list

      append_error: if( astat > 0 )then                              ! allocation error

         call msg_quit( "allocate symbol failed: " // trim( symbol_name))

      endif append_error                                             ! allocation error

      current_symbol => current_symbol% next                         ! new last entry

   else start_or_append                                              ! no list yet

      allocate( first_symbol, stat= astat)                           ! start list

      start_error: if( astat > 0 )then                               ! allocation error

         call msg_quit( "allocate symbol failed: " // trim( symbol_name))

      endif start_error                                              ! allocation error

      current_symbol => first_symbol                                 ! first symbol is last symbol

   endif start_or_append

! ----------------------------------------------------------------------

!  update new entry's list components

   current_symbol% name_str = symbol_name                            ! store the name

   current_symbol% type_code = type_none                             ! no type yet

   nullify( current_symbol% next)                                    ! last on list

!  initialize new entry's data components

   current_symbol% defined = .false.                                 ! not yet defined

   current_symbol% constant = .false.                                ! not known to be a constant

   current_symbol% predefined = .false.                              ! not known to be from the setfile

   current_symbol% integer_value = -huge( current_symbol% integer_value)

   current_symbol% logical_value = .false.

   nullify( current_symbol% dummy_args)                              ! set pointer to known state

   current_symbol% macro_value = blank

   nullify( current_symbol% text_lines)                              ! set pointer to known state

! ----------------------------------------------------------------------

!  set the caller's pointer to the new one

   symbol_ptr => current_symbol

! ----------------------------------------------------------------------

!  add_symbol() exit

return                                                               ! add_symbol()

! **********************************************************************

!  add_symbol()

end subroutine add_symbol

! **********************************************************************
! **********************************************************************

!  process_logical_declaration() process logical declarations

subroutine process_logical_declaration( logical_stmt)

! **********************************************************************

!  process_logical_declaration() interface

! ----------------------------------------------------------------------

!  the statement containing the logical declaration

character( len= *), intent( in) :: logical_stmt                    

! **********************************************************************

!  entry: logical_stmt is blank_compress_lower_case logical declaration past the logical keyword
!         "::..." | ",parameter::..."

!  exit: logical declaration is processed or error exit

! **********************************************************************

!  process_logical_declaration() local

! ----------------------------------------------------------------------

!  string containing a single symbol declaration symbol

   character( len= buffer_len) :: symbol_str                         ! process symbol by symbol

!  name of symbol

   character( len= symbol_name_len) :: symbol_name                   ! maximum length is 32 characters

!  results of decoding statement

   integer :: symbol_len                                             ! length of string decoded

! ----------------------------------------------------------------------

!  point to next character to be decoded

   integer :: next_char                                              ! next character to process

   integer :: decl_len                                               ! length of a definition

! **********************************************************************

!  process_logical_declaration() text

continue                                                             ! process_logical_declaration()

   next_char = 1

! ----------------------------------------------------------------------

!  if active line, process the declaration

   active_line: if( if_construct% now_selected )then                 ! if active line

!  extract all symbols on directive

      all_symbols: do                                                ! all symbols

!  one symbol at a time to the symbol string

         decl_len = scan( logical_stmt( next_char: ), end_of_decl) + next_char - 1

         symbol_str = logical_stmt( next_char: decl_len - 1)         ! up to the comma or blank

!  extract symbol name

         call get_symbol_name( symbol_str, symbol_name, symbol_len)  ! name of logical

!  store symbol in symbol list

         call add_logical( symbol_str( symbol_len + 1: decl_len - 1), symbol_name, .false.)

!  comma separates symbols, blank is end of statement

         all_done: if( logical_stmt( decl_len: decl_len) == blank )then

            exit all_symbols

         endif all_done                                              ! at end of statement

!  reset for next symbol

         next_char = decl_len + 1                                    ! one character after this one

!  extract all symbols on directive

      enddo all_symbols                                              ! all symbols

!  if active line, process the declaration

   endif active_line                                                 ! if active line

! ----------------------------------------------------------------------

!  process_logical_declaration() exit

return                                                               ! process_logical_declaration()

! **********************************************************************

!  process_logical_declaration()

end subroutine process_logical_declaration

! **********************************************************************
! **********************************************************************

!  process_logical_constant() process logical declarations

subroutine process_logical_constant( logical_stmt)

! **********************************************************************

!  process_logical_constant() interface

! ----------------------------------------------------------------------

!  the statement containing the declaration

character( len= *), intent( in) :: logical_stmt

! **********************************************************************

!  entry: logical_stmt is blank_compress_lower_case logical declaration past the logical keyword
!         "::..." | ",parameter::..."

!  exit: logical declaration is processed or error exit

! **********************************************************************

!  process_logical_constant() local

! ----------------------------------------------------------------------

!  string containing a single symbol declaration symbol

   character( len= buffer_len) :: symbol_str                         ! process symbol by symbol

!  name of symbol

   character( len= symbol_name_len) :: symbol_name                          ! maximum length is 32 characters

!  results of decoding statement

   integer :: symbol_len                                             ! length of string decoded

! ----------------------------------------------------------------------

!  point to next character to be decoded

   integer :: next_char                                              ! next character to process

   integer :: decl_len                                               ! length of a definition

! **********************************************************************

!  process_logical_constant() text

continue                                                             ! process_logical_constant()

   next_char = 1

! ----------------------------------------------------------------------

!  if active line, process the declaration

   active_line: if( if_construct% now_selected )then                 ! if active line

!  extract all symbols on directive

      all_symbols: do                                                ! all symbols

!  one symbol at a time to the symbol string

         decl_len = scan( logical_stmt( next_char: ), end_of_decl) + next_char - 1

         symbol_str = logical_stmt( next_char: decl_len - 1)         ! up to the comma or blank

!  extract symbol name

         call get_symbol_name( symbol_str, symbol_name, symbol_len)  ! name of logical

!  store symbol in symbol list

         call add_logical( symbol_str( symbol_len + 1: decl_len - 1), symbol_name, .true.)

!  comma separates symbols, blank is end of statement

         all_done: if( logical_stmt( decl_len: decl_len) == blank )then

            exit all_symbols

         endif all_done                                              ! at end of statement

!  reset for next symbol

         next_char = decl_len + 1                                    ! one character after this one

!  extract all symbols on directive

      enddo all_symbols                                              ! all symbols

!  if active line, process the declaration

   endif active_line                                                 ! if active line

! ----------------------------------------------------------------------

!  process_logical_constant() exit

return                                                               ! process_logical_constant()

! **********************************************************************

!  process_logical_constant()

end subroutine process_logical_constant

! **********************************************************************
! **********************************************************************

!  add_logical() copy integer symbol to symbol table

subroutine add_logical( log_decl_str, symbol_name, is_const)

! **********************************************************************

!  add_logical() interface

! ----------------------------------------------------------------------

!  the statement containing the logical declaration

character( len= *), intent( in) :: log_decl_str

!  the symbol name

character( len= *), intent( in) :: symbol_name

!  true if the symbol is a constant

logical, intent( in) :: is_const                                    

! **********************************************************************

!  entry: log_decl_str is blank_compress_lower_case logical declaration statement past the double colon
!         "name" | "name=..."
!         sym_name is the symbol name
!         is_const is true if this is a constant declaration
!         processing_set_file is true if this was found in the setfile

!  exit: logical declaration is added to the logical symbol list or error exit

! **********************************************************************

!  add_logical() local

! ----------------------------------------------------------------------

!  pointer to pre-existing symbol

   type( symbol_t), pointer :: symbol_ptr                            ! from seek_symbol_name()

!  evaluate expression if there is one

   character( len= buffer_len) :: expr_str

! **********************************************************************

!  add_logical() text

continue                                                             ! add_logical()

! ----------------------------------------------------------------------

!  check if already on logical list

   call seek_symbol_name( symbol_name, symbol_ptr)                   ! find name on logical list

   duplicate_log: if( associated( symbol_ptr) )then                  ! if found

      name_type: select case( symbol_ptr% type_code)                 ! which type
   
      case( type_logical) name_type                                  ! logical
   
         set_defn: if( symbol_ptr% predefined )then                  ! defined in setfile

            call msg_continue( "logical predeclared in setfile: " // trim( symbol_name) )

            param_match: if( is_const .neqv. symbol_ptr% constant )then

               call msg_quit( "inconsistent definition of logical (constant v. variable): " &
                              // trim( symbol_name) )

            endif param_match

            symbol_ptr% predefined = .false.                         ! only declare once per program

            return                                                   ! don't add second definition

         else set_defn                                               ! defined in setfile

            call msg_quit( "duplicate logical declaration: " // trim( symbol_name) )

         endif set_defn                                              ! defined in setfile

      case( type_integer) name_type                                  ! name is already integer
   
         call msg_quit( "logical name already defined as integer: " // trim( symbol_name) )

      case( type_macro) name_type                                    ! name is already macro
   
         call msg_quit( "integer name already defined as macro: " // trim( symbol_name) )

      case( type_text) name_type                                     ! name is already text
   
         call msg_quit( "integer name already defined as text: " // trim( symbol_name) )

      end select name_type

   endif duplicate_log

! ----------------------------------------------------------------------

!  allocate next logical

   call add_symbol( symbol_name, symbol_ptr)                         ! put logical name into list

! ----------------------------------------------------------------------

!  store symbol code

   symbol_ptr% type_code = type_logical                              ! set code

!  store whether symbol is a parameter

   symbol_ptr% constant = is_const                                   ! is a constant

!  store whether symbol is declared in the setfile

   symbol_ptr% predefined = processing_set_file                      ! in setfile

!  determine if declaration specifies a value

   got_eq: if( len( log_decl_str) > 0 )then                          ! expr might be a null string

      symbol_ptr% defined = log_decl_str( : len( equals)) == equals

   else got_eq

      symbol_ptr% defined = .false.

   endif got_eq

! ----------------------------------------------------------------------

!  constants must have values

   constant_value: if( symbol_ptr% constant &                        ! if is a constant
            .and. .not. symbol_ptr% defined )then                    ! must have a value

         call msg_quit( "a logical constant must have a value: " &
                        // trim( symbol_name) // trim( log_decl_str) )

   endif constant_value                                              ! constants have values

!  decode the value

   got_value: if( symbol_ptr% defined )then                          ! decode value

      all_constants = .true.                                         ! preset flag

      expr_str = log_decl_str( len( equals) + 1: )

      call eval_log_expr( expr_str, symbol_ptr% logical_value)

!  evaluate the expression

      non_const: if( symbol_ptr% constant .and. .not. all_constants )then

         call msg_quit( "non constant expression used to define logical constant: " &
                        // trim( symbol_name) // trim( expr_str))

      endif non_const                                                ! expression

   endif got_value                                                   ! decode value

! ----------------------------------------------------------------------

!  add_logical() exit

return                                                               ! add_logical()

! **********************************************************************

!  add_logical()

end subroutine add_logical

! **********************************************************************
! **********************************************************************

!  %%% process coco constructs

! **********************************************************************
! **********************************************************************

!  process_macro_declaration() process macro declarations

subroutine process_macro_declaration( mac_stmt)

! **********************************************************************

!  process_macro_declaration() interface

! ----------------------------------------------------------------------

!  the statement containing the macro declaration

character( len= *), intent( in) :: mac_stmt                        

! **********************************************************************

!  entry: mac_stmt is blank_compress_lower_case logical declaration
!         past the macro keyword "::..."

!  exit: macro declaration is processed or error exit

! **********************************************************************

!  process_macro_declaration() local

! ----------------------------------------------------------------------

!  name of symbol

   character( len= symbol_name_len) :: symbol_name                          ! maximum length is 32 characters

!  results of decoding statement

   integer :: symbol_len                                             ! length of string decoded

! ----------------------------------------------------------------------

!  point to next character to be decoded

   integer :: next_char                                              ! next character to process

! **********************************************************************

!  process_macro_declaration() text

continue                                                             ! process_macro_declaration()

   next_char = 1

! ----------------------------------------------------------------------

!  if active line, process the declaration

   active_line: if( if_construct% now_selected )then                 ! process active lines

!  extract symbol name

      call get_macro_name( mac_stmt( next_char: ), symbol_name, symbol_len)

!  store symbol in symbol list

      next_char = next_char + symbol_len                             ! character after symbol name

      call add_macro( mac_stmt( next_char: ), symbol_name)           ! fill in macro_t

! ----------------------------------------------------------------------

!  if reporting extensions

      extensions: if( options% report_extensions )then               ! macros are extensions

         call msg_continue( "processed macro directive: " // trim( mac_stmt))

      endif extensions                                               ! macros are extensions

   endif active_line                                                 ! process active lines

! ----------------------------------------------------------------------

!  process_macro_declaration() exit

return                                                               ! process_macro_declaration()

! **********************************************************************

!  process_macro_declaration()

end subroutine process_macro_declaration

! **********************************************************************
! **********************************************************************

!  process_undefine_directive() process macro declarations

subroutine process_undefine_directive( undefine_stmt)

! **********************************************************************

!  process_undefine_directive() interface

! ----------------------------------------------------------------------

!  the statement containing the macro declaration

character( len= *), intent( in) :: undefine_stmt                        

! **********************************************************************

!  entry: undefine_stmt is blank_compress_lower_case logical declaration
!         past the undefine: keyword: "symbol[,symbol]..."

!  exit: macro declaration is processed or error exit

! **********************************************************************

!  process_undefine_directive() local

! ----------------------------------------------------------------------

!  name of symbol

   character( len= symbol_name_len) :: symbol_name                   ! maximum length is 32 characters

!  results of decoding statement

   integer :: symbol_len                                             ! length of string decoded

! ----------------------------------------------------------------------

!  pointers to characters

   integer :: next_char                                              ! walk through string

! **********************************************************************

!  process_undefine_directive() text

continue                                                             ! process_undefine_directive()

! ----------------------------------------------------------------------

!  syntax check

   not_null: if( undefine_stmt == blank )then

      call msg_quit( "no names on undefine statement")

   endif not_null

! ----------------------------------------------------------------------

!  if active line, process the declaration

   active_line: if( if_construct% now_selected )then                 ! process active lines

!  extract all symbols on directive

      next_char = 1                                                  ! start at beginning

      all_symbols: do                                                ! all symbols

!  one symbol at a time to the symbol string

         symbol_len = scan( undefine_stmt( next_char: ), end_of_decl) + next_char - 1

         symbol_name = undefine_stmt( next_char: symbol_len - 1)     ! up to the comma or blank

!  store symbol in symbol list

         call remove_symbol( symbol_name)

!  comma separates symbols, blank is end of statement

         all_done: if( undefine_stmt( symbol_len: symbol_len) == blank )then

            exit all_symbols

         endif all_done                                              ! at end of statement

!  reset for next symbol

         next_char = symbol_len + 1                                  ! one character after this one

!  extract all symbols on directive

      enddo all_symbols                                              ! all symbols

! ----------------------------------------------------------------------

!  if reporting extensions

      extensions: if( options% report_extensions )then               ! undefines are extensions

         call msg_continue( "processed undefine directive: " // trim( undefine_stmt))

      endif extensions                                               ! undefines are extensions

   endif active_line                                                 ! process active lines

! ----------------------------------------------------------------------

!  process_undefine_directive() exit

return                                                               ! process_undefine_directive()

! **********************************************************************

!  process_undefine_directive()

end subroutine process_undefine_directive

! **********************************************************************
! **********************************************************************

!  remove_symbol() seek symbol on symbol list

subroutine remove_symbol( symbol_str)

! **********************************************************************

!  remove_symbol() interface

! ----------------------------------------------------------------------

!  the name of the symbol being sought

character( len= *), intent( in) :: symbol_str

! **********************************************************************

!  entry: symbol_str is blank_compress_lower_case logical symbol directive
!         "name..."

!  exit: symbol found or not in logical symbol array

! **********************************************************************

!  remove_symbol() local

! ----------------------------------------------------------------------

!  a pointer to the symbol found

   type( symbol_t), pointer :: symbol_ptr

!  preserve the previous pointer

   type( symbol_t), pointer :: previous_ptr

! ----------------------------------------------------------------------

!  check deallocation status

   integer :: astat

! **********************************************************************

!  remove_symbol() text

continue                                                             ! remove_symbol()

! ----------------------------------------------------------------------

!  search symbol list

   nullify( symbol_ptr)                                              ! ensure pointer is ready for use

   nullify( previous_ptr)

   symbol_ptr => first_symbol                                        ! start of symbol list

!  loop through symbol list

   search_list: do while( associated( symbol_ptr) )                  ! find name

!  if found name matching

      name_match: if( symbol_str == symbol_ptr% name_str )then       ! if names match

         relink_list: if( associated( previous_ptr) )then            ! if pointer to previous

            previous_ptr% next => symbol_ptr% next                   ! use it

         else relink_list                                            ! if not pointer to previous

            first_symbol => symbol_ptr% next                         ! use first

         endif relink_list                                           ! if pointer to previous

!  deallocate any allocated components

         dealloc_args: if( associated( symbol_ptr% dummy_args) )then

            deallocate( symbol_ptr% dummy_args, stat= astat)

            dummy_args_error: if( astat > 0 )then                    ! deallocate error

               call msg_quit( "error deallocating dummy arguments: " // trim( symbol_ptr% dummy_args( 1)))

            endif dummy_args_error                                   ! deallocate error

         endif dealloc_args

         dealloc_text: if( associated( symbol_ptr% text_lines) )then

            deallocate( symbol_ptr% text_lines, stat= astat)

            text_lines_error: if( astat > 0 )then                    ! deallocate error

               call msg_quit( "error deallocating text block: " // trim( symbol_ptr% text_lines( 1)))

            endif text_lines_error                                   ! deallocate error

         endif dealloc_text

!  deallocate its entry

         deallocate( symbol_ptr, stat= astat)                        ! remove the symbol entry

         dealloc_error: if( astat > 0 )then                          ! deallocate error

            call msg_quit( "error deallocating symbol: " // trim( symbol_str))

         endif dealloc_error                                         ! deallocate error

         return                                                      ! done

      endif name_match                                               ! if names match

!  else update the pointers and try again

      update_ptr: if( associated( previous_ptr) )then                ! if pointer to previous

         previous_ptr => symbol_ptr                                  ! preserve previous next

         symbol_ptr => symbol_ptr% next                              ! check next entry on list

      else update_ptr                                                ! if not pointer to prvious

         previous_ptr => first_symbol                                ! preserve previous next

         symbol_ptr => first_symbol% next                            ! check next symbol on list

      endif update_ptr                                               ! if pointer to previous

   enddo search_list                                                 ! find name

!  searched entire list with no match for name   

   call msg_quit( "symbol not found: " // trim( symbol_str))

! ----------------------------------------------------------------------

!  remove_symbol() exit

return                                                               ! remove_symbol()

! **********************************************************************

!  remove_symbol()

end subroutine remove_symbol

! **********************************************************************
! **********************************************************************

!  add_macro() copy integer symbol to symbol table

subroutine add_macro( mac_decl_str, symbol_name)

! **********************************************************************

!  add_macro() interface

! ----------------------------------------------------------------------

!  the statement containing the macro definition

character( len= *), intent( in) :: mac_decl_str

!  the macro name

character( len= *), intent( in) :: symbol_name                    

! **********************************************************************

!  entry: mac_decl_str is blank_compress_lower_case logical declaration statement past the double colon
!         "" | "=..."
!         sym_name is the symbol name
!         is_const is true if this is a constant declaration
!         processing_set_file is true if this was found in the setfile

!  exit: logical declaration is added to the logical symbol list or error exit

! **********************************************************************

!  add_macro() local

! ----------------------------------------------------------------------

!  pointer to pre-existing symbol and to the new symbol

   type( symbol_t), pointer :: symbol_ptr                            ! from seek_symbol_name()

!  location of close parenthesis

   integer :: close_idx                                              ! close parenthesis

   integer :: char_idx                                               ! point to chars

!  buffer for macro argument list

   character( len= buffer_len) :: arg_list_str                       ! process arg list into array

! **********************************************************************

!  add_macro() text

continue                                                             ! add_macro()

! ----------------------------------------------------------------------

!  check if already on symbol list

   call seek_symbol_name( symbol_name, symbol_ptr)                   ! check symbol list

   duplicate_mac: if( associated( symbol_ptr) )then                  ! found same name

      name_type: select case( symbol_ptr% type_code)                 ! which type
   
      case( type_macro) name_type                                    ! macro
   
         call msg_quit( "duplicate macro declaration: " // trim( symbol_name) )

      case( type_integer) name_type                                  ! name is already integer
   
         call msg_quit( "macro name already defined as integer: " // trim( symbol_name) )

      case( type_logical) name_type                                  ! name is already logical
   
         call msg_quit( "macro name already defined as macro: " // trim( symbol_name) )

      case( type_text) name_type                                     ! name is already text
   
         call msg_quit( "macro name already defined as text: " // trim( symbol_name) )

      end select name_type

   endif duplicate_mac                                               ! found same name

! ----------------------------------------------------------------------

!  allocate next macro

   call add_symbol( symbol_name, symbol_ptr)                         ! store in list

! ----------------------------------------------------------------------

!  store symbol code

   symbol_ptr% type_code = type_macro                                ! macro code

!  determine if declaration specifies an arglist

   got_arglist: if( mac_decl_str( len( open_paren): len( open_paren)) == open_paren )then

! paren enclose arglist

      call seek_close_paren( mac_decl_str, len( open_paren), close_idx)

      no_close_paren: if( close_idx == 0 )then

         call msg_quit( "no closing parenthesis: " // trim( mac_decl_str) )

      endif no_close_paren

      arg_list_str = mac_decl_str( len( open_paren) + 1: close_idx - 1)

      call process_dummy_arglist( arg_list_str, symbol_ptr)

      call verify_dummy_args( symbol_ptr)

      char_idx = close_idx + len( equals) + 1                        ! value is past close paren or =

   else got_arglist                                                  ! has arglist or not

      char_idx = len( equals) + 1                                    ! value is past =

      nullify( symbol_ptr% dummy_args)

   endif got_arglist                                                 ! has arglist or not

!  validate macro definition

   call verify_macro_value( symbol_ptr, mac_decl_str( char_idx: ))

!  store the value

   symbol_ptr% macro_value = mac_decl_str( char_idx: )               ! value string

! ----------------------------------------------------------------------

!  add_macro() exit

return                                                               ! add_macro()

! **********************************************************************

!  add_macro()

end subroutine add_macro

! **********************************************************************
! **********************************************************************

!  process_dummy_arglist() process macro or text dummy argument list

subroutine process_dummy_arglist( arglist, symbol_ptr)

! **********************************************************************

!  process_dummy_arglist() interface

! ----------------------------------------------------------------------

!  the comma separated dummy argument list

character( len= *), intent( in) :: arglist

!  a pointer to the macro

type( symbol_t), pointer :: symbol_ptr

! **********************************************************************

!  entry: arglist is a character with the arglist

!  exit: symbol_ptr has its arglist array allocated or defined

! **********************************************************************

!  process_dummy_arglist() constants

   character( len= *), parameter :: end_of_arg = blank // comma      ! either may end an arg

! **********************************************************************

!  process_dummy_arglist() local

! ----------------------------------------------------------------------

   character( len= buffer_len) :: arg_str                            ! dummy args buffer

!  length of dummy argument list

   integer :: arg_len                                                ! len_trim( arg_buf)

! ----------------------------------------------------------------------

!  number of dummy arguments found

   integer :: number_of_args                                         ! count dummy args

!  point to next character to be decoded

   integer :: next_char                                              ! character by character

   integer :: this_arg                                               ! loop thru args

!  allocation status

   integer :: astat                                                  ! check status

! **********************************************************************

!  process_dummy_arglist() text

continue                                                             ! process_dummy_arglist()

! ----------------------------------------------------------------------

!  get length of arglist

   arg_str = arglist                                                 ! work with copy

   arg_len = len_trim( arglist)                                      ! length of arg list

!  if the arglist has characters

   null_list: if( arg_len > 0 )then                                  ! arglist is not null

!  count macro dummy arguments

      number_of_args = 1                                             ! start counting args

      count_args: do next_char = 1, arg_len                          ! loop thru arglist

         found_another: select case( arg_str( next_char: next_char)) ! check each char

         case( comma) found_another                                  ! commas separate args

            number_of_args = number_of_args + 1                      ! one more arg found

         end select found_another                                    ! check each char

      enddo count_args                                               ! loop thru arglist

!  allocate array to hold dummy args

      allocate( symbol_ptr% dummy_args( number_of_args), stat= astat)

      arg_error: if( astat > 0 )then                                 ! allocation failed

         call msg_quit( "allocate dummy argument list failed: " // trim( symbol_ptr% name_str))

      endif arg_error                                                ! allocation failed

!  put each dummy arg into its own array element

      get_args: do this_arg = 1, number_of_args                      ! process each arg

!  find end of each arg

         next_char = scan( arg_str, end_of_arg)                      ! find next blank or comma

!  store arg

         symbol_ptr% dummy_args( this_arg) = arg_str( : next_char - 1)

!  shift off that arg

         arg_str = arg_str( next_char + 1: )                         ! remove to past the comma or blank

      enddo get_args                                                 ! check each arg

   else null_list                                                    ! arglist is not null

      nullify( symbol_ptr% dummy_args)                               ! no dummy args

   endif null_list                                                   ! arglist is not null

! ----------------------------------------------------------------------

!  process_dummy_arglist() exit

return                                                               ! process_dummy_arglist()

! **********************************************************************

!  process_dummy_arglist()

end subroutine process_dummy_arglist

! **********************************************************************
! **********************************************************************

!  verify_macro_value() process macro dummy argument list

subroutine verify_macro_value( macro_ptr, value_str)

! **********************************************************************

!  verify_macro_value() interface

! ----------------------------------------------------------------------

!  a pointer to the macro

type( symbol_t), pointer :: macro_ptr

!  the macro value string

character( len=*), intent( in) :: value_str                       

! **********************************************************************

!  entry: macro or its coridate value

!  exit: macro's value is valid & may be assigned to macro

! **********************************************************************

!  verify_macro_value() local

! ----------------------------------------------------------------------

!  make ?name? string

   character( len= target_len) :: search_str                     !  find recursive macros

!  point to characters

   integer :: dup_idx                                                !  point to recursive names

!  point to dummy args

   integer :: this_arg                                               !  index arglist

! **********************************************************************

!  verify_macro_value() text

continue                                                             ! verify_macro_value()

! ----------------------------------------------------------------------

!  check for null valued macros

   null_macro: if( len_trim( value_str) == 0 )then

      call msg_quit( "macro has null value: " // trim( macro_ptr% name_str) )

   endif null_macro

! ----------------------------------------------------------------------

!  check for recursive macros

   search_str = arg_key // trim( macro_ptr% name_str) // arg_key

   dup_idx = index( value_str, trim( search_str))

   recursive_mac: if( dup_idx > 0 )then                              ! no recursion

      call msg_quit( "recursive macro definition: " // trim( macro_ptr% name_str) )

   endif recursive_mac                                               ! no recursion

!  check that dummy args all appear in the macro value

   have_dummy_args: if( associated( macro_ptr% dummy_args) )then

      scan_dummy_args: do this_arg = lbound( macro_ptr% dummy_args, dim= 1), ubound( macro_ptr% dummy_args, dim= 1)

         search_str = arg_key // trim( macro_ptr% dummy_args( this_arg)) // arg_key

         dup_idx = index( value_str, trim( search_str))

         arg_not_used: if( dup_idx == 0 )then                        ! arg not used

            call msg_quit( "macro argument unused in: " // trim( macro_ptr% name_str) &
                           // " argument: " // trim( macro_ptr% dummy_args( this_arg)) )

         endif arg_not_used                                          ! arg not used

      enddo scan_dummy_args

   endif have_dummy_args

! ----------------------------------------------------------------------

!  verify_macro_value() exit

return                                                               ! verify_macro_value()

! **********************************************************************

!  verify_macro_value()

end subroutine verify_macro_value

! **********************************************************************
! **********************************************************************

!  verify_dummy_args() process macro or text dummy argument list

subroutine verify_dummy_args( symbol_ptr)

! **********************************************************************

!  verify_dummy_args() interface

! ----------------------------------------------------------------------

!  a pointer to the macro

type( symbol_t), pointer :: symbol_ptr

! **********************************************************************

!  entry: macro or text dummy arg list

!  exit: macro's value is valid & may be assigned to macro

! **********************************************************************

!  verify_dummy_args() local

! ----------------------------------------------------------------------

!  point to characters

   integer :: this_arg                                               ! point to arg in arglist

   integer :: dup_idx                                                ! point to duplicate arg

   integer :: char_idx                                               ! index of illegal character

   integer :: arg_name_len                                           ! check only characters in arg name

! ----------------------------------------------------------------------

!  check for the same name amongst integers, logicals, macros, text blocks

   type( symbol_t), pointer :: name_ptr                              ! point to symbol types

! **********************************************************************

!  verify_dummy_args() text

continue                                                             ! verify_dummy_args()

! ----------------------------------------------------------------------

!  macro or text dummy arguments must be valid names

   check_names: do this_arg = lbound( symbol_ptr% dummy_args, dim= 1), ubound( symbol_ptr% dummy_args, dim= 1)

!  check whether initial character is alphabetic

      char_idx = verify( symbol_ptr% dummy_args( this_arg)( 1: 1), alpha_chars)

      initial_ok: if( char_idx /= 0 )then                            ! if initial not alphabetic

         call msg_quit( "illegal initial character in macro arg name: " // &
                        trim( symbol_ptr% dummy_args( this_arg)) )

      endif initial_ok                                               ! if initial not alphabetic

!  check whether following characters in name are legal

      arg_name_len = len_trim( symbol_ptr% dummy_args( this_arg))    ! check rest of name

      char_idx = verify( symbol_ptr% dummy_args( this_arg)( 2: arg_name_len), alphanum_chars)

      name_ok: if( char_idx /= 0 )then                               ! if character not legal

         call msg_quit( "illegal character in macro arg name: " // &
                        trim( symbol_ptr% dummy_args( this_arg)) )

      endif name_ok                                                  ! if character not legal

!  check whether names of args duplicates arg names

      self_dup: do dup_idx = 1, this_arg - 1                         ! check each dummy arg

         dup_arg: if( symbol_ptr% dummy_args( this_arg) == symbol_ptr% dummy_args( dup_idx) )then

            call msg_quit( "duplicate dummy argument name: " // &
                           symbol_ptr% dummy_args( this_arg) )

         endif dup_arg                                               ! duplicate name

      enddo self_dup

!  check whether arg name duplicates symbol names

      call seek_symbol_name( symbol_ptr% dummy_args( this_arg), name_ptr)

      dup_sym: if( associated( name_ptr) )then                       ! duplicate name

         call msg_quit( "dummy argument name duplicates symbol name: " // &
                        symbol_ptr% dummy_args( this_arg) )

      endif dup_sym                                                  ! duplicate name

   enddo check_names                                                 ! check each dummy arg

! ----------------------------------------------------------------------

!  verify_dummy_args() exit

return                                                               ! verify_dummy_args()

! **********************************************************************

!  verify_dummy_args()

end subroutine verify_dummy_args

! **********************************************************************
! **********************************************************************

!  %%% process if constructs: if, elseif, else, endif, ifndef

! **********************************************************************
! **********************************************************************

!  process_if_directive() process a coco if( )then directive

subroutine process_if_directive( if_dir)

! **********************************************************************

!  process_if_directive() interface

! ----------------------------------------------------------------------

!  a statement containing an if directive

character( len= *), intent( in) :: if_dir                          

! **********************************************************************

!  entry: if_dir is blank_compress_lower_case if directive, past the coco key and the "if("
!         "<logical>)then"

!  exit: the directive is processed or error exit

! **********************************************************************

!  process_if_directive() local

! ----------------------------------------------------------------------

!  pointer to ')then'

   integer :: then_idx                                               ! find then

!  value of logical expression

   logical :: expression_value                                       ! between if( and )then

!  status of allocating a new if block

   integer :: astat                                                  !  allocation status

!  copy expression string for evaluation

   character( len= buffer_len) :: expr_str                           ! passed to eval_log_expr()

! **********************************************************************

!  process_if_directive() text

continue                                                             ! process_if_directive()

! ----------------------------------------------------------------------

!  check for a well formed if directive

   then_idx = index( if_dir, then_str)                               ! where is then

   syntax_check: if( then_idx == 0 )then                             ! syntax error

      call msg_quit( "no 'then' in if directive: " // trim( if_dir) )

   endif syntax_check                                                ! syntax error

   extra_chars_check: if( if_dir( then_idx + len( then_str): ) /= blank )then

      call msg_quit( "extra characters after if directive: " // trim( if_dir) )

   endif extra_chars_check                                           ! syntax error

! ----------------------------------------------------------------------

!  append new if construct at end of list

   allocate( if_construct% nested, stat= astat)                      ! allocate new if block

   alloc_error: if( astat > 0 )then                                  ! process error

      call msg_quit( "allocate if block failed: " // trim( if_dir) )

   endif alloc_error                                                 ! process error

!  establish pointers

   if_construct% nested% enclosing => if_construct                   ! new backward pointer

!  make new if construct the active if construct

   if_construct => if_construct% nested                              ! now in nested if construct

   nullify( if_construct% nested)                                    ! ready for use

!  this phase is an if block

   if_construct% phase = if_block                                    ! the next phase can be elseif, else, endif

! ----------------------------------------------------------------------

!  if this if block is enclosed within selected lines

   active_lines: if( if_construct% enclosing% now_selected )then     ! do no more than check syntax if not selected

! ----------------------------------------------------------------------

!  evaluate logical expression only when enclosing if block is selected

      expr_str = if_dir( : then_idx - 1)                             ! copy expression string

      call eval_log_expr( expr_str, expression_value)                ! evaluate expression

!  set if value accordingly

      if_construct% now_selected = expression_value                  ! this block is selected

      if_construct% ever_selected = expression_value                 ! this block is selected

! ----------------------------------------------------------------------

!  the enclosing if block is not selected

   else active_lines                                                 ! do no more than check syntax if not selected

      if_construct% now_selected = .false.                           ! this block is not selected

      if_construct% ever_selected = .true.                           ! no other block is selected

   endif active_lines                                                ! do no more than check syntax if not selected

! ----------------------------------------------------------------------

!  process_if_directive() exit

return                                                               ! process_if_directive()

! **********************************************************************

!  process_if_directive()

end subroutine process_if_directive

! **********************************************************************
! **********************************************************************

!  process_elseif_directive() process a coco elseif( )then directive

subroutine process_elseif_directive( elseif_dir)

! **********************************************************************

!  process_elseif_directive() interface

! ----------------------------------------------------------------------

!  a statement containing an elseif directive

character( len= *), intent( in) :: elseif_dir                      

! **********************************************************************

!  entry: elseif_dir is blank_compress_lower_case elseif directive, past the coco key

!  exit: the directive is processed or error exit

! **********************************************************************

!  process_elseif_directive() local

! ----------------------------------------------------------------------

!  location of closing )then

   integer :: then_idx                                               ! after logical expression

!  value of logical expression

   logical :: expression_value                                       ! between if( and )then

   character( len= buffer_len) :: expr_str

! **********************************************************************

!  process_elseif_directive() text

continue                                                             ! process_elseif_directive()

! ----------------------------------------------------------------------

!  if not in if-block, elseif is misplaced

   if_sequence: select case( if_construct% phase)                    ! elseif in if sequence

   case( outside_block ) if_sequence                                 ! before if or after endif

      call msg_quit( "elseif outside if construct: " // trim( elseif_dir) )

   case( else_block ) if_sequence                                    ! after else and before endif

      call msg_quit( "elseif after else: " // trim( elseif_dir) )

   end select if_sequence                                            ! elseif in if sequence

! ----------------------------------------------------------------------

!  logical expression must be between 'if(' and ')then'

   then_idx = index( elseif_dir, then_str)                           ! find then

   syntax_check: if( then_idx == 0 )then                             ! syntax error

      call msg_quit( "no 'then' in elseif directive: " // trim( elseif_dir) )

   endif syntax_check                                                ! syntax error

   extra_chars_check: if( elseif_dir( then_idx + len( then_str): ) /= blank )then

      call msg_quit( "extra characters after elseif directive: " // trim( elseif_dir) )

   endif extra_chars_check                                           ! syntax error

!  this phase is an elseif block

   if_construct% phase = elseif_block                                ! the next phase can be elseif, else, endif

! ----------------------------------------------------------------------

!  if this if block is enclosed within selected lines

   active_lines: if( if_construct% enclosing% now_selected )then

      previous_true: if( if_construct% ever_selected )then

         if_construct% now_selected = .false.

      else previous_true

!  evaluate logical expression

         expr_str = elseif_dir( : then_idx - 1)                      ! copy expression string

         call eval_log_expr( expr_str, expression_value)             ! evaluate expression

!  set if value accordingly

         if_construct% now_selected = expression_value

         if_construct% ever_selected = expression_value

      endif previous_true

   endif active_lines                                                ! do no more than check syntax if not selected

! ----------------------------------------------------------------------

!  process_elseif_directive() exit

return                                                               ! process_elseif_directive()

! **********************************************************************

!  process_elseif_directive()

end subroutine process_elseif_directive

! **********************************************************************
! **********************************************************************

!  process_else_directive() process a coco else directive

subroutine process_else_directive( else_dir)

! **********************************************************************

!  process_else_directive() interface

! ----------------------------------------------------------------------

!  a statement containing an else directive

character( len= *), intent( in) :: else_dir                         

! **********************************************************************

!  entry: else_dir is blank_compress_lower_case else directive, past the coco key

!  exit: the directive is processed or error exit

! **********************************************************************

!  process_else_directive() text

continue                                                             ! process_else_directive()

! ----------------------------------------------------------------------

!  if not in if-block, else is misplaced

   if_sequence: select case( if_construct% phase)                    ! else sequence

   case( outside_block ) if_sequence                                 ! before if or after endif

      call msg_quit( "else outside if construct: " // trim( else_dir) )

   case( else_block ) if_sequence                                    ! only one else block

      call msg_quit( "else after else: " // trim( else_dir) )

   end select if_sequence                                            ! else sequence

!  must have nothing after 'else'

   syntax_error: if( else_dir /= blank )then                         ! nothing after else

      call msg_quit( "extra characters after else directive: " // trim( else_dir) )

   endif syntax_error                                                ! nothing after else

! ----------------------------------------------------------------------

!  this phase is an else block

   if_construct% phase = else_block                                  ! the next phase must be endif

! ----------------------------------------------------------------------

!  select else block if this if ithe enclosing block is active and no previous block has been selected

   if_construct% now_selected = if_construct% enclosing% now_selected &
                                .and. .not. if_construct% ever_selected

! ----------------------------------------------------------------------

!  process_else_directive() exit

return                                                               ! process_else_directive()

! **********************************************************************

!  process_else_directive()

end subroutine process_else_directive

! **********************************************************************
! **********************************************************************

!  process_endif_directive() process a coco endif directive

subroutine process_endif_directive( endif_dir)

! **********************************************************************

!  process_endif_directive() interface

! ----------------------------------------------------------------------

!  a statement containing an endif directive

character( len= *), intent( in) :: endif_dir                     

! **********************************************************************

!  entry: endif_dir is blank_compress_lower_case endif directive, past the coco key

!  exit: the directive is processed or error exit

! **********************************************************************

!  process_endif_directive() local

! ----------------------------------------------------------------------

!  deallocate status

   integer :: astat                                                  ! check status

! **********************************************************************

!  process_endif_directive() text

continue                                                             ! process_endif_directive()

! ----------------------------------------------------------------------

!  if not in if-block, endif is misplaced

   if_sequence: select case( if_construct% phase)                    ! if sequence

   case( outside_block ) if_sequence                                 ! not in if block

      call msg_quit( "endif outside any if construct: " // trim( endif_dir) )

   end select if_sequence                                            ! if sequence

!  must have nothing after 'endif'

   syntax_error: if( endif_dir /= blank )then                        ! syntax error

      call msg_quit( "extra characters after endif directive: " // trim( endif_dir) )

   endif syntax_error                                                ! syntax error

! ----------------------------------------------------------------------

!  decrement if level

   if_construct => if_construct% enclosing                           ! jump back to enclosing if block

   deallocate( if_construct% nested, stat= astat)                    ! deallocate if block

   next_error: if( astat > 0 )then                                   ! deallocate error

      call msg_quit( "deallocate if construct failed: " // trim( endif_dir) )

   endif next_error                                                  ! deallocate error

   nullify( if_construct% nested)                                    ! reset pointer

! ----------------------------------------------------------------------

!  process_endif_directive() exit

return                                                               ! process_endif_directive()

! **********************************************************************

!  process_endif_directive()

end subroutine process_endif_directive

! **********************************************************************
! **********************************************************************

!  process_ifdef_directive() process a coco ifdef( )then directive

subroutine process_ifdef_directive( ifdef_dir)

! **********************************************************************

!  process_ifdef_directive() interface

! ----------------------------------------------------------------------

!  a statement containing an if directive

character( len= *), intent( in) :: ifdef_dir

! **********************************************************************

!  entry: ifdef_dir is blank_compress_lower_case ifdef directive,
!         past the coco key and the "ifdef("
!         "<logical>)then"

!  exit: the directive is processed or error exit

! **********************************************************************

!  process_ifdef_directive() local

! ----------------------------------------------------------------------

!  pointer to ')then'

   integer :: then_idx                                               ! find then

!  status of allocating a new if block

   integer :: astat                                                  !  allocation status

!  copy symbol name string to see if it exists

   character( len= symbol_name_len) :: name_str                      ! passed to seek_symbol_name()

!  symbol pointer to return symbol or null

   type( symbol_t), pointer :: symbol_ptr

! **********************************************************************

!  process_ifdef_directive() text

continue                                                             ! process_ifdef_directive()

! ----------------------------------------------------------------------

!  check for a well formed if directive

   then_idx = index( ifdef_dir, then_str)                           ! where is then

   syntax_check: if( then_idx == 0 )then                             ! syntax error

      call msg_quit( "no 'then' in ifdef directive: " // trim( ifdef_dir) )

   endif syntax_check                                                ! syntax error

   extra_chars_check: if( ifdef_dir( then_idx + len( then_str): ) /= blank )then

      call msg_quit( "extra characters after ifdef directive: " // trim( ifdef_dir) )

   endif extra_chars_check                                           ! syntax error

! ----------------------------------------------------------------------

!  append new if construct at end of list

   allocate( if_construct% nested, stat= astat)                      ! allocate new if block

   alloc_error: if( astat > 0 )then                                  ! process error

      call msg_quit( "allocate ifdef block failed: " // trim( ifdef_dir) )

   endif alloc_error                                                 ! process error

!  establish pointers

   if_construct% nested% enclosing => if_construct                   ! new backward pointer

!  make new if construct the active if construct

   if_construct => if_construct% nested                              ! now in nested if construct

   nullify( if_construct% nested)                                    ! ready for use

!  this phase is an if block

   if_construct% phase = else_block                                  ! the next phase must be endif

! ----------------------------------------------------------------------

!  if this if block is enclosed within selected lines

   active_lines: if( if_construct% enclosing% now_selected )then     ! do no more than check syntax if not selected

! ----------------------------------------------------------------------

!  check existance only when enclosing if block is selected

      name_str = ifdef_dir( : then_idx - 1)                          ! copy expression string

      call seek_symbol_name( name_str, symbol_ptr)                   ! attempt to find name

!  set ifdef value accordingly

      if_construct% now_selected = associated( symbol_ptr)           ! this block is selected

      if_construct% ever_selected = associated( symbol_ptr)          ! this block is selected

! ----------------------------------------------------------------------

!  the enclosing if block is not selected

   else active_lines                                                 ! do no more than check syntax if not selected

      if_construct% now_selected = .false.                           ! this block is not selected

      if_construct% ever_selected = .true.                           ! no other block is selected

   endif active_lines                                                ! do no more than check syntax if not selected

! ----------------------------------------------------------------------

!  process_ifdef_directive() exit

return                                                               ! process_ifdef_directive()

! **********************************************************************

!  process_ifdef_directive()

end subroutine process_ifdef_directive

! **********************************************************************
! **********************************************************************

!  process_ifndef_directive() process a coco ifndef( )then directive

subroutine process_ifndef_directive( ifndef_dir)

! **********************************************************************

!  process_ifndef_directive() interface

! ----------------------------------------------------------------------

!  a statement containing an if directive

character( len= *), intent( in) :: ifndef_dir

! **********************************************************************

!  entry: ifndef_dir is blank_compress_lower_case ifndef directive,
!         past the coco key and the "ifndef("
!         "<logical>)then"

!  exit: the directive is processed or error exit

! **********************************************************************

!  process_ifndef_directive() local

! ----------------------------------------------------------------------

!  pointer to ')then'

   integer :: then_idx                                               ! find then

!  status of allocating a new if block

   integer :: astat                                                  !  allocation status

!  copy symbol name string to see if it exists

   character( len= symbol_name_len) :: name_str                      ! passed to seek_symbol_name()

!  symbol pointer to return symbol or null

   type( symbol_t), pointer :: symbol_ptr

! **********************************************************************

!  process_ifndef_directive() text

continue                                                             ! process_ifndef_directive()

! ----------------------------------------------------------------------

!  check for a well formed if directive

   then_idx = index( ifndef_dir, then_str)                           ! where is then

   syntax_check: if( then_idx == 0 )then                             ! syntax error

      call msg_quit( "no 'then' in ifndef directive: " // trim( ifndef_dir) )

   endif syntax_check                                                ! syntax error

   extra_chars_check: if( ifndef_dir( then_idx + len( then_str): ) /= blank )then

      call msg_quit( "extra characters after ifndef directive: " // trim( ifndef_dir) )

   endif extra_chars_check                                           ! syntax error

! ----------------------------------------------------------------------

!  append new if construct at end of list

   allocate( if_construct% nested, stat= astat)                      ! allocate new if block

   alloc_error: if( astat > 0 )then                                  ! process error

      call msg_quit( "allocate ifndef block failed: " // trim( ifndef_dir) )

   endif alloc_error                                                 ! process error

!  establish pointers

   if_construct% nested% enclosing => if_construct                   ! new backward pointer

!  make new if construct the active if construct

   if_construct => if_construct% nested                              ! now in nested if construct

   nullify( if_construct% nested)                                    ! ready for use

!  this phase is an if block

   if_construct% phase = else_block                                  ! the next phase must be endif

! ----------------------------------------------------------------------

!  if this if block is enclosed within selected lines

   active_lines: if( if_construct% enclosing% now_selected )then     ! do no more than check syntax if not selected

! ----------------------------------------------------------------------

!  check existance only when enclosing if block is selected

      name_str = ifndef_dir( : then_idx - 1)                         ! copy expression string

      call seek_symbol_name( name_str, symbol_ptr)                   ! attempt to find name

!  set ifndef value accordingly

      if_construct% now_selected = .not. associated( symbol_ptr)     ! this block is selected

      if_construct% ever_selected = .not. associated( symbol_ptr)    ! this block is selected

! ----------------------------------------------------------------------

!  the enclosing if block is not selected

   else active_lines                                                 ! do no more than check syntax if not selected

      if_construct% now_selected = .false.                           ! this block is not selected

      if_construct% ever_selected = .true.                           ! no other block is selected

   endif active_lines                                                ! do no more than check syntax if not selected

! ----------------------------------------------------------------------

!  process_ifndef_directive() exit

return                                                               ! process_ifndef_directive()

! **********************************************************************

!  process_ifndef_directive()

end subroutine process_ifndef_directive

! **********************************************************************
! **********************************************************************

!  process_assert_directive() process an assert directive

subroutine process_assert_directive( assert_dir)

! **********************************************************************

!  process_assert_directive() interface

! ----------------------------------------------------------------------

!  a statement containing an assert directive

character( len= *), intent( in) :: assert_dir

! **********************************************************************

!  entry: assert_dir is quoted assert condition

!  exit: assertion code is written to the output

! **********************************************************************

!  process_assert_directive() constants

! ----------------------------------------------------------------------

!  pieces of the assert output

   character( len= *), parameter :: if_prt = 'if( .not. ( '          ! the if part

   character( len= *), parameter :: then_prt = ' ) )then'            ! the then part

   character( len= *), parameter :: write_prt = 'write( unit= error_unit, fmt= *) "assertion failed: '

   character( len= *), parameter :: stop_prt = 'stop "assertion failed"'

   character( len= *), parameter :: endif_prt = 'endif'              ! the endif part

! ----------------------------------------------------------------------

!  starting column of output

   integer, parameter :: start_col = 7                               ! allow fixed format and free format

! **********************************************************************

!  process_assert_directive() local

! ----------------------------------------------------------------------

!  the condition must be unquoted before use

   character( len= buffer_len) :: assert_cond                        ! the unquoted condition

!  assemble the output line

   character( len= buffer_len) :: edit_line                          ! assemble output lines

! ----------------------------------------------------------------------

!  convert the current input line number to characters

   character( len= conversion_len) :: conversion_str                 ! write character line number

!  lengths of quoted and unquoted include file name

   integer :: construct_len                                          ! length of quoted condition

   integer :: cond_len                                               ! length of unquoted condition

! ----------------------------------------------------------------------

!  inhibit wrapping when editing, assert processing doesn its own wrapping

   integer :: wrap_state                                             ! process wrapping after editing

! **********************************************************************

!  process_assert_directive() text

continue                                                             ! process_assert_directive()

! ----------------------------------------------------------------------

!  check syntax- condition must be string within quotes

   call unquote_string( assert_dir, assert_cond, construct_len, cond_len )

   no_cond_str: if( cond_len == 0 .or. construct_len == 0 )then      ! no assert condition

      call msg_quit( "can't find assert condition: " // trim( assert_dir))

   endif no_cond_str                                                 ! no assert condition

!  check syntax- directive must be blank after condition

   extra_chars: if( assert_dir( construct_len + 1: ) /= blank )then

      call msg_quit( "extra characters after assert condition: " // trim( assert_dir))

   endif extra_chars

! ----------------------------------------------------------------------

!  if active block, process assert directive

   active_line: if( if_construct% now_selected )then                 ! if active assert line

!  editing source lines is enabled

      edit_source: if( options% edit_source )then                    ! edit source lines

!  if ? is present edit source line

         edit_cond: if( index( assert_cond, arg_key) > 0 )then

!  disable wrapping because each line will be wrapped after it is constructed

            wrap_state = options% wrap_length                        ! save wrap state

            options% wrap_length = wrap_off                          ! inhibit wrapping

            call edit_source_line( assert_cond)                      ! edit integers, macros

            options% wrap_length = wrap_state                        ! reset wrapping

         endif edit_cond                                             ! if ? is present

      endif edit_source                                              ! edit source lines

! ----------------------------------------------------------------------

!  write the if statement

      edit_line( : start_col - 1) = blank                            ! blank out first few cols

      edit_line( start_col: ) = if_prt // trim( assert_cond) // then_prt

!  remove any line length overflow

      wrap_if: if( options% wrap_length /= wrap_off )then            ! if wrapping source

         call wrap_source_line( edit_line)                           ! wrap source lines if len > 132

      endif wrap_if                                                  ! if wrapping source

! ----------------------------------------------------------------------

!  write assembled if-then statement

      line = edit_line

      call write_source_line( output_file)                           ! write the if statement

! ----------------------------------------------------------------------

!  write the write statement

      edit_line( : start_col - 1) = blank                            ! blank out first few cols

!  get the current line number

      write( unit= conversion_str, fmt= conversion_fmt) current_file% lines_transfered

!  construct the assertion complaint

      edit_line( start_col: ) = write_prt // trim( current_file% name_str) &
                                          // ": " // trim( adjustl( conversion_str)) &
                                          // ': " // ' // trim( assert_dir)

!  remove any line length overflow

      wrap_write: if( options% wrap_length /= wrap_off )then         ! if wrapping source

         call wrap_source_line( edit_line)                           ! wrap source lines if len > 132

      endif wrap_write                                               ! if wrapping source

! ----------------------------------------------------------------------

!  write assembled write statement

      line = edit_line

      call write_source_line( output_file)                           ! to output

! ----------------------------------------------------------------------

!  blank until the start column for the stop and endif

      line( : start_col - 1) = blank                                 ! blank out first few cols

!  write the stop statement

      line( start_col: ) = stop_prt                                  ! stop string

      call write_source_line( output_file)                           ! to output

! ----------------------------------------------------------------------

!  write the endif statement

      line( start_col: ) = endif_prt                                 ! endif string

      call write_source_line( output_file)                           ! to output

! ----------------------------------------------------------------------

!  report extension

      extensions: if( options% report_extensions )then               ! assertions are extensions

         call msg_continue( "processed assert directive")            ! remark assert

      endif extensions                                               ! assertions are extensions

! ----------------------------------------------------------------------

   endif active_line                                                 ! if active assert line

! ----------------------------------------------------------------------

!  process_assert_directive() exit

return                                                               ! process_assert_directive()

! **********************************************************************

!  process_assert_directive()

end subroutine process_assert_directive

! **********************************************************************
! **********************************************************************

!  process_assertif_directive() process an assertif directive

subroutine process_assertif_directive( assertif_dir)

! **********************************************************************

!  process_assertif_directive() interface

! ----------------------------------------------------------------------

!  a statement containing an assertif directive

character( len= *), intent( in) :: assertif_dir

! **********************************************************************

!  entry: assertif_dir "<logical expression>)'assert condition'"

!  exit: if logical expression, assertion code is written to the output

! **********************************************************************

!  process_assertif_directive() local

! ----------------------------------------------------------------------

!  length of logical condition

   integer :: logical_len                                            ! length of logical expression

!  value of logical condition

   logical :: logical_value                                          ! value of logical expression

! **********************************************************************

!  process_assertif_directive() text

continue                                                             ! process_assertif_directive()

! ----------------------------------------------------------------------

!  check syntax- condition must be string within quotes

   call seek_close_paren( assertif_dir, 1, logical_len )

!  verify found logical expression

   no_logical_exp: if( logical_len == 0 )then                        ! no logical expression

      call msg_quit( "can't find assertif expression: " // trim( assertif_dir))

   endif no_logical_exp                                              ! no logical expression

! ----------------------------------------------------------------------

!  if active block, process assertif directive

   active_line: if( if_construct% now_selected )then                 ! if active assertif line

!  evaluate logical expression

      call eval_log_expr( assertif_dir( 2: logical_len - 1) // blank, logical_value)

!  if true, process assert

      expr_true: if( logical_value )then                             ! value on assertif directive

         call process_assert_directive( assertif_dir( logical_len + 1: ))

      endif expr_true                                                ! value on assertif directive

! ----------------------------------------------------------------------

!  report extension

      extensions: if( options% report_extensions )then               ! assertifs are extensions

         call msg_continue( "processed assertif directive")          ! remark assertif

      endif extensions                                               ! assertifs are extensions

! ----------------------------------------------------------------------

   endif active_line                                                 ! if active assertif line

! ----------------------------------------------------------------------

!  process_assertif_directive() exit

return                                                               ! process_assertif_directive()

! **********************************************************************

!  process_assertif_directive()

end subroutine process_assertif_directive

! **********************************************************************
! **********************************************************************

!  process_dump_directive() process an dump directive

subroutine process_dump_directive

! **********************************************************************

!  entry: dump_dir is quoted dump condition

!  exit: dumpion code is written to the output

! **********************************************************************

!  process_dump_directive() constants

! ----------------------------------------------------------------------

!  dump banner

   character( len= *), parameter :: dump_header = 'dump coco symbols: file: '

!  mark integers and logicals as constant or variable

   character( len= *), parameter :: variable_str = 'variable'

   character( len= *), parameter :: constant_str = 'constant'

! ----------------------------------------------------------------------

!  column widths

   integer, parameter :: vc_len = max( len( variable_str), len( constant_str) ) + 1

   integer, parameter :: pad_len = 2

! ----------------------------------------------------------------------

!  names of types

   integer, parameter :: type_len = 10

   character( len= type_len), dimension( 5), parameter :: type_str = (/ &
                                                            'integer   ', &
                                                            'logical   ', &
                                                            'macro     ', &
                                                            'text      ', &
                                                            '<unknown> ' /)

   integer, parameter :: unknown_idx = 5

   character( len= *), parameter :: undefined_str = '<undefined>'

! **********************************************************************

!  process_dump_directive() local

! ----------------------------------------------------------------------

!  convert line number to string

   character( len= conversion_len) :: line_number_str                ! report line number

!  point to symbols

   type( symbol_t), pointer :: symbol_ptr                            ! scan symbol list

!  point to next character to be added to report line

   integer :: next_char                                              ! format report line

   integer :: arg_len

   integer :: this_arg

! **********************************************************************

!  process_dump_directive() text

continue                                                             ! process_dump_directive()

! ----------------------------------------------------------------------

!  if active block, process dump directive

   active_line: if( if_construct% now_selected )then                 ! if active dump line

      write( line_number_str, fmt= conversion_fmt) current_file% lines_transfered

      log_line = dump_header // trim( current_file% name_str) &
                             // ", line: " // trim( adjustl( line_number_str))

      call write_source_line( log_file)

! ----------------------------------------------------------------------

!  write a line for each symbol on the list

      symbol_ptr => first_symbol

      all_symbols: do while( associated( symbol_ptr) )

         log_line = blank                                            ! initialize line

!  line varies by type fo symbol

         which_type: select case( symbol_ptr% type_code)

! ----------------------------------------------------------------------

!  type integer

         case( type_integer) which_type

!  type and name

            log_line( : type_len) = type_str( symbol_ptr% type_code)

            log_line( type_len: type_len + symbol_name_len) = symbol_ptr% name_str

!  constant or variable

            next_char = type_len + symbol_name_len

            integer_v_or_c: if( symbol_ptr% constant )then

               log_line( next_char: next_char + vc_len) = constant_str

            else integer_v_or_c

               log_line( next_char: next_char + vc_len) = variable_str

            endif integer_v_or_c

!  defined or undefined

            next_char = next_char + vc_len + pad_len

!  if defined write the value

            integer_defined: if( symbol_ptr% defined )then

               write( unit= log_line( next_char: next_char + conversion_len), fmt= conversion_fmt) &
                      symbol_ptr% integer_value

               log_line( next_char: next_char + conversion_len + pad_len) = &
                         adjustr( log_line( next_char: next_char + conversion_len + pad_len) )

            else integer_defined

               log_line( next_char + pad_len: ) = undefined_str

            endif integer_defined

! ----------------------------------------------------------------------

!  type logical

         case( type_logical) which_type

!  type and name

            log_line( : type_len) = type_str( symbol_ptr% type_code)

            log_line( type_len: type_len + symbol_name_len) = symbol_ptr% name_str

!  constant or variable

            next_char = type_len + symbol_name_len

            logical_v_or_c: if( symbol_ptr% constant )then

               log_line( next_char: next_char + vc_len) = constant_str

            else logical_v_or_c

               log_line( next_char: next_char + vc_len) = variable_str

            endif logical_v_or_c

!  defined or undefined

            next_char = next_char + vc_len + pad_len

            logical_defined: if( symbol_ptr% defined )then

!  if defined write the value

               logical_t_f: if( symbol_ptr% logical_value )then

                  log_line( next_char: next_char + conversion_len) = true_str

               else logical_t_f

                  log_line( next_char: next_char + conversion_len) = false_str

               endif logical_t_f

               log_line( next_char: next_char + conversion_len + pad_len) = &
                         adjustr( log_line( next_char: next_char + conversion_len + pad_len) )

            else logical_defined

               log_line( next_char + pad_len: ) = undefined_str

            endif logical_defined

! ----------------------------------------------------------------------

!  type macro

         case( type_macro) which_type

!  type and name

            log_line( : type_len) = type_str( symbol_ptr% type_code)

            log_line( type_len: type_len + symbol_name_len) = symbol_ptr% name_str

!  has arg list or not

            next_char = type_len + symbol_name_len

            log_line( next_char: next_char) = open_paren

            next_char = next_char + len( open_paren)

            macro_args: if( associated( symbol_ptr% dummy_args) )then

               arg_len = len_trim( symbol_ptr% dummy_args( 1) )

               log_line( next_char: next_char + arg_len) = blank // symbol_ptr% dummy_args( 1)

               next_char = next_char + arg_len + len( blank)

               next_macro_arg: do this_arg = 2, size( symbol_ptr% dummy_args)

                  arg_len = len_trim( symbol_ptr% dummy_args( this_arg) )

                  log_line( next_char: next_char + arg_len + 1) = &
                          comma // blank // symbol_ptr% dummy_args( this_arg)

                  next_char = next_char + arg_len + len( comma // blank)

               enddo next_macro_arg

            endif macro_args

            log_line( next_char: next_char) = close_paren

!  value

            next_char = max( next_char + pad_len, type_len + symbol_name_len + vc_len + pad_len)

            log_line( next_char: ) = symbol_ptr% macro_value

! ----------------------------------------------------------------------

! type text

         case( type_text) which_type

!  type and name

            log_line( : type_len) = type_str( symbol_ptr% type_code)

            log_line( type_len: type_len + symbol_name_len) = symbol_ptr% name_str

!  has arg list or not

            next_char = type_len + symbol_name_len

            log_line( next_char: next_char) = open_paren

            next_char = next_char + len( open_paren)

            text_args: if( associated( symbol_ptr% dummy_args) )then

               arg_len = len_trim( symbol_ptr% dummy_args( 1) )

               log_line( next_char: next_char + arg_len) = blank // symbol_ptr% dummy_args( 1)

               next_char = next_char + arg_len + len( blank)

               next_text_arg: do this_arg = 2, size( symbol_ptr% dummy_args)

                  arg_len = len_trim( symbol_ptr% dummy_args( this_arg) )

                  log_line( next_char: next_char + arg_len + 1) = comma // blank // symbol_ptr% dummy_args( this_arg)

                  next_char = next_char + arg_len + len( comma // blank)

               enddo next_text_arg

            endif text_args

            log_line( next_char: next_char) = close_paren

!  first line of text

            next_char = max( next_char + pad_len, type_len + symbol_name_len + vc_len + pad_len)

            got_text_lines: if( associated( symbol_ptr% text_lines) )then

               log_line( next_char: ) = symbol_ptr% text_lines( 1)

            endif got_text_lines

! ----------------------------------------------------------------------

!  unknown type

         case default which_type

!  type and name

            log_line( : type_len) = type_str( unknown_idx)

            log_line( type_len: type_len + symbol_name_len) = symbol_ptr% name_str

         end select which_type

!  next_char = next_char + symbol_name_len

         call write_source_line( log_file)

         symbol_ptr => symbol_ptr% next

      enddo all_symbols

! ----------------------------------------------------------------------

!  report extension

      extensions: if( options% report_extensions )then               ! dump is an extension

         call msg_continue( "processed dump directive")              ! remark dump

      endif extensions                                               ! dump is an extension

   endif active_line                                                 ! if active dump line

! ----------------------------------------------------------------------

!  process_dump_directive() exit

return                                                               ! process_dump_directive()

! **********************************************************************

!  process_dump_directive()

end subroutine process_dump_directive

! **********************************************************************
! **********************************************************************

!  process_text_directive() process an text declaration

subroutine process_text_directive( text_dir)

! **********************************************************************

!  process_text_directive() interface

! ----------------------------------------------------------------------

!  a statement containing a text directive

character( len= *), intent( in) :: text_dir                         

! **********************************************************************

!  entry: text_dir is quoted text condition

!  exit: text code is stored in the text variable on the symbol list

! **********************************************************************

!  process_text_directive() local

! ----------------------------------------------------------------------

!  name of symbol

   character( len= symbol_name_len) :: text_name                     ! maximum length is 32 characters

!  results of decoding statement

   integer :: symbol_len                                             ! length of string decoded

!  point to next character to be decoded

   integer :: next_char                                              ! next character to process

! **********************************************************************

!  process_text_directive() text

continue                                                             ! process_text_directive()

   next_char = 1

! ----------------------------------------------------------------------

!  if active block, process text declaration

   active_line: if( if_construct% now_selected )then                 ! if active text line

!  extract text name

      call get_text_name( text_dir( next_char: ), text_name, symbol_len)

!  store text in symbol list

      next_char = next_char + symbol_len                             ! character after symbol name

      call add_text( text_dir( next_char: ), text_name)              ! initialize text symbol

! ----------------------------------------------------------------------

!  report extension

      extensions: if( options% report_extensions )then               ! texts are extensions

         call msg_continue( "processed text declaration " // trim( text_name) )

      endif extensions                                               ! texts are extensions

!  count text blocks

      total% text_blocks = total% text_blocks + 1

   endif active_line                                                 ! if active text line

! ----------------------------------------------------------------------

!  process_text_directive() exit

return                                                               ! process_text_directive()

! **********************************************************************

!  process_text_directive()

end subroutine process_text_directive

! **********************************************************************
! **********************************************************************

!  add_text() copy text block to symbol table

subroutine add_text( text_decl_str, text_name)

! **********************************************************************

!  add_text() interface

! ----------------------------------------------------------------------

!  a statement containing a text declaration

character( len= *), intent( in) :: text_decl_str

!  the name of the text

character( len= *), intent( in) :: text_name                        

! **********************************************************************

!  entry: mac_decl_str is blank_compress_lower_case logical declaration statement past the double colon
!         "" | "=..."
!         text_name is the symbol name
!         is_const is true if this is a constant declaration
!         processing_set_file is true if this was found in the setfile

!  exit: logical declaration is added to the logical symbol list or error exit

! **********************************************************************

!  add_text() constants

! ----------------------------------------------------------------------

!  end of a text block

   character( len= *), parameter :: endtext_str = 'endtext'          ! end of block

! **********************************************************************

!  add_text() local

! ----------------------------------------------------------------------

!  pointer to pre-existing symbol

   type( symbol_t), pointer :: symbol_ptr                            ! from seek_symbol_name()

!  location of close parenthesis

   integer :: close_idx                                              ! find end of arg list

!  buffer for text argument list

   character( len= buffer_len) :: arg_list_str                       ! process arg list into array

! ----------------------------------------------------------------------

!  the text scratch file

   type( file_t) :: text_file                                        ! file variable

! ----------------------------------------------------------------------

!  copy buffer

   character( len= buffer_len) :: statement                          ! check for directives

!  lines of text in the text block

   integer :: this_line                                              ! loop index to copy lines

   integer :: this_arg

   character( len= target_len) :: search_str

   integer :: arg_idx

   integer :: arg_len

   logical :: complete

   integer :: next_char

!  allocation status

   integer :: astat                                                  ! check status

! **********************************************************************

!  add_text() text

continue                                                             ! add_text()

! ----------------------------------------------------------------------

!  check if already on text list

   call seek_symbol_name( text_name, symbol_ptr)                     ! check text list

   have_a_name: if( associated( symbol_ptr) )then                    ! if name already on list

      duplicate_name: select case( symbol_ptr% type_code)
   
      case( type_text) duplicate_name
   
         call msg_quit( "duplicate text declaration: " // trim( text_name) )

      case( type_integer) duplicate_name
   
         call msg_quit( "text name already defined as an integer: " // trim( text_name) )

      case( type_logical) duplicate_name
   
         call msg_quit( "text name already defined as a logical: " // trim( text_name) )

      case( type_macro) duplicate_name
   
         call msg_quit( "text name already defined as a macro: " // trim( text_name) )

      case default duplicate_name

         call msg_quit( "duplicate name: " // trim( text_name) )

      end select duplicate_name

   endif have_a_name                                                 ! if name already on list

! ----------------------------------------------------------------------

!  allocate next text

   call add_symbol( text_name, symbol_ptr)                           ! store in list

! ----------------------------------------------------------------------

!  build complete text entry

   symbol_ptr% type_code = type_text                                 ! entry is a text block

!  determine if declaration specifies an arg list

   got_arglist: if( text_decl_str( : len( open_paren)) == open_paren )then

      call seek_close_paren( text_decl_str, len( open_paren), close_idx)

      no_close_paren: if( close_idx == 0 )then

         call msg_quit( "no closing parenthesis: " // trim( text_decl_str) )

      endif no_close_paren

      arg_list_str = text_decl_str( len( open_paren) + 1: close_idx - 1)

      call process_dummy_arglist( arg_list_str, symbol_ptr)

      call verify_dummy_args( symbol_ptr)                            ! dummy args must be names

   else got_arglist                                                  ! has arglist or not

      nullify( symbol_ptr% dummy_args)

   endif got_arglist                                                 ! has arglist or not

! ----------------------------------------------------------------------

!  store text value (read into scratch file, count lines, allocate storage, copy to storage)

! ----------------------------------------------------------------------

!  initialize the text file variable

   text_file = file_t( text_unit, null_string, &                     ! unit, name
                             null_string, null(), &                  ! format, line pointer
                             0, 0, &                                 ! iostat, lines transfered
                             .false., .true.)                        ! named, <ignored>

!  open the set text file

   call open_scratch( text_file)                                     ! open text scratch file

!  start as if with a complete statement

   complete = .true.                                                 ! read a complete text statement

! ----------------------------------------------------------------------

!  main read setfile lines loop

   read_lines: do                                                    ! read setfile line by line

! ----------------------------------------------------------------------

!  read a text line from the current source file

      read( unit= current_file% logical_unit, fmt= current_file% format_str, &
            iostat= current_file% io_status) current_file% line

      read_error: if( current_file% io_status > 0 )then              ! read error

         call msg_quit( "read text failed: " // trim( current_file% name_str))

      endif read_error                                               ! read error

! ----------------------------------------------------------------------

!  read until end of file or complete statement

      read_eof: if( current_file% io_status < 0 )then                ! eof

         total% input_lines = total% input_lines + current_file% lines_transfered

         call msg_quit( "end of file encountered within text block")

      endif read_eof                                                 ! eof

!  count lines

      current_file% lines_transfered = current_file% lines_transfered + 1

      total% text_lines = total% text_lines + 1

!  write all lines to the output as coco lines

      call write_coco_line( output_file)

! ----------------------------------------------------------------------

!  process coco lines

      coco_line: if( current_file% line( : len( coco_key)) == coco_key )then

!  count coco lines

         total% coco_lines = total% coco_lines + 1                       ! one more coco line

!  ignore coco comments

         coco_statement: if( is_coco_statement( current_file% line( len( coco_key) + 1: )) )then

!  gather a complete statement

            call gather_coco_statement( current_file% line, statement, complete)

!  if incomplete, go get rest of statement

            got_statement: if( .not. complete )then

               cycle read_lines

            endif got_statement

! ----------------------------------------------------------------------

!  check for the end text statement

            end_text: if( statement == endtext_str // symbol_ptr% name_str &
                     .or. statement == endtext_str )then             ! or end text

               exit read_lines                                       ! exit at end text name

            endif end_text                                           ! end of text block

!  check for certain directives in the text block

            call verify_text_directive( statement)                   ! check for subset of directives

         endif coco_statement

!  source lines

      else coco_line                                                 ! line is a coco line

         continuation_error: if( .not. complete )then

            call msg_quit( "source line in continued coco statement")

         endif continuation_error

!  end processing text statements

      endif coco_line                                                ! line is a coco line

!  write the text line

      write( unit= text_file% logical_unit, iostat= text_file% io_status) text_file% line

      write_text: if( text_file% io_status > 0 )then                 ! write error

         call msg_quit( "write text file failed: " // trim( text_file% line))

      endif write_text                                               ! write error

! count text lines

      text_file% lines_transfered = text_file% lines_transfered + 1

!  end main read setfile lines loop

   enddo read_lines                                                  ! read setfile line by line

! ----------------------------------------------------------------------

!  check for no lines in text block

   null_text: if( text_file% lines_transfered == 0 )then

      call close_scratch( text_file)

      call msg_quit( "text block has no lines: " // trim( symbol_ptr% name_str) )

   endif null_text

! ----------------------------------------------------------------------

!  allocate array for text

   allocate( symbol_ptr% text_lines( text_file% lines_transfered), stat= astat)

   alloc_error: if( astat > 0 )then                                  ! allocate error

      call msg_quit( "allocate text block failed " // trim( text_name) )

   endif alloc_error                                                 ! allocate error

!  count text lines defined

   total% text_lines = total% text_lines + 1


!  rewind text scratch file

   rewind( unit= text_file% logical_unit, iostat= text_file% io_status)

   rewind_text: if( text_file% io_status > 0 )then                   ! rewind error

      call msg_quit( "rewind text scratch file failed")

   endif rewind_text                                                 ! rewind error

!  copy text scratch file to array

   copy: do this_line = 1, size( symbol_ptr% text_lines)

      symbol_ptr% text_lines( this_line) = blank                     ! set line blank

      read( unit= text_file% logical_unit, iostat= text_file% io_status) symbol_ptr% text_lines( this_line)

      read_text: if( text_file% io_status > 0 )then                  ! read error

         call msg_quit( "read text scratch file failed")

      endif read_text                                                ! read error

!  force lines to lower case

      each_char: do next_char = 1, len( symbol_ptr% text_lines( this_line))

         to_lower: select case( symbol_ptr% text_lines( this_line)( next_char: next_char))

         case( 'A': 'Z') to_lower

            symbol_ptr% text_lines( this_line)( next_char: next_char) = &
               achar( iachar( symbol_ptr% text_lines( this_line)( next_char: next_char)) + change_case)

         end select to_lower

      enddo each_char

   enddo copy                                                        ! read each line

!  verify whether each dummy arg appears in the text block somewhere

   has_dummy_args: if( associated( symbol_ptr% dummy_args) )then

      check_arg: do this_arg = lbound( symbol_ptr% dummy_args, dim= 1), ubound( symbol_ptr% dummy_args, dim= 1)

         arg_idx = 0

         search_str = arg_key // trim( symbol_ptr% dummy_args( this_arg) ) // arg_key

         arg_len = len_trim( search_str)

         check_line: do this_line = lbound( symbol_ptr% text_lines, dim= 1), ubound( symbol_ptr% text_lines, dim= 1)

            arg_idx = max( arg_idx, index( symbol_ptr% text_lines( this_line), search_str( : arg_len) ) )

         enddo check_line

         not_found: if( arg_idx == 0 )then

            call msg_quit( "dummy arg " // search_str( : arg_len) &
                           // " not found in text: " // trim( symbol_ptr% name_str) )

         endif not_found

      enddo check_arg

   endif has_dummy_args

!  close text scratch file

   call close_scratch( text_file)

! ----------------------------------------------------------------------

!  add_text() exit

return                                                               ! add_text()

! **********************************************************************

!  add_text()

end subroutine add_text

! **********************************************************************
! **********************************************************************

!  verify_text_directive() check that no invalid directives appear in a text block

subroutine verify_text_directive( text_stmt)

! **********************************************************************

!  verify_text_directive() interface

! ----------------------------------------------------------------------

!  a statement from a text block

character( len= *), intent( in) :: text_stmt                       

! **********************************************************************

!  entry: text_stmt is a blank_compress_lower_case coco directive past the coco key
!         which must not contain setfile directives or any of:
!         "include..." | "integer..." | "logical..." |
!         "macro..." | "text..." | "copy..."

!  exit: if it exists, the directive is found and flagged

! **********************************************************************

!  verify_text_directive() local

   logical :: flag

! **********************************************************************

!  verify_text_directive() text

continue                                                             ! verify_text_directive()

! ----------------------------------------------------------------------

!  which directive?

   flag = .false.                                                    ! preset flag

! ----------------------------------------------------------------------

!  stop directive

   which_directive: if( text_stmt( : len( include_str)) == include_str )then

      flag = .true.                                                  ! found directive

! ----------------------------------------------------------------------

!  integer declaration

   elseif( text_stmt( : len( integer_str)) == integer_str )then which_directive

      flag = .true.                                                  ! found directive

! ----------------------------------------------------------------------

!  integer constant declaration

   elseif( text_stmt( : len( integer_constant_str)) == integer_constant_str )then which_directive

      flag = .true.                                                  ! found directive

! ----------------------------------------------------------------------

!  logical declaration

   elseif( text_stmt( : len( logical_str)) == logical_str )then which_directive

      flag = .true.                                                  ! found directive

! ----------------------------------------------------------------------

!  logical constant declaration

   elseif( text_stmt( : len( logical_constant_str)) == logical_constant_str )then which_directive

      flag = .true.                                                  ! found directive

! ----------------------------------------------------------------------

!  macro declaration

   elseif( text_stmt( : len( macro_str)) == macro_str )then which_directive

      flag = .true.                                                  ! found directive

! ----------------------------------------------------------------------

!  text declaration

   elseif( text_stmt( : len( text_str)) == text_str )then which_directive

      flag = .true.                                                  ! found directive

! ----------------------------------------------------------------------

!  copy directive

   elseif( text_stmt( : len( copy_str)) == copy_str )then which_directive

      flag = .true.                                                  ! found directive

! ----------------------------------------------------------------------

!  which directive?

   endif which_directive                                             ! check directives

! ----------------------------------------------------------------------

!  complain and quit if any directives found which shouldn't be in a text block

   dir_check: if( flag )then                                         ! no directives

      call msg_quit( "illegal directive in text block: " &
                     // trim( text_stmt) )

   endif dir_check                                                   ! no directives

! ----------------------------------------------------------------------

!  verify_text_directive() exit

return                                                               ! verify_text_directive()

! **********************************************************************

!  verify_text_directive()

end subroutine verify_text_directive

! **********************************************************************
! **********************************************************************

!  process_copy_directive() process a coco copy directive

subroutine process_copy_directive( copy_dir)

! **********************************************************************

!  process_copy_directive() interface

! ----------------------------------------------------------------------

!  a statement containing a copy directive

character( len= *), intent( in) :: copy_dir

! **********************************************************************

!  entry: copy directive

!  exit: the directive is processed or error exit

! **********************************************************************

!  process_copy_directive() constants

! ----------------------------------------------------------------------

!  mark beginning and end of text

   character( len= *), parameter :: begin_txt = '??! TEXT '          ! mark beginning

   character( len= *), parameter :: end_txt = '??! END TEXT '        ! mark end

! **********************************************************************

!  process_copy_directive() local

! ----------------------------------------------------------------------

!  use name from directive to find text block pointer

   character( len= symbol_name_len) :: text_name

!  length of text block name

   integer :: name_len

!  find beginning of name

   integer :: name_idx

!  find end of name

   integer :: end_name_idx

!  pointer to text block

   type( symbol_t), pointer :: text_ptr                              ! point to text

!  gather a coco statement from the text block

   character( len= buffer_len) :: statement                          ! a coco construct

!  loop through the text block lines

   integer :: this_line                                              ! do lines index

!  find open parenthesis on copy directive

   integer :: open_paren_idx                                         ! open parenthesis

!  find close parenthesis

   integer :: close_paren_idx                                        ! close parenthesis

!  number of lines in text block or zero if none

   integer :: text_lines_size

!  if args found

   logical :: process_args                                           ! process args if present

!  communicate with gather_statement()

   logical :: complete                                               ! signal a complete statement

! **********************************************************************

!  process_copy_directive() text

continue                                                             ! process_copy_directive()

! ----------------------------------------------------------------------

!  check for valid directive

   name_idx = 1                                                      ! beginning of name

   call get_text_name( copy_dir, text_name, name_len)                ! find end of name

   end_name_idx = name_idx + name_len - 1

   call get_text_ptr( copy_dir( name_idx: end_name_idx), text_ptr)

   check_name: if( .not. associated( text_ptr) )then                 ! not found

      call msg_quit( "text name not found: " // trim( copy_dir( name_idx: )) )

   endif check_name                                                  ! not found

! ----------------------------------------------------------------------

!  if active block, process text declaration

   active_line: if( if_construct% now_selected )then                 ! if active copy line

!  test first character after name

      open_paren_idx = end_name_idx + 1

!  check that if text has dummy args, copy has actual args, and vice versa

      have_args: if( associated( text_ptr% dummy_args) )then         ! text has dummy args

!  text with args

         no_args: if( copy_dir( open_paren_idx: open_paren_idx) /= open_paren )then

            call msg_quit( "no actual arguments for text: " // trim( text_ptr% name_str) )

         endif no_args                                               ! check for actual args

         call seek_close_paren( copy_dir, open_paren_idx, close_paren_idx)

         process_args = .true.                                       ! substitute

      else have_args                                                 ! text block has/has not args

!  text without args

         got_args: if( copy_dir( open_paren_idx: open_paren_idx) == open_paren )then

            call msg_quit( "no dummy arguments for text: " // trim( copy_dir))

         endif got_args                                              ! check for actual args

         process_args = .false.

!  block has/has not args

      endif have_args                                                ! text block has/has not args

! ----------------------------------------------------------------------

!  mark the beginning of the text

      line = begin_txt // text_ptr% name_str                         ! begin text block

      call write_coco_line( output_file)                             ! write as coco line

! ----------------------------------------------------------------------

!  get number of lines in text block

      have_lines: if( associated( text_ptr% text_lines) )then

         text_lines_size = size( text_ptr% text_lines)

      else have_lines

         text_lines_size = 0

      endif have_lines

! ----------------------------------------------------------------------

!  loop thru text block lines

      copy_lines: do this_line = 1, text_lines_size                  ! copy all the lines

         line = text_ptr% text_lines( this_line)

!  coco lines or source lines

         coco_lines: if( line( : len( coco_key)) == coco_key )then

!  write coco line to the output

            call write_coco_line( output_file)                       ! as per the alter state

! ----------------------------------------------------------------------

!  process coco lines, ignore coco comments

            coco_construct: if( is_coco_statement( line( len( coco_key) + 1: )) )then

!  gather a complete coco statement

               call gather_coco_statement( line, statement, complete)

!  if not yet a complete statement, get next line

               incomplete: if( .not. complete )then

                  cycle copy_lines

               endif incomplete

!  process (permitted in a block) directives

               call process_block_directive( statement)

               output_file% line => line                             ! relink buffer

            endif coco_construct

! ----------------------------------------------------------------------

!  source lines

         else coco_lines                                             ! coco or source

!  if args substitute in text line

            go_args: if( process_args )then                          ! process args

               text_ptr% macro_value = line

               call process_actual_arglist( copy_dir( open_paren_idx + 1: close_paren_idx - 1), &
                                            line, text_ptr)

            endif go_args                                            ! process args

!  editing source lines is enabled

            edit_source_args: if( options% edit_source )then         ! edit source lines

!  if ? present, edit source line

               edit_line_args: if( index( line, arg_key) > 0 )then

                  call edit_source_line( line)                       ! edit integers, logicals, macros

               endif edit_line_args                                  ! if have ?name?

            endif edit_source_args                                   ! edit source lines

!  finally, write out the line

            call write_source_line( output_file)                     ! write as source line

         endif coco_lines                                            ! coco or source

      enddo copy_lines                                               ! copy all the lines

      total% copied_lines = total% copied_lines + text_lines_size

!  mark the end of the text

      line = end_txt // text_ptr% name_str                           ! end of text block

      call write_coco_line( output_file)                             ! write as coco line

! ----------------------------------------------------------------------

!  report extension

      extensions: if( options% report_extensions )then               ! copys are extensions

         call msg_continue( "processed copy directive " // trim( text_ptr% name_str) )

      endif extensions                                               ! copys are extensions

!  process active lines only

   endif active_line                                                 ! if active text line

! ----------------------------------------------------------------------

!  process_copy_directive() exit

return                                                               ! process_copy_directive()

! **********************************************************************

!  process_copy_directive()

end subroutine process_copy_directive

! **********************************************************************
! **********************************************************************

!  process_copyif_directive() process a coco copyif directive

subroutine process_copyif_directive( copyif_dir)

! **********************************************************************

!  process_copyif_directive() interface

! ----------------------------------------------------------------------

!  a statement containing a copy directive

character( len= *), intent( in) :: copyif_dir

! **********************************************************************

!  entry: copy directive

!  exit: the directive is processed or error exit

! **********************************************************************

!  process_copyif_directive() local

! ----------------------------------------------------------------------

!  communicate with seek_close_paren()

   integer :: logical_len                                            ! length of logical expression

!  communicate with eval_log_expr()

   logical :: logical_value                                          ! value of logical expression

! **********************************************************************

!  process_copyif_directive() text

continue                                                             ! process_copyif_directive()

! ----------------------------------------------------------------------

!  check syntax- condition must be string within quotes

   call seek_close_paren( copyif_dir, 1, logical_len )

!  verify found logical expression

   no_logical_exp: if( logical_len == 0 )then                        ! no logical expression

      call msg_quit( "can't find copyif expression: " // trim( copyif_dir))

   endif no_logical_exp                                              ! no logical expression

! ----------------------------------------------------------------------

!  if active block, process assertif directive

   active_line: if( if_construct% now_selected )then                 ! if active copyif line

!  evaluate logical expression

      call eval_log_expr( copyif_dir( 2: logical_len - 1) // blank, logical_value)

!  if true, process copy

      expr_true: if( logical_value )then                             ! value on copyif directive

         call process_copy_directive( copyif_dir( logical_len + 1: ))

      endif expr_true                                                ! value on copyif directive

! ----------------------------------------------------------------------

!  report extension

      extensions: if( options% report_extensions )then               ! copyifs are extensions

         call msg_continue( "processed copyif directive")            ! remark copyif

      endif extensions                                               ! assertifs are extensions

! ----------------------------------------------------------------------

   endif active_line                                                 ! if active copyif line

! ----------------------------------------------------------------------

!  process_copyif_directive() exit

return                                                               ! process_copyif_directive()

! **********************************************************************

!  process_copyif_directive()

end subroutine process_copyif_directive

! **********************************************************************
! **********************************************************************

!  process_block_directive() process a coco text block directive

subroutine process_block_directive( block_stmt)

! **********************************************************************

!  process_block_directive() interface

! ----------------------------------------------------------------------

!  a statement from a text block

character( len= *), intent( in) :: block_stmt

! **********************************************************************

!  entry: coco_stmt is a blank_compress_lower_case coco directive past the coco key
!         "stop..." | "message..." | "if..." | "elseif..." | "else..." |
!         "endif..." | "assert..." | "name=..."

!  exit: the directive is processed or error exit

! **********************************************************************

!  process_block_directive() local

! ----------------------------------------------------------------------

!  point to location of symbol

   type( symbol_t), pointer :: symbol_ptr                            ! symbol pointer

!  pointer to equals

   integer :: eq_idx                                                 ! find the equals

!  expression string is after the equals

   integer :: expr_idx                                               ! expression follows the equals

! **********************************************************************

!  process_block_directive() text

continue                                                             ! process_block_directive()

! ----------------------------------------------------------------------

!  which directive?

! ----------------------------------------------------------------------

!  detect assignment statements assigning to variables named by keywords

      eq_idx = scan( block_stmt( : symbol_name_len + 1), equals)     ! seach a name's worth of statement

      got_equals: if( eq_idx > 0 )then                               ! if found '='

         call seek_symbol_name( block_stmt( : eq_idx - 1), symbol_ptr)

      endif got_equals                                               ! if found '='

! ----------------------------------------------------------------------

!  which directive?

! ----------------------------------------------------------------------

!  assignment directive

   which_directive: if( associated( symbol_ptr) )then

!  up to the equals must be a declared name

      expr_idx = eq_idx + len( equals)                               ! start of expression

!  must be an integer or logical variable

      integer_or_logical: select case( symbol_ptr% type_code)        ! which type
         
      case( type_integer) integer_or_logical                         ! assign to integer

         call process_integer_assignment( block_stmt( expr_idx: ), symbol_ptr)

      case( type_logical) integer_or_logical                         ! assign to logical

         call process_logical_assignment( block_stmt( expr_idx: ), symbol_ptr)

      case default integer_or_logical                                ! must assign to integer or logical

         call msg_quit( "variable must be an integer or a logical: " // trim( symbol_ptr% name_str) )
         
      end select integer_or_logical                                  ! which type

      nullify( symbol_ptr)                                           ! done

! ----------------------------------------------------------------------

!  stop directive

   elseif( block_stmt( : len( stop_str)) == stop_str )then which_directive

      call process_stop_directive( block_stmt( len( stop_str) + 1: ) )

! ----------------------------------------------------------------------

!  message directive

   elseif( block_stmt( : len( message_str)) == message_str )then which_directive

      call process_message_directive( block_stmt( len( message_str) + 1: ) )

! ----------------------------------------------------------------------

!  if directive

   elseif( block_stmt( : len( if_str)) == if_str )then which_directive

      call process_if_directive( block_stmt( len( if_str) + 1: ) )

! ----------------------------------------------------------------------

!  elseif directive

   elseif( block_stmt( : len( elseif_str)) == elseif_str )then which_directive

      call process_elseif_directive( block_stmt( len( elseif_str) + 1: ) )

! ----------------------------------------------------------------------

!  else directive

   elseif( block_stmt( : len( else_str)) == else_str )then which_directive

      call process_else_directive( block_stmt( len( else_str) + 1: ) )     ! go process else

! ----------------------------------------------------------------------

!  endif directive

   elseif( block_stmt( : len( endif_str)) == endif_str )then which_directive

      call process_endif_directive( block_stmt( len( endif_str) + 1: ) )

! ----------------------------------------------------------------------

!  assert declaration

   elseif( block_stmt( : len( assert_str)) == assert_str )then which_directive

      call process_assert_directive( block_stmt( len( assert_str) + 1: ))

! ----------------------------------------------------------------------

!  cannot process this directive

   else which_directive                                              ! symbol declared

         call msg_quit( "unknown block directive: " // trim( block_stmt))

! ----------------------------------------------------------------------

!  which directive?

   endif which_directive                                             ! process directives

! ----------------------------------------------------------------------

!  process_block_directive() exit

return                                                               ! process_block_directive()

! **********************************************************************

!  process_block_directive()

end subroutine process_block_directive

! **********************************************************************
! **********************************************************************

!  process_doc_directive() process a coco doc directive

subroutine process_doc_directive( doc_dir)

! **********************************************************************

!  process_doc_directive() interface

! ----------------------------------------------------------------------

!  a statement containing a copy directive

character( len= *), intent( in) :: doc_dir

! **********************************************************************

!  entry: doc directive

!  exit: the directive is processed or error exit

! **********************************************************************

!  process_doc_directive() constants

! ----------------------------------------------------------------------

!  mark beginning and end of text

   character( len= *), parameter :: enddoc_str = 'enddoc'            ! find end

! **********************************************************************

!  process_doc_directive() local

! ----------------------------------------------------------------------

!  mark beginning and end of text

   character( len= buffer_len) :: statement                          ! need coco statement to find end doc

   logical :: complete                                               ! true when complete statement read

! **********************************************************************

!  process_doc_directive() text

continue                                                             ! process_doc_directive()

! ----------------------------------------------------------------------

!  check for valid directive

   valid_doc: if( doc_dir /= blank )then

      call msg_quit( "invalid characters at end of doc directive: " // doc_dir)

   endif valid_doc

! ----------------------------------------------------------------------

!  if active block, process text declaration

   active_line: if( if_construct% now_selected )then                 ! if active copy line

!  if no doc file open, error

      no_doc: if( .not. doc_file% named_file )then

         call msg_quit( 'no document file when doc directive encountered')

      endif no_doc

!  connect doc file to line buffer

      doc_file% line => line

!  read until end doc found or ( enf or error)

      copy_lines: do

!  read doc lines from input

         read( unit= current_file% logical_unit, fmt= current_file% format_str, &
               iostat= current_file% io_status) current_file% line

         read_error: if( current_file% io_status > 0 )then           ! read error

            call msg_quit( "read input failed: " // trim( current_file% name_str))

         endif read_error                                            ! read error

! ----------------------------------------------------------------------

!  read until end of file or complete statement

         read_eof: if( current_file% io_status < 0 )then             ! eof

            total% input_lines = total% input_lines + current_file% lines_transfered

            call msg_quit( "end of file encountered within doc ... end doc")

         endif read_eof                                              ! eof

!  write line to output as coco line

         call write_coco_line( output_file)

!  count current file lines

         current_file% lines_transfered = current_file% lines_transfered + 1

! ----------------------------------------------------------------------

!  process setfile lines or error if source lines

         coco_line: if( line( : len( coco_key)) == coco_key )then    ! coco lines start with '??'

!  count coco lines

            total% coco_lines = total% coco_lines + 1                ! one more coco line

!  process setfile lines, ignore coco comments

            coco_statement: if( is_coco_statement( line( len( coco_key) + 1: )) )then

! ----------------------------------------------------------------------

!  read a complete statement line by line

               call gather_coco_statement( line, statement, complete)

!  if not yet a complete statement go get the rest of it

               get_statement: if( .not. complete )then               ! if not yet a complete statement

                  cycle copy_lines                                   ! read another line

               endif get_statement                                   ! if not yet a complete statement

!  check statement for end doc directive

               got_end_doc: if( statement == enddoc_str )then

                  exit copy_lines

               endif got_end_doc

!  process setfile lines, ignore coco comments

            endif coco_statement                                     ! non-comment line

!  process setfile lines, ignore coco comments

         endif coco_line                                             ! non-comment line

!  editing source lines is enabled

         edit_source: if( options% edit_source )then                 ! edit source lines

!  if ? present, edit source line

            edit_line: if( index( line, arg_key) > 0 )then           ! if have ?name?

               call edit_source_line( line)                          ! edit integers, logicals, macros

            endif edit_line                                          ! if have ?name?

         endif edit_source                                           ! edit source lines

!  write line to doc file

         call write_source_line( doc_file)

!  enddo

      enddo copy_lines

!  disconnect doc file from line buffer

      nullify( doc_file% line)

! ----------------------------------------------------------------------

!  report extension

      extensions: if( options% report_extensions )then               ! copys are extensions

         call msg_continue( "processed doc ... end doc directive")

      endif extensions                                               ! copys are extensions

!  process active lines only

   endif active_line                                                 ! if active text line

! ----------------------------------------------------------------------

!  process_doc_directive() exit

return                                                               ! process_doc_directive()

! **********************************************************************

!  process_doc_directive()

end subroutine process_doc_directive

! **********************************************************************
! **********************************************************************

!  process_output_directive() process include output options

subroutine process_output_directive( output_dir)

! **********************************************************************

!  process_output_directive() interface

! ----------------------------------------------------------------------

!  the output directive from the setfile

character( len= *), intent( in) :: output_dir                      

! **********************************************************************

!  entry: output_opt is a output to be added to the list
!         of directories to be searched for inlcude files

!  exit: output_opt is on the list

! **********************************************************************

!  process_output_directive() local

! ----------------------------------------------------------------------

!  the name of the file to be opened

   character( len= filename_len) :: output_name

!  the length of the quoted string

   integer :: quoted_len

!  the length of the unquoted string

   integer :: unquoted_len

! **********************************************************************

!  process_output_directive() text

continue                                                             ! process_output_directive()

! ----------------------------------------------------------------------

!  unquote string on directive

   call unquote_string( output_dir, output_name, unquoted_len, quoted_len)

   no_name: if( unquoted_len == 0 .or. quoted_len == 0 )then

      call msg_quit( "no name found on output directive: " // trim( output_dir) )

   endif no_name

!  verify no extra characters beyond name

   extra_chars: if( output_dir( unquoted_len + 1: ) /= blank )then

      call msg_quit( "extra characters after output file name: " // trim( output_dir))

   endif extra_chars

! ----------------------------------------------------------------------

!  if line is an active line

   active_line: if( if_construct% now_selected )then

!  if the output file has content, copy the setfile to it

   made_output: if( output_file% lines_transfered > 0 )then          ! if any actual output

!  mark the setfile in the output (whether it is present or not)

         line = mark_set_file

         call write_coco_line( output_file)                          ! use the current alter setting

! ----------------------------------------------------------------------

!  if processed a setfile

         append_set_file: if( set_file% named_file )then             ! copy setfile

!  copy setfile file to output

            call copy_set_file                                       ! setfile to output

!  if processed setfile

         endif append_set_file                                       ! copy setfile

      endif made_output                                              ! if actual output

!  close the current output file

      call close_file( output_file)                                  ! close current output

!  report to logfile

      summary: if( options% print_summary )then                      ! if summary requested

         call write_report                                           ! lines read, written, etc.

      endif summary                                                  ! if summary requested

! ----------------------------------------------------------------------

!  set new name and reset line count

      output_file% name_str = output_name                            ! new name

      output_file% lines_transfered = 0                              ! yet on the new file

!  open the new output

      call open_file( output_file)                                   ! open the output

! ----------------------------------------------------------------------

!  report extension

      extensions: if( options% report_extensions )then               ! copys are extensions

         call msg_continue( "opened new output file: " // trim( output_file% name_str) )

      endif extensions                                               ! copys are extensions

   endif active_line

! ----------------------------------------------------------------------

!  process_output_directive() exit

return                                                               ! process_output_directive()

! **********************************************************************

!  process_output_directive()

end subroutine process_output_directive

! **********************************************************************
! **********************************************************************

!  seek_symbol_name() seek symbol on symbol list

subroutine seek_symbol_name( symbol_str, symbol_ptr)

! **********************************************************************

!  seek_symbol_name() interface

! ----------------------------------------------------------------------

!  the name of the symbol being sought

character( len= *), intent( in) :: symbol_str

!  a pointer to the symbol found

type( symbol_t), pointer :: symbol_ptr

! **********************************************************************

!  entry: symbol_str is blank_compress_lower_case logical symbol directive
!         "name..."

!  exit: symbol found or not in logical symbol array

! **********************************************************************

!  seek_symbol_name() text

continue                                                             ! seek_symbol_name()

! ----------------------------------------------------------------------

!  search symbol list

   nullify( symbol_ptr)                                              ! ensure pointer is ready for use

   symbol_ptr => first_symbol                                        ! start of symbol list

   search_list: do while( associated( symbol_ptr) )                  ! find name

      name_match: if( symbol_str == symbol_ptr% name_str )then       ! if names match

         exit search_list                                            ! eureka

      endif name_match                                               ! if names match

      symbol_ptr => symbol_ptr% next                                 ! check next symbol on list

   enddo search_list                                                 ! find name

! ----------------------------------------------------------------------

!  seek_symbol_name() exit

return                                                               ! seek_symbol_name()

! **********************************************************************

!  seek_symbol_name()

end subroutine seek_symbol_name

! **********************************************************************
! **********************************************************************

!  get_integer_value() seek symbol on symbol list

subroutine get_integer_value( integer_str, return_value)

! **********************************************************************

!  get_integer_value() interface

! ----------------------------------------------------------------------

!  the name of the integer whose value is sought

character( len= *), intent( in) :: integer_str

!  the value of the integer

integer, intent( out) :: return_value                              

! **********************************************************************

!  entry: symbol_str is blank_compress_lower_case logical symbol directive
!         "name..."

!  exit: symbol found or not in logical symbol array

! **********************************************************************

!  get_integer_value() local

! ----------------------------------------------------------------------

!  pointer to search symbol list

   type( symbol_t), pointer :: symbol_ptr

! **********************************************************************

!  get_integer_value() text

continue                                                             ! get_integer_value()

! ----------------------------------------------------------------------

!  search symbol list

   symbol_ptr => first_symbol                                        ! start of symbol list

   search_list: do while( associated( symbol_ptr) )                  ! find name

      name_match: if( integer_str == symbol_ptr% name_str )then      ! if names match

         right_type: select case( symbol_ptr% type_code)

         case( type_integer) right_type

            value_defined: if( symbol_ptr% defined )then

               return_value = symbol_ptr% integer_value

               return

            else value_defined

               call msg_quit( "integer not defined: " // trim( integer_str) )

            endif value_defined

         case default right_type

            call msg_quit( "need integer, not: " // trim( integer_str) )

         end select right_type

      endif name_match                                               ! if names match

      symbol_ptr => symbol_ptr% next                                 ! check next symbol on list

   enddo search_list                                                 ! find name

! ----------------------------------------------------------------------

!  integer not found

   call msg_quit( "unknown integer: " // trim( integer_str) )

! ----------------------------------------------------------------------

!  get_integer_value() exit

return                                                               ! get_integer_value()

! **********************************************************************

!  get_integer_value()

end subroutine get_integer_value

! **********************************************************************
! **********************************************************************

!  get_logical_value() seek symbol on symbol list

subroutine get_logical_value( logical_str, return_value)

! **********************************************************************

!  get_logical_value() interface

! ----------------------------------------------------------------------

!  the name of the logical whose value is sought

character( len= *), intent( in) :: logical_str

!  the value of the logical

logical, intent( out) :: return_value                             

! **********************************************************************

!  entry: symbol_str is blank_compress_lower_case logical symbol directive
!         "name..."

!  exit: symbol found or not in logical symbol array

! **********************************************************************

!  get_logical_value() local

! ----------------------------------------------------------------------

!  pointer to search symbol list

   type( symbol_t), pointer :: symbol_ptr

! **********************************************************************

!  get_logical_value() text

continue                                                             ! get_logical_value()

! ----------------------------------------------------------------------

!  search symbol list

   symbol_ptr => first_symbol                                        ! start of symbol list

   search_list: do while( associated( symbol_ptr) )                  ! find name

      name_match: if( logical_str == symbol_ptr% name_str )then      ! if names match

         right_type: select case( symbol_ptr% type_code)

         case( type_logical) right_type

            value_defined: if( symbol_ptr% defined )then

               return_value = symbol_ptr% logical_value

               return

            else value_defined

               call msg_quit( "logical not defined: " // trim( logical_str) )

            endif value_defined

         case default right_type

            call msg_quit( "need logical, not: " // trim( logical_str) )

         end select right_type

      endif name_match                                               ! if names match

      symbol_ptr => symbol_ptr% next                                 ! check next symbol on list

   enddo search_list                                                 ! find name

! ----------------------------------------------------------------------

!  logical not found

   call msg_quit( "unknown logical: " // trim( logical_str) )

! ----------------------------------------------------------------------

!  get_logical_value() exit

return                                                               ! get_logical_value()

! **********************************************************************

!  get_logical_value()

end subroutine get_logical_value

! **********************************************************************
! **********************************************************************

!  get_next_integer() seek symbol on symbol list

subroutine get_next_integer( integer_ptr)

! **********************************************************************

!  get_next_integer() interface

! ----------------------------------------------------------------------

!  a pointer to the next integer on the symbol list

type( symbol_t), pointer :: integer_ptr

! **********************************************************************

!  entry: symbol_str is blank_compress_lower_case logical symbol directive
!         "name..."

!  exit: symbol found or not in logical symbol array

! **********************************************************************

!  get_next_integer() text

continue                                                             ! get_next_integer()

! ----------------------------------------------------------------------

!  start at the symbol list head or continue from previous integer

   start_or_continue: if( associated( integer_ptr) )then             ! if already searching

      integer_ptr => integer_ptr% next                               ! continue with the next integer

   else start_or_continue                                            ! if have not started

      integer_ptr => first_symbol                                    ! start search at the list head

   endif start_or_continue                                           ! have or have not started

!  search symbol list for integers

   search_list: do while( associated( integer_ptr) )                 ! find name

      right_type: select case( integer_ptr% type_code)               ! what type?

      case( type_integer) right_type                                 ! integer

         return                                                      ! with another integer pointer

      end select right_type                                          ! what type?

      integer_ptr => integer_ptr% next                               ! check next symbol on list

   enddo search_list                                                 ! find name

! ----------------------------------------------------------------------

!  get_next_integer() exit

return                                                               ! get_next_integer()

! **********************************************************************

!  get_next_integer()

end subroutine get_next_integer

! **********************************************************************
! **********************************************************************

!  get_next_logical() seek symbol on symbol list

subroutine get_next_logical( logical_ptr)

! **********************************************************************

!  get_next_logical() interface

! ----------------------------------------------------------------------

!  a pointer to the next logical on the symbol list

type( symbol_t), pointer :: logical_ptr

! **********************************************************************

!  entry: symbol_str is blank_compress_lower_case logical symbol directive
!         "name..."

!  exit: symbol found or not in logical symbol array

! **********************************************************************

!  get_next_logical() text

continue                                                             ! get_next_logical()

! ----------------------------------------------------------------------

!  start at the symbol list head or continue from previous logical

   start_or_continue: if( associated( logical_ptr) )then             ! if already searching

      logical_ptr => logical_ptr% next                               ! continue with the next logical

   else start_or_continue                                            ! if have not started

      logical_ptr => first_symbol                                    ! start search at the list head

   endif start_or_continue                                           ! have or have not started

!  search symbol list for logicals

   search_list: do while( associated( logical_ptr) )                 ! find name

      right_type: select case( logical_ptr% type_code)               ! what type?

      case( type_logical) right_type                                 ! logical

         return                                                      ! with another logical pointer

      end select right_type                                          ! what type?

      logical_ptr => logical_ptr% next                               ! check next symbol on list

   enddo search_list                                                 ! find name

! ----------------------------------------------------------------------

!  get_next_logical() exit

return                                                               ! get_next_logical()

! **********************************************************************

!  get_next_logical()

end subroutine get_next_logical                                    

! **********************************************************************
! **********************************************************************

!  get_next_macro() seek symbol on symbol list

subroutine get_next_macro( macro_ptr)

! **********************************************************************

!  get_next_macro() interface

! ----------------------------------------------------------------------

!  a pointer to the next macro on the symbol list

type( symbol_t), pointer :: macro_ptr

! **********************************************************************

!  entry: symbol_str is blank_compress_lower_case logical symbol directive
!         "name..."

!  exit: symbol found or not in logical symbol array

! **********************************************************************

!  get_next_macro() text

continue                                                             ! get_next_macro()

! ----------------------------------------------------------------------

!  start at the symbol list head or continue from previous macro

   start_or_continue: if( associated( macro_ptr) )then               ! if already searching

      macro_ptr => macro_ptr% next                                   ! continue with the next macro

   else start_or_continue                                            ! if have not started

      macro_ptr => first_symbol                                      ! start search at the list head

   endif start_or_continue                                           ! have or have not started

!  search symbol list for macros

   search_list: do while( associated( macro_ptr) )                   ! find name

      right_type: select case( macro_ptr% type_code)                 ! what type?

      case( type_macro) right_type                                   ! macro

         return                                                      ! with another macro pointer

      end select right_type                                          ! what type?

      macro_ptr => macro_ptr% next                                   ! check next symbol on list

   enddo search_list                                                 ! find name

! ----------------------------------------------------------------------

!  get_next_macro() exit

return                                                               ! get_next_macro()

! **********************************************************************

!  get_next_macro()

end subroutine get_next_macro

! **********************************************************************
! **********************************************************************

!  get_text_ptr() seek symbol on symbol list

subroutine get_text_ptr( text_str, text_ptr)

! **********************************************************************

!  get_text_ptr() interface

! ----------------------------------------------------------------------

!  the name of the text being sought

character( len= *), intent( in) :: text_str

!  a pointer to the text

type( symbol_t), pointer :: text_ptr                                

! **********************************************************************

!  entry: symbol_str is blank_compress_lower_case logical symbol directive
!         "name..."

!  exit: symbol found or not in logical symbol array

! **********************************************************************

!  get_text_ptr() text

continue                                                             ! get_text_ptr()

! ----------------------------------------------------------------------

!  start at the symbol list head

   nullify( text_ptr)                                                ! ensure pointer is ready to use

   text_ptr => first_symbol                                          ! start search at the list head

!  search symbol list for texts

   search_list: do while( associated( text_ptr) )                    ! find name

      right_name: if( text_ptr% name_str == text_str )then

         right_type: select case( text_ptr% type_code)               ! what type?

         case( type_text) right_type                                 ! text

            return                                                   ! with another text pointer

         case default right_type                                     ! what type?

            call msg_quit( "not a text name: " // trim( text_str) )

         end select right_type                                       ! what type?

      endif right_name

      text_ptr => text_ptr% next                                     ! check next symbol on list

   enddo search_list                                                 ! find name

! ----------------------------------------------------------------------

!  get_text_ptr() exit

return                                                               ! get_text_ptr()

! **********************************************************************

!  get_text_ptr()

end subroutine get_text_ptr

! **********************************************************************
! **********************************************************************

!  %%% diagnose and evaluate expressions

! **********************************************************************
! **********************************************************************

!  integer_or_logical() determine type of expression

subroutine integer_or_logical( expr_str, flag)

! **********************************************************************

!  integer_or_logical() interface

! ----------------------------------------------------------------------

!  an expression whose type is to be assertained

character( len= *), intent( in) :: expr_str

!  true if the type is integer

logical, intent( out) :: flag                                       

! **********************************************************************

!  entry: symbol_str is string "..."

!  exit: flag is true if string is an integer expression and false otherwise

! **********************************************************************

!  integer_or_logical() constants

! ----------------------------------------------------------------------

!  search for a character which must be part of a logical expression

   character( len= *), parameter :: logical_chars = '.<>='           ! diagnostic of logicals

!  search for a character which may be part of an integer expression or numeric inquiry

   character( len= *), parameter :: integer_chars = '+-*/%'          ! diagnostic of integers

! **********************************************************************

!  integer_or_logical() local

! ----------------------------------------------------------------------

!  search results

   integer :: char_idx                                               ! character location

   type( symbol_t), pointer ::  symbol_ptr                           ! point to symbol

 ! **********************************************************************

!  integer_or_logical() text

continue                                                             ! integer_or_logical()

! ----------------------------------------------------------------------

!  does string contain a character which is only in logical expressions?

   char_idx = scan( expr_str, logical_chars)                         ! seek char

   got_dot: if( char_idx > 0 )then                                   ! find char?

      flag = .false.                                                 ! must be logical

      return                                                         ! quit

   endif got_dot                                                     ! find char?

!  does string contain a character which is only in integer expressions?

   char_idx = scan( expr_str, integer_chars)                         ! seek char

   got_op: if( char_idx > 0 )then                                    ! find char?

      flag = .true.                                                  ! must be integer

      return                                                         ! quit

   endif got_op                                                      ! find char?

! ----------------------------------------------------------------------

!  is string an integer or a logical symbol name?

   char_idx = verify( expr_str, alphanum_chars)                      ! all name characters

   got_name: if( char_idx == 0 )then                                 ! all alphanumeric?

      call seek_symbol_name( expr_str, symbol_ptr)                   ! check list

      int_name: if( associated( symbol_ptr) )then                    ! on list?

         flag = symbol_ptr% type_code == type_integer                ! must be integer

         return                                                      ! quit

      endif int_name                                                 ! on list?

   endif got_name                                                    ! all alphanumeric?

! ----------------------------------------------------------------------

!  is string all digits?

   char_idx = verify( expr_str, digit_chars)                         ! all digits

   got_digits: if( char_idx == 0 )then                               ! find char?

      flag = .true.                                                  ! must be integer

      return                                                         ! quit

   endif got_digits                                                  ! find char?

! ----------------------------------------------------------------------

!  can't classify the expression, punt

   call msg_quit( "can't classify: " // trim( expr_str) )

! ----------------------------------------------------------------------

!  integer_or_logical() exit

return                                                               ! integer_or_logical()

! **********************************************************************

!  integer_or_logical()

end subroutine integer_or_logical

! **********************************************************************
! **********************************************************************

!  eval_int_expr() evaluate int_expr as an integer

recursive subroutine eval_int_expr( int_expr, value)

! **********************************************************************

!  eval_int_expr() interface

! ----------------------------------------------------------------------

!  the integer expression to be evaluated

character( len= *), intent( in) :: int_expr

!  the value of the integer expression

integer, intent( out) :: value                                      

! **********************************************************************

!  entry: int_expr is blank_compress_lower_case integer int_expr

!  exit: true if value is int_expr value, false otherwise

! **********************************************************************

!  eval_int_expr() constants

! ----------------------------------------------------------------------

!  addition operators

   integer, parameter :: add_op_len = max( len( plus), len( minus) ) ! length

!  multiplication operators

   character( len= *), parameter :: times = '*'                      ! mul op

   character( len= *), parameter :: divby = '/'                      ! div op

   integer, parameter :: mul_op_len = max( len( times), len( divby) )

!  length of operators

   integer, parameter :: op_len = max( len( plus), len( minus), len( times), len( divby) )

! **********************************************************************

!  eval_int_expr() local

! ----------------------------------------------------------------------

!  operations to be done

   character( len= add_op_len) :: add_op                             ! add op yet to be done

   character( len= mul_op_len) :: mul_op                             ! mul op yet to be done

! ----------------------------------------------------------------------

!  next operation

   character( len= op_len) :: next_op                                ! look ahead op

! ----------------------------------------------------------------------

!  partial values of the int_expr

   integer :: l_add, r_add                                           ! left and right addends

   integer :: l_mul, r_mul                                           ! left and right multiplicors

! ----------------------------------------------------------------------

!  pointers to characters

   integer :: next_char                                              ! next character to process

   integer :: next_op_idx                                            ! points to next operation

   integer :: expr_len                                               ! length of the int_expr

   integer :: primary_len                                            ! length of integer primary

! **********************************************************************

!  eval_int_expr() text

continue                                                             ! eval_int_expr()

! ----------------------------------------------------------------------

!  limits of scan

   next_char = 1                                                     ! from beginning

   expr_len = len_trim( int_expr)                                    ! to end

!  initialize adds

   add_op = plus                                                     ! not yet done

   l_add = 0                                                         ! no values yet

! ----------------------------------------------------------------------

!  scan thru int_expr

   add_ops: do while( next_char <= expr_len)                         ! process expression

!  find a primary

      call eval_int_primary( int_expr( next_char: ), primary_len, r_add)

      next_op_idx = next_char + primary_len                          ! point to next operator

!  find next operator or end of expression

      add_end: if( next_op_idx <= expr_len )then                     ! operator within string

         next_op = int_expr( next_op_idx: next_op_idx)               ! look ahead operator

         next_char = next_op_idx + 1                                 ! point to next primary

      else add_end                                                   ! operator within string

         next_op = blank                                             ! no operator remain

         next_char = next_op_idx                                     ! past end of expression

      endif add_end                                                  ! operator within string

! ----------------------------------------------------------------------

!  initialize for a set of mul ops

      mul_op = next_op                                               ! multiply next

      l_mul = r_add                                                  ! addend is now multiplicand

! ----------------------------------------------------------------------

!  process a set of mul ops

      mul_ops: do while( next_op == times .or. next_op == divby)     ! while mulops

!  find a primary

         call eval_int_primary( int_expr( next_char: ), primary_len, r_mul)

         next_op_idx = next_char + primary_len                       ! point to next operator

!  find next operator or end of expression

         mul_end: if( next_op_idx <= expr_len )then                  ! operator within string

            next_op = int_expr( next_op_idx: next_op_idx)

            next_char = next_op_idx + 1                              ! point to next primary

         else mul_end                                                ! operator within string

            next_op = blank                                          ! no operators remain

            next_char = next_op_idx                                  ! past end of expression

         endif mul_end                                               ! operator within string

!  do the pending add op

         mul_div: select case( mul_op)                               ! multiplication operators

         case( times) mul_div                                        ! multiply

            l_mul = l_mul * r_mul                                    ! compute product

         case( divby) mul_div                                        ! divide

            l_mul = l_mul / r_mul                                    ! compute quotient

         end select mul_div                                          ! multiplication operators

         mul_op = next_op                                            ! times, divby, blank

      enddo mul_ops                                                  ! process a set of mul ops

!  product is the right operand

      r_add = l_mul                                                  ! product is an addend

! ----------------------------------------------------------------------

!  do the pending add op

      add_sub: select case( add_op)                                  ! addition operators

      case( blank, plus) add_sub                                     ! addition

         l_add = l_add + r_add                                       ! compute sum

      case( minus) add_sub                                           ! subtraction

         l_add = l_add - r_add                                       ! compute difference

      case default add_sub                                           ! unknown

         call msg_quit( "unknown arithmetic operator: " // add_op)

      end select add_sub                                             ! addition operators

      add_op = next_op                                               ! next operation

   enddo add_ops                                                     ! process expression

! ----------------------------------------------------------------------

!  value of integer expression

   value = l_add                                                     ! return value computed

! ----------------------------------------------------------------------

!  eval_int_expr() exit

return                                                               ! eval_int_expr()

! **********************************************************************

!  eval_int_expr()

end subroutine eval_int_expr

! **********************************************************************
! **********************************************************************

!  eval_log_expr() expression is evaluated as a logical

recursive subroutine eval_log_expr( log_expr, value)

! **********************************************************************

!  eval_log_expr() interface

! ----------------------------------------------------------------------

!  the logical expression to be evaluated

character( len= *), intent( in) :: log_expr

!  the value of the expression

logical, intent( out) :: value                                      

! **********************************************************************

!  entry: expression is blank_compress_lower_case logical expression

!  exit: value is expression value

! **********************************************************************

!  eval_log_expr() constants

   integer, parameter :: eqv_op_len = max( len( eqv_str), len( neqv_str))

!  length of the next operator

   integer, parameter :: next_op_len = max( len( or_str), len( and_str), len( eqv_str), len( neqv_str))

! **********************************************************************

!  eval_log_expr() local

! ----------------------------------------------------------------------

!  the current eqv operator

   character( len= eqv_op_len) :: eqv_op                             ! .eqv. or .neqv.

!  the next operator

   character( len= next_op_len) :: next_op                           ! logical operator or blank

! ----------------------------------------------------------------------

!  point to characters not yet decoded

   integer :: next_char                                              ! the next character

   integer :: next_op_idx                                            ! the next operator

   integer :: expr_len                                               ! the whole expression

   integer :: primary_len                                            ! the length decoded

!  false if and but no eqv

   logical :: do_or                                                  ! do only

!  expression values

   logical :: l_eqv, l_or, l_and                                     ! left operors

   logical :: r_eqv, r_or, r_and                                     ! right operors

! **********************************************************************

!  eval_log_expr() text

continue                                                             ! eval_log_expr()

! ----------------------------------------------------------------------

!  limits of scan

   next_char = 1                                                     ! from beginning

   expr_len = len_trim( log_expr)                                    ! to end

!  initialize equivalences

   eqv_op = eqv_str                                                  ! not yet done

   l_eqv = .true.                                                    ! no values yet

! ----------------------------------------------------------------------

!  scan thru log_expr

   eqv_ops: do while( next_char <= expr_len)                         ! process .eqv. & .neqv.

!  find a primary and return its length and value

      call eval_log_primary( log_expr( next_char: ), primary_len, r_eqv)

      next_op_idx = next_char + primary_len                          ! point to next operator

!  find next operator or end of expression

      eqv_or_end: if( next_op_idx <= expr_len )then                  ! operator within string

!  decode which operator

         eqv_next_op: if( log_expr( next_op_idx: next_op_idx + len( eqv_str) - 1) == eqv_str )then

            next_op = log_expr( next_op_idx: next_op_idx + len( eqv_str) - 1)

            next_char = next_op_idx + len( eqv_str)                  ! point to next primary

         elseif( log_expr( next_op_idx: next_op_idx + len( neqv_str) - 1) == neqv_str )then eqv_next_op

            next_op = log_expr( next_op_idx: next_op_idx + len( neqv_str) - 1)

            next_char = next_op_idx + len( neqv_str)                 ! point to next primary

         elseif( log_expr( next_op_idx: next_op_idx + len( or_str) - 1) == or_str )then eqv_next_op

            next_op = log_expr( next_op_idx: next_op_idx + len( or_str) - 1)

            next_char = next_op_idx + len( or_str)                   ! point to next primary

         elseif( log_expr( next_op_idx: next_op_idx + len( and_str) - 1) == and_str )then eqv_next_op

            next_op = log_expr( next_op_idx: next_op_idx + len( and_str) - 1)

            next_char = next_op_idx + len( and_str)                  ! point to next primary

         else eqv_next_op                                            ! unknown operator

            call msg_quit( "unknown logical operator: " // trim( log_expr( next_op_idx: ) ))

         endif eqv_next_op                                           ! which operator

!  past end of expression

      else eqv_or_end                                                ! operator within string

         next_op = blank                                             ! no operators left

         next_char = next_op_idx                                     ! past end of expression

      endif eqv_or_end                                               ! operator within string

! ----------------------------------------------------------------------

!  initialize for a set of or ops

      l_or = r_eqv                                                   ! eqv-end is now or-end

! ----------------------------------------------------------------------

!  process a set of and ops

      or_ops: do while( next_op == or_str .or. next_op == and_str)

         do_or = next_op == or_str                                   ! now eqv later

         or_next: select case( do_or)                                ! or

         case( .true.) or_next                                       ! process operor after or

!  find a primary and return its length and value

            call eval_log_primary( log_expr( next_char: ), primary_len, r_or)

            next_op_idx = next_char + primary_len                    ! point to next operator

!  find next operator or end of expression

            or_end: if( next_op_idx <= expr_len )then                ! still within the expression

!  decode which operator

               or_next_op: if( log_expr( next_op_idx: next_op_idx + len( eqv_str) - 1) == eqv_str )then

                  next_op = log_expr( next_op_idx: next_op_idx + len( eqv_str) - 1)

                  next_char = next_op_idx + len( eqv_str)            ! point to next primary

               elseif( log_expr( next_op_idx: next_op_idx + len( neqv_str) - 1) == neqv_str )then or_next_op

                  next_op = log_expr( next_op_idx: next_op_idx + len( neqv_str) - 1)

                  next_char = next_op_idx + len( neqv_str)           ! point to next primary

               elseif( log_expr( next_op_idx: next_op_idx + len( or_str) - 1) == or_str )then or_next_op

                  next_op = log_expr( next_op_idx: next_op_idx + len( or_str) - 1)

                  next_char = next_op_idx + len( or_str)             ! point to next primary

               elseif( log_expr( next_op_idx: next_op_idx + len( and_str) - 1) == and_str )then or_next_op

                  next_op = log_expr( next_op_idx: next_op_idx + len( and_str) - 1)

                  next_char = next_op_idx + len( and_str)            ! point to next primary

               else or_next_op                                       ! unknown operator

                  call msg_quit( "unknown logical operator: " // trim( log_expr( next_op_idx: ) ) )

               endif or_next_op

            else or_end                                              ! operator within string

               next_op = blank                                       ! no operators left

               next_char = next_op_idx                               ! past end of expression

            endif or_end                                             ! operator within string

         case default or_next                                        ! or

            r_or = .true.                                            ! set up for and

         end select or_next                                          ! or or and

! ----------------------------------------------------------------------

!  initialize for a set of or ops

         l_and = r_or                                                ! or-end is now and-end

! ----------------------------------------------------------------------

!  process a set of and ops

         and_ops: do while( next_op == and_str)                      ! process and operations

!  find a primary

            call eval_log_primary( log_expr( next_char: ), primary_len, r_and)

            next_op_idx = next_char + primary_len                    ! point to next operator

!  find next operator or end of expression

            and_end: if( next_op_idx <= expr_len )then               ! if still within expression

!  decode which operator

               and_next_op: if( log_expr( next_op_idx: next_op_idx + len( eqv_str) - 1) == eqv_str )then

                  next_op = log_expr( next_op_idx: next_op_idx + len( eqv_str) - 1)

                  next_char = next_op_idx + len( eqv_str)            ! point to next primary

               elseif( log_expr( next_op_idx: next_op_idx + len( neqv_str) - 1) == neqv_str )then and_next_op

                  next_op = log_expr( next_op_idx: next_op_idx + len( neqv_str) - 1)

                  next_char = next_op_idx + len( neqv_str)           ! point to next primary

               elseif( log_expr( next_op_idx: next_op_idx + len( and_str) - 1) == and_str )then and_next_op

                  next_op = log_expr( next_op_idx: next_op_idx + len( and_str) - 1)

                  next_char = next_op_idx + len( and_str)            ! point to next primary

               elseif( log_expr( next_op_idx: next_op_idx + len( or_str) - 1) == or_str )then and_next_op

                  next_op = log_expr( next_op_idx: next_op_idx + len( or_str) - 1)

                  next_char = next_op_idx + len( or_str)             ! point to next primary

               else and_next_op                                      ! unknown operator

                  call msg_quit( "unknown logical operator: " // trim( log_expr( next_op_idx: ) ) )

               endif and_next_op

            else and_end                                             ! operator within string

               next_op = blank                                       ! no operators left

               next_char = next_op_idx                               ! past end of expression

            endif and_end                                            ! operator within string

!  do the pending and op

            l_and = l_and .and. r_and                                ! compute product

         enddo and_ops                                               ! process eqv operations

!  product is the right operand

         r_or = l_and                                                ! result is a eqv-end

! ----------------------------------------------------------------------

!  do the pending or op

         this_or: select case( do_or)                                ! set above

         case( .true.) this_or                                       ! compute or

            l_or = l_or .or. r_or                                    ! compute sum

         case default this_or                                        ! compute eqv

            l_or = r_or                                              ! use or result

         end select this_or                                          ! compute or

      enddo or_ops                                                   ! process or operations

!  product is the right operand

      r_eqv = l_or                                                   ! result is an eqv-end

! ----------------------------------------------------------------------

!  do the pending eqv op

      eqv_neqv: select case( eqv_op)                                 ! equivalence operators

      case( blank, eqv_str) eqv_neqv                                 ! equivalence

         l_eqv = l_eqv .eqv. r_eqv                                   ! compute equivalence

      case( neqv_str) eqv_neqv                                       ! not equivalence

         l_eqv = l_eqv .neqv. r_eqv                                  ! compute difference

      end select eqv_neqv                                            ! equivalence operators

      eqv_op = next_op                                               ! the next operator

   enddo eqv_ops                                                     ! process expression

! ----------------------------------------------------------------------

   value = l_eqv                                                     ! return value computed

! ----------------------------------------------------------------------

!  eval_log_expr() exit

return                                                               ! eval_log_expr()

! **********************************************************************

!  eval_log_expr()

end subroutine eval_log_expr

! **********************************************************************
! **********************************************************************

!  eval_rel_expr() a relational expression is evaluated as a logical

subroutine eval_rel_expr( rel_expr, value)

! **********************************************************************

!  eval_rel_expr() interface

! ----------------------------------------------------------------------

!  the relational expression ot be evaluated

character( len= *), intent( in) :: rel_expr

!  the value of the relational expression

logical, intent( out) :: value                                      

! **********************************************************************

!  entry: expression is blank_compress_lower_case relational expression

!  exit: value is expression value

! **********************************************************************

!  eval_rel_expr() local

! ----------------------------------------------------------------------

!  index of symbol entry

   integer :: dot_idx                                                ! find dot

   integer :: eq_idx, ne_idx, gt_idx, ge_idx, le_idx, lt_idx         ! point to op

   integer :: l_val, r_val                                           ! left and right values

   character( len= buffer_len) :: expr_str

! **********************************************************************

!  eval_rel_expr() text

continue                                                             ! eval_rel_expr()

! ----------------------------------------------------------------------

   dot_idx = index( rel_expr, dot)                                   ! look for dot in string

! ----------------------------------------------------------------------

!  find a dot?

   got_dot: if( dot_idx > 0 )then                                    ! dot or modern

!  seek all operators with dot

      eq_idx = index( rel_expr, dot_eq)                              ! seek eq

      ne_idx = index( rel_expr, dot_ne)                              ! seek ne

      gt_idx = index( rel_expr, dot_gt)                              ! seek gt

      ge_idx = index( rel_expr, dot_ge)                              ! seek ge

      le_idx = index( rel_expr, dot_le)                              ! seek le

      lt_idx = index( rel_expr, dot_lt)                              ! seek lt

! ----------------------------------------------------------------------

!  find one

      dot_rel_op: if( eq_idx > 0 )then                               ! found op

         expr_str = rel_expr( : eq_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integer expression

         expr_str = rel_expr( eq_idx + len( dot_eq): )

         call eval_int_expr( expr_str, r_val)

         value = l_val == r_val                                      ! evaluate logical op

      elseif( ne_idx > 0 )then dot_rel_op                            ! found op

         expr_str = rel_expr( : ne_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integers

         expr_str = rel_expr( ne_idx + len( dot_ne): )

         call eval_int_expr( expr_str, r_val)

         value = l_val /= r_val                                      ! evaluate logical op

      elseif( ge_idx > 0 )then dot_rel_op                            ! found op

         expr_str = rel_expr( : ge_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integer

         expr_str = rel_expr( ge_idx + len( dot_ge): )

         call eval_int_expr( expr_str, r_val)

         value = l_val >= r_val                                      ! evaluate logical op

      elseif( le_idx > 0 )then dot_rel_op                            ! found op

         expr_str = rel_expr( : le_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integer

         expr_str = rel_expr( le_idx + len( dot_le): )

         call eval_int_expr( expr_str, r_val)

         value = l_val <= r_val                                      ! evaluate logical op

      elseif( gt_idx > 0 )then dot_rel_op                            ! found op

         expr_str = rel_expr( : gt_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integer

         expr_str = rel_expr( gt_idx + len( dot_gt): )

         call eval_int_expr( expr_str, r_val)

         value = l_val > r_val                                       ! evaluate logical op

      elseif( lt_idx > 0 )then dot_rel_op                            ! found op

         expr_str = rel_expr( : lt_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integer

         expr_str = rel_expr( lt_idx + len( dot_lt): )

         call eval_int_expr( expr_str, r_val)

         value = l_val < r_val                                       ! evaluate logical op

! ----------------------------------------------------------------------

!  unknown relational operator

      else dot_rel_op                                                ! found op

         call msg_quit( "no relational operator (.eq., .ne., .gt., .ge., .le., .lt.): " // rel_expr)

      endif dot_rel_op                                               ! found op

! ----------------------------------------------------------------------

!  operator without dot

   else got_dot                                                      ! dot or modern

!  seek all comparison ops

      eq_idx = index( rel_expr, ch_eq)                               ! seek ==

      ne_idx = index( rel_expr, ch_ne)                               ! seek /=

      gt_idx = index( rel_expr, ch_gt)                               ! seek >

      ge_idx = index( rel_expr, ch_ge)                               ! seek >=

      le_idx = index( rel_expr, ch_le)                               ! seek <=

      lt_idx = index( rel_expr, ch_lt)                               ! seek <

! ----------------------------------------------------------------------

!  find one

      ch_rel_op: if( eq_idx > 0 )then                                ! found op

         expr_str = rel_expr( : eq_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integer

         expr_str = rel_expr( eq_idx + len( ch_eq): )

         call eval_int_expr( expr_str, r_val)

         value = l_val == r_val                                      ! evaluate logical op

      elseif( ne_idx > 0 )then ch_rel_op                             ! found op

         expr_str = rel_expr( : ne_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integer

         expr_str = rel_expr( ne_idx + len( ch_ne): )

         call eval_int_expr( expr_str, r_val)

         value = l_val /= r_val                                      ! evaluate logical op

      elseif( ge_idx > 0 )then ch_rel_op                             ! found op

         expr_str = rel_expr( : ge_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integer

         expr_str = rel_expr( ge_idx + len( ch_ge): )

         call eval_int_expr( expr_str, r_val)

         value = l_val >= r_val                                      ! evaluate logical op

      elseif( le_idx > 0 )then ch_rel_op                             ! found op

         expr_str = rel_expr( : le_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integer

         expr_str = rel_expr( le_idx + len( ch_le): )

         call eval_int_expr( expr_str, r_val)

         value = l_val <= r_val                                      ! evaluate logical op

      elseif( gt_idx > 0 )then ch_rel_op                             ! found op

         expr_str = rel_expr( : gt_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integer

         expr_str = rel_expr( gt_idx + len( ch_gt): )

         call eval_int_expr( expr_str, r_val)

         value = l_val > r_val                                       ! evaluate logical op

      elseif( lt_idx > 0 )then ch_rel_op                             ! found op

         expr_str = rel_expr( : lt_idx - 1)

         call eval_int_expr( expr_str, l_val)                        ! evaluate integer

         expr_str = rel_expr( lt_idx + len( ch_lt): )

         call eval_int_expr( expr_str, r_val)

         value = l_val < r_val                                       ! evaluate logical op

! ----------------------------------------------------------------------

!  unknown relational operator

      else ch_rel_op                                                 ! found op

         call msg_quit( "no relational operator (==, /=, >, >=, <=, <): " // rel_expr)

      endif ch_rel_op                                                ! found op

   endif got_dot                                                     ! dot or modern

! ----------------------------------------------------------------------

!  eval_rel_expr() exit

return                                                               ! eval_rel_expr()

! **********************************************************************

!  eval_rel_expr()

end subroutine eval_rel_expr

! **********************************************************************
! **********************************************************************

!  seek_log_primary() a relational expression is evaluated as a logical

subroutine seek_log_primary( log_expr, op_idx, rel_op_idx)

! **********************************************************************

!  seek_log_primary() interface

! ----------------------------------------------------------------------

!  the logical primary to be evaluated

character( len= *), intent( in) :: log_expr

!  the index of the next operator or end of line after the primary

integer, intent( out) :: op_idx

!  the index of the next relational operator or zero

integer, intent( out) :: rel_op_idx                               

! **********************************************************************

!  entry: find log op before first (if any) open paren or after matching

!  exit: length to first log op

! **********************************************************************

!  seek_log_primary() local

integer :: paren_level                                               ! count parenthesis level

! **********************************************************************

!  seek_log_primary() text

continue

!  initialize while loop parameters

   op_idx = 1                                                        ! first character

   paren_level = 0                                                   ! outside any parenthesis

   rel_op_idx = 0                                                    ! none found yet

! ----------------------------------------------------------------------

!  scan through expression

   scan_stmt: do while( op_idx <=  len_trim( log_expr))              ! scan all characters

!  check each character

      which_char: select case( log_expr( op_idx: op_idx))            ! each character of log_expr

!  need to track parenthesis level

      case( open_paren) which_char                                   ! found (

         paren_level = paren_level + 1                               ! ( + 1

         op_idx = op_idx + len( open_paren)

         cycle scan_stmt

      case( close_paren) which_char                                  ! found )

         paren_level = paren_level - 1                               ! ) - 1

         op_idx = op_idx + len( close_paren)

         cycle scan_stmt

      case( dot) which_char                                          ! found .

         log_op_at_level_zero: if( paren_level == 0 )then            ! consider operators outside parenthesis

!  find logical operator

            find_log_op: if( log_expr( op_idx: op_idx + len( or_str) - 1) == or_str )then

               exit scan_stmt                                        ! exit at .or. operator

            elseif( log_expr( op_idx: op_idx + len( and_str) - 1) == and_str )then find_log_op

               exit scan_stmt                                        ! exit at .and. operator

            elseif( log_expr( op_idx: op_idx + len( eqv_str) - 1) == eqv_str )then find_log_op

               exit scan_stmt                                        ! exit at .eqv. operator

            elseif( log_expr( op_idx: op_idx + len( neqv_str) - 1) == neqv_str )then find_log_op

               exit scan_stmt                                        ! exit at .neqv. operator

            endif find_log_op                                        ! found logical operator

         endif log_op_at_level_zero                                  ! consider operators outside parenthesis

      end select which_char

!  check for relational operator (which diagnoses a relational expression)

      rel_op_at_level_zero: if( paren_level == 0 )then               ! check operators in this primary only

         found_rel_op: if( log_expr( op_idx: op_idx + len( dot_eq) - 1) == dot_eq &
                      .or. log_expr( op_idx: op_idx + len( dot_ne) - 1) == dot_ne &
                      .or. log_expr( op_idx: op_idx + len( dot_lt) - 1) == dot_lt &
                      .or. log_expr( op_idx: op_idx + len( dot_le) - 1) == dot_le &
                      .or. log_expr( op_idx: op_idx + len( dot_ge) - 1) == dot_ge &
                      .or. log_expr( op_idx: op_idx + len( dot_gt) - 1) == dot_gt &
                      .or. log_expr( op_idx: op_idx + len( ch_eq) - 1) == ch_eq &
                      .or. log_expr( op_idx: op_idx + len( ch_ne) - 1) == ch_ne &
                      .or. log_expr( op_idx: op_idx + len( ch_lt) - 1) == ch_lt &
                      .or. log_expr( op_idx: op_idx + len( ch_le) - 1) == ch_le &
                      .or. log_expr( op_idx: op_idx + len( ch_ge) - 1) == ch_ge &
                      .or. log_expr( op_idx: op_idx + len( ch_gt) - 1) == ch_gt )then

            rel_op_idx = op_idx

         endif found_rel_op

      endif rel_op_at_level_zero                                     ! consider operators outside parenthesis

!  catch unbalanced parenthesis in logical expression

      unbalanced_parens: if( paren_level < 0 )then

         call msg_quit( "unbalanced parenthesis in expression: " // trim( log_expr) )

      endif unbalanced_parens

!  scan next character

      op_idx = op_idx + 1

   enddo scan_stmt

!  point to last character in primary

   op_idx = op_idx - 1

! ----------------------------------------------------------------------

!  seek_log_primary() exit

return                                                               ! seek_log_primary()

! **********************************************************************

!  seek_log_primary()

end subroutine seek_log_primary

! **********************************************************************
! **********************************************************************

!  eval_int_primary() decode a string to get an integer value

recursive subroutine eval_int_primary( primary_str, primary_len, value)

! **********************************************************************

!  eval_int_primary() interface

! ----------------------------------------------------------------------

!  the integer primary to be evaluated

character( len= *), intent( in) :: primary_str

!  the length of the inetger primary

integer, intent( out) :: primary_len

!  the value of the primary

integer, intent( out) :: value

! **********************************************************************

!  entry: primary_str is a string containing a literal integer

!  exit: primary_len is the length decoded, value is integer value

! **********************************************************************

!  eval_int_primary() local

! ----------------------------------------------------------------------

!  decode integer literal via internal read

   integer :: conversion_iostat                                      ! internal read iostat

!  process sign separately

   integer :: isign                                                  ! sign of primary

!  pointers to characters

   integer :: next_char                                              ! next character to be processed

   integer :: char_idx                                               ! search for character

   integer :: match_paren                                            ! close paren

!  decode digit strings

   character( len= conversion_len) :: conversion_str

!  string containing expressions

   character( len= buffer_len) :: expr_str

! **********************************************************************

!  eval_int_primary() text

continue                                                             ! eval_int_primary()

! ----------------------------------------------------------------------

!  decode unary operator

   next_char = 1                                                     ! start at beginning of literal

!  evaluate the primary using the expression string

   expr_str = primary_str                                            ! searches may go past end of primary

! ----------------------------------------------------------------------

!  test first character is minus

   process_sign: select case( expr_str( next_char: next_char) )      ! check leading sign

! ----------------------------------------------------------------------

   case( minus) process_sign                                         ! check leading sign

      next_char = next_char + len( minus)                            ! skip minus

      primary_len = len( minus)                                      ! processed so far

      isign = -1                                                     ! make value negative

! ----------------------------------------------------------------------

!  test first character is plus

   case( plus) process_sign                                          ! check leading sign

      next_char = next_char + len( plus)                             ! skip plus

      primary_len = len( plus)                                       ! processed so far

      isign = 1                                                      ! make value positive

! ----------------------------------------------------------------------

!  test first character is neither plus nor minus

   case default process_sign                                         ! check leading sign

      primary_len = 0                                                ! processed so far

      isign = 1                                                      ! make value positive

   end select process_sign                                           ! check leading sign

! ----------------------------------------------------------------------

!  find the value of a variable, a literal, or a parenthesized primary_str

   get_value: select case( expr_str( next_char: next_char) )         ! which kind of primary

! ----------------------------------------------------------------------

!  get the value from the variable

   case( 'a': 'z') get_value                                         ! a variable name

!  seek the value of the symbol name

      char_idx = verify( expr_str( next_char: ) // blank, alphanum_chars) + next_char - 2

      call get_integer_value( expr_str( next_char: char_idx), value)

!  processed the alphanumeric characters

      primary_len = primary_len + char_idx                           ! length of string decoded

! ----------------------------------------------------------------------

!  get the value of a literal

   case( '0': '9') get_value                                         ! starts a literal

!  find the first character which is not a digit

      char_idx = verify( expr_str( next_char: ) // blank, digit_chars) + next_char - 2

!  decode digits

      conversion_str = expr_str( next_char: char_idx)                ! extract digit sequence

      conversion_str = adjustr( conversion_str)                      ! right justify in string

      read( unit= conversion_str, fmt= conversion_fmt, iostat= conversion_iostat) value

!  check read error

      decode: if( conversion_iostat > 0 )then                        ! decode failed

         call msg_quit( "can't decode: " // primary_str)

      endif decode                                                   ! decode failed

!  processed the digit string

      primary_len = primary_len + char_idx                           ! length of string decoded

! ----------------------------------------------------------------------

!  get the value of an primary_str

   case( open_paren) get_value                                       ! ( primary_str )

      call seek_close_paren( expr_str, next_char, match_paren)       ! matching paren

      found_match: if(  match_paren <= len_trim( primary_str) )then  ! within string

!  go evaluate the nested expression

         expr_str = primary_str( next_char + 1: match_paren - 1)

         call eval_int_expr( expr_str, value)

!  unmatched parenthesis so complain and quit

      else found_match                                               ! within string

         call msg_quit( "unmatched parenthesis: " // trim( primary_str))

      endif found_match                                              ! within string

!  processed up to the closing parenthesis

      primary_len = match_paren                                      ! length of string decoded

! ----------------------------------------------------------------------

!  error: cannot get the value

   case default get_value                                            ! which kind of primary

      call msg_quit( "bad integer expression: " // trim( primary_str) )

   end select get_value                                              ! which kind of primary

! ----------------------------------------------------------------------

!  apply sign

   value = value * isign                                             ! effects of the sign

! ----------------------------------------------------------------------

!  eval_int_primary() exit

return                                                               ! eval_int_primary()

! **********************************************************************

!  eval_int_primary()

end subroutine eval_int_primary                                    

! **********************************************************************
! **********************************************************************

!  eval_log_primary() decode a string to get an logical value

recursive subroutine eval_log_primary( primary_str, primary_len, value)

! **********************************************************************

!  eval_log_primary() interface

! ----------------------------------------------------------------------

!  the logical primary to be evaluated

character( len= *), intent( in) :: primary_str

!  the length of the logical primary

integer, intent( out) :: primary_len

!  the value of the logical primary

logical, intent( out) :: value                                       

! **********************************************************************

!  entry: primary_str is a string containing a literal logical

!  exit: value is logical value

! **********************************************************************

!  eval_log_primary() local

! ----------------------------------------------------------------------

!  logical "sign"

   logical :: lsign                                                  ! reverse true/false

   integer :: rel_op_idx                                             ! points to a relational operator or zero

!  next character to be decoded

   integer :: next_char                                              ! points to character

   integer :: match_paren                                            ! close paren

   character( len= buffer_len) :: expr_str                           ! pass expression to eval_rel_expr()

! **********************************************************************

!  eval_log_primary() text

continue                                                             ! eval_log_primary()

! ----------------------------------------------------------------------

!  find length of primary and whether it is a relational expression

   call seek_log_primary( primary_str, primary_len, rel_op_idx)

!  decode unary operator

   next_char = 1                                                     ! start with first character

! ----------------------------------------------------------------------

!  expression too short to contain a .not.

   process_sign: if( primary_len <= len( not_str) )then              ! check leading not

      lsign = .true.                                                 ! use value as is

!  expression has a .not.

   elseif( primary_str( next_char: len( not_str)) == not_str )then process_sign

      next_char = next_char + len( not_str)                          ! processed characters

      lsign = .false.                                                ! change "sign" of value

!  no .not.

   else process_sign                                                 ! check leading not

      lsign = .true.                                                 ! use value as is

   endif process_sign                                                ! check leading not

! ----------------------------------------------------------------------

!  a logical primary is either a logical expression or a relational expression

   log_or_rel: if( rel_op_idx == 0 )then

! ----------------------------------------------------------------------

!  find the value of a variable, a literal, or a parenthesized expression

      get_value: select case( primary_str( next_char: next_char) )   ! kind of primary

! ----------------------------------------------------------------------

!  get the value from the variable

      case( 'a': 'z') get_value                                      ! a variable name

!  check whether it's a logical name or error

         call get_logical_value( primary_str( next_char: primary_len), value)

! ----------------------------------------------------------------------

!  get the value of a literal

      case( dot) get_value                                           ! starts a literal

!  decode literal value

         literal_value: if( primary_str( next_char: next_char + len( true_str) - 1) == true_str )then

!  found a .true. string

            value = .true.                                           ! value of .true.

         elseif( primary_str( next_char: next_char + len( false_str) - 1) == false_str )then literal_value

!  found a .false. string

            value = .false.                                          ! value of .false.

!  complain and quit

         else literal_value                                          ! only two literals

            call msg_quit( "bad logical literal: " // trim( primary_str) )

         endif literal_value                                         ! only two literals

! ----------------------------------------------------------------------

!  get the value of an expression

      case( open_paren) get_value                                    ! ( expression )

!  seek the closing parenthesis

         call seek_close_paren( primary_str, next_char, match_paren) ! find match paren

!  if found, determine whether it is a logical or (part of a) relational expression

         found_match: if( match_paren <= len_trim( primary_str) )then

!  evaluate the logical expression within parenthesis

            expr_str = primary_str( next_char + 1: match_paren - 1)

            call eval_log_expr( expr_str, value)

!  unmatched parenthesis so complain and quit

         else found_match                                            ! found match

            call msg_quit( "unmatched parenthesis: " // trim( primary_str))

         endif found_match                                           ! found match

! ----------------------------------------------------------------------

!  error: can't decode logical value

      case default                                                   ! kind of primary

         call msg_quit( "bad logical primary: " // trim( primary_str))

      end select get_value                                           ! kind of primary

! ----------------------------------------------------------------------

!  evaluate the relational expression

   else log_or_rel                                                   ! kind of primary

         call eval_rel_expr( primary_str( next_char: primary_len), value)

   endif log_or_rel                                                  ! kind of primary

! ----------------------------------------------------------------------

!  apply sign

   value = value .eqv. lsign                                         ! effect of not

! ----------------------------------------------------------------------

!  eval_log_primary() exit

return                                                               ! eval_log_primary()

! **********************************************************************

!  eval_log_primary()

end subroutine eval_log_primary

! **********************************************************************
! **********************************************************************

!  %%% string utilities- editing, parenthesis and quotes

! **********************************************************************
! **********************************************************************

!  replace_substring() edit source lines

subroutine replace_substring( mixed_case_str, lower_case_str, search_str, replace_str, first_idx)

! **********************************************************************

!  replace_substring() interface

! ----------------------------------------------------------------------

!  mixed case string to be printed

character( len= *), intent( inout), optional :: mixed_case_str

!  lower case string to be searched

character( len= *), intent( inout) :: lower_case_str

!  substring to be replaced

character( len= *), intent( in) :: search_str                        

!  string to replace target

character( len= *), intent( in) :: replace_str                       

!  location of first occurance

integer, intent( in) :: first_idx

! **********************************************************************

!  entry: line is a line of Fortran source with (possibly) ?target?

!  exit: line has any ?target? strings replaced

! **********************************************************************

!  replace_substring() local

! ----------------------------------------------------------------------

!  beginning and end of target within lines

   integer :: end_idx

   integer :: search_idx

   integer :: search_len

! **********************************************************************

!  replace_substring() text

! ----------------------------------------------------------------------

continue                                                             ! replace_substring()

! ----------------------------------------------------------------------

!  initialize

   search_idx = first_idx                                            ! target may change

   search_len = len( search_str)                                     ! length of search string

!  if mixed case is present

   mixed_present: if( present( mixed_case_str) )then                 ! edit two strings or one

!  replace in both strings

      edit_mixed: do while( search_idx > 0 )                         ! if found search_str

         end_idx = search_idx + search_len                           ! end of replacement

         mixed_case_str = mixed_case_str( : search_idx - 1) &        ! original before target
                          // replace_str &                           ! replace target
                          // mixed_case_str( end_idx: )              ! original after target

         lower_case_str = lower_case_str( : search_idx - 1) &        ! lower case original before target
                          // replace_str &                           ! replace target
                          // lower_case_str( end_idx: )              ! lower case original after target

         search_idx = index( lower_case_str, search_str)

      enddo edit_mixed                                               ! if found search_str

!  mixed case is not present

   else mixed_present                                                ! edit two strings or one

!  replace in lower case only   

      edit_string: do while( search_idx > 0 )                        ! if found search_str

         end_idx = search_idx + search_len                           ! end of replacement

         lower_case_str = lower_case_str( : search_idx - 1) &        ! lower case original before target
                          // replace_str &                           ! replace target
                          // lower_case_str( end_idx: )              ! lower case original after target

         search_idx = index( lower_case_str, search_str)

      enddo edit_string                                              ! if found search_str

   endif mixed_present                                               ! edit two strings or one

! ----------------------------------------------------------------------

!  replace_substring() exit

return                                                               ! replace_substring()

! **********************************************************************

!  replace_substring()

end subroutine replace_substring

! **********************************************************************
! **********************************************************************

!  seek_close_paren() true if successfully found matching ()

subroutine seek_close_paren( string, start, match)

! **********************************************************************

!  seek_close_paren() interface

! ----------------------------------------------------------------------

!  the string starting with open parenthesis

character( len= *), intent( in) :: string

!  the index of the open parenthesis

integer, intent( in) :: start

!  the index of the matching close parenthesis

integer, intent( out) :: match

! **********************************************************************

!  seek_close_paren() constants

! ----------------------------------------------------------------------

!  close parenthesis

   character( len= *), parameter :: close_paren = ')'                ! ends an expression

! **********************************************************************

!  seek_close_paren() local

! ----------------------------------------------------------------------

!  counters and pointers

   integer :: level                                                  ! level of parenthesis encountered

   integer :: string_len                                             ! either end of string

! **********************************************************************

!  seek_close_paren() text

continue                                                             ! seek_close_paren()

! ----------------------------------------------------------------------

!  initialize

   string_len = len_trim( string)                                    ! to end

! ----------------------------------------------------------------------

   level = 0                                                         ! count nesting level

   search: do match = start + 1, string_len                          ! look thru string

      levels: select case( string( match: match) )                   ! count nesting

      case( open_paren) levels                                       ! nested

         level = level + 1                                           ! increment counter

      case( close_paren) levels                                      ! end of nested pair

         eureka: if( level == 0 )then                                ! match?

            exit search                                              ! quit

         endif eureka                                                ! match?

         level = level - 1                                           ! decrement counter

      end select levels                                              ! count nesting

   enddo search                                                      ! look thru string

! ----------------------------------------------------------------------

!  seek_close_paren() exit

return                                                               ! seek_close_paren()

! **********************************************************************

!  seek_close_paren()

end subroutine seek_close_paren

! **********************************************************************
! **********************************************************************

!  unquote_string() true if extracts string from between quotes

subroutine unquote_string( quoted_str, unquoted_str, in_len, out_len)

! **********************************************************************

!  unquote_string() interface

! ----------------------------------------------------------------------

!  the quoted string to be unquoted

character( len= *), intent( in) :: quoted_str

!  the unquoted string

character( len= *), intent( out) :: unquoted_str

!  the length of the quoted string

integer, intent( out) :: in_len

!  the length of the unquoted string

integer, intent( out) :: out_len

! **********************************************************************

!  unquote_string() local

! ----------------------------------------------------------------------

!  which quote is to be used

   character( len= 1) :: quote                                       ! the starting quote

! **********************************************************************

!  unquote_string() text

continue                                                             ! unquote_string()

! ----------------------------------------------------------------------

!  which quote is the first quote (if either)

   which_quote: select case( quoted_str( 1: 1) )                     ! which quote

! ----------------------------------------------------------------------

!  string delimited by single quote

   case( single_quote) which_quote                                   ! use single quote

      quote = single_quote                                           ! quote is single quote

! ----------------------------------------------------------------------

!  string delimited by double quote

   case( double_quote) which_quote                                   ! use double quote

      quote = double_quote                                           ! quote is double quote

! ----------------------------------------------------------------------

!  string delimited by neither quote- nothing to do

   case default which_quote                                          ! neither

      in_len = 0                                                     ! no quote

      out_len = 0                                                    ! no unquoted string

      unquoted_str = null_string                                     ! null output

      return                                                         ! quit

   end select which_quote

! ----------------------------------------------------------------------

!  initialize scan loop

   in_len = 2                                                        ! start past quote in quoted string
   out_len = 1                                                       ! start at beginning of unquoted string

   unquoted_str = blank                                              ! initialize output

!  scan thru the quoted string

   scan_string: do while( in_len <= len_trim( quoted_str) )          ! loop thru quoted string

! ----------------------------------------------------------------------

!  if find one matching quote

      next_char: if( quoted_str( in_len: in_len) == quote )then      ! find next quote

!  check next character

         in_len = in_len + 1                                         ! next character in quoted string

!  check for a pair of quotes

         next_quote: if( quoted_str( in_len: in_len) == quote )then

            unquoted_str( out_len: out_len) = quoted_str( in_len: in_len)

            in_len = in_len + 1                                      ! next character in quoted

            out_len = out_len + 1                                    ! next character in unquoted

         else next_quote                                             ! find two adjacent quotes

            exit scan_string                                         ! quit

         endif next_quote                                            ! find two adjacent quotes

! ----------------------------------------------------------------------

!  character is not a matching quote

      else next_char                                                 ! find a quote

         unquoted_str( out_len: out_len) = quoted_str( in_len: in_len)

         in_len = in_len + 1                                         ! next character in quoted

         out_len = out_len + 1                                       ! next character in unquoted

      endif next_char                                                ! find a quote

   enddo scan_string                                                 ! loop thru quoted string

! ----------------------------------------------------------------------

!  unquote_string() exit

return                                                               ! unquote_string()

! **********************************************************************

!  unquote_string()

end subroutine unquote_string

! **********************************************************************
! **********************************************************************

!  seek_match_quote() true if extracts string from between quotes

subroutine seek_match_quote( quoted_str, start_char, in_len)

! **********************************************************************

!  seek_match_quote() interface

! ----------------------------------------------------------------------

!  the quoted string to be unquoted

character( len= *), intent( in) :: quoted_str

!  the length of the quoted string

integer, intent( in) :: start_char

!  the length of the quoted string

integer, intent( out) :: in_len

! **********************************************************************

!  seek_match_quote() local

! ----------------------------------------------------------------------

!  which quote is to be used

   character( len= 1) :: quote                                       ! the starting quote

! **********************************************************************

!  seek_match_quote() text

continue                                                             ! seek_match_quote()

! ----------------------------------------------------------------------

!  which quote is the first quote (if either)

   which_quote: select case( quoted_str( start_char: start_char) )   ! which quote

! ----------------------------------------------------------------------

!  string delimited by single quote

   case( single_quote) which_quote                                   ! use single quote

      quote = single_quote                                           ! quote is single quote

! ----------------------------------------------------------------------

!  string delimited by double quote

   case( double_quote) which_quote                                   ! use double quote

      quote = double_quote                                           ! quote is double quote

! ----------------------------------------------------------------------

!  string delimited by neither quote- nothing to do

   case default which_quote                                          ! neither

      in_len = 0                                                     ! no quote

      return                                                         ! quit

   end select which_quote

! ----------------------------------------------------------------------

!  initialize scan loop

   in_len = start_char + 1                                           ! start past quote in quoted string

!  scan thru the quoted string

   scan_string: do while( in_len <= len_trim( quoted_str) )          ! loop thru quoted string

! ----------------------------------------------------------------------

!  if find one matching quote

      next_char: if( quoted_str( in_len: in_len) == quote )then      ! find next quote

!  check next character

         in_len = in_len + 1                                         ! next character in quoted string

!  check for a pair of quotes

         next_quote: if( quoted_str( in_len: in_len) == quote )then

            in_len = in_len + 1                                      ! next character in quoted

         else next_quote                                             ! find two adjacent quotes

            exit scan_string                                         ! quit

         endif next_quote                                            ! find two adjacent quotes

! ----------------------------------------------------------------------

!  character is not a matching quote

      else next_char                                                 ! find a quote

         in_len = in_len + 1                                         ! next character in quoted

      endif next_char                                                ! find a quote

   enddo scan_string                                                 ! loop thru quoted string

! ----------------------------------------------------------------------

!  seek_match_quote() exit

return                                                               ! seek_match_quote()

! **********************************************************************

!  seek_match_quote()

end subroutine seek_match_quote

! **********************************************************************
! **********************************************************************

!  coco

! $Id: coco.f90 1.28 2003/10/22 13:08:20Z Dan Release $
! **********************************************************************

end program coco                                                     ! eof
