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

#! /bin/sh
#
# This does all the builds needed for a new arith package.
#

#
# Check for -b (build only, don't test) and -n (don't "make delete")
# switches.
#
unset build_only
set   do_make_delete

if ( $#argv >= 1 ) then
  while ( "${1}" =~ -? )
    if ( "${1}" == "-b" ) then
      set build_only
    else if ( "${1}" == "-n" ) then
      unset do_make_delete
    else
      echo "usage: $0 [-b] [-n] [build_dir [arch1 [arch2 ...]]]"
      exit 1
    endif

    shift
    if ( $#argv == 0 ) then
      break
    endif
  end
endif

#
# Get build directory.
#
if ( $#argv == 0 ) then
  set bdir = ${cwd}
else
  set bdir = ${1}
  if ( ! -d ${bdir} ) then
    echo "usage: $0 [-b] [build_dir [arch1 [arch2 ...]]]"
    echo "       '${bdir}' is not a directory."
    exit 1
  else
    shift
  endif
endif

#
# Figure out which architectures we need to build (default is all).
#
set is_cray_arch    = ( T    \
                        T    \
                        T    \
                        T    \
                        T    \
                        F    \
                        F    \
                      )

set arch_names      = ( ymp        \
                        c90        \
                        ts-ieee    \
                        t3d        \
                        t3e        \
                        solaris    \
                        irix       \
                      )

set arch_ids        = ( ymp        \
                        c90        \
                        ts_ieee    \
                        t3d        \
                        t3e        \
                        solaris    \
                        irix       \
                      )

set arch_targets    = ( cray-ymp        \
                        cray-c90        \
                        cray-ts,ieee    \
                        cray-t3d        \
                        cray-t3e        \
                        solaris         \
                        irix            \
                      )

set archs           = ( cray-ymp        \
                        cray-c90        \
                        cray-ts,ieee    \
                        cray-t3d        \
                        cray-t3e        \
                        solaris         \
                        irix            \
                      )

set do_arch         = ( F    \
                        F    \
                        F    \
                        F    \
                        F    \
                        F    \
                        F    \
                      )

set arch_hosts      = ( "rain    wind"            \
                        "rain    wind"            \
                        "voyager rain    wind"    \
                        "rain    wind"            \
                        "rain    wind"            \
                        "olive   sequoia"         \
                        "click   clack"           \
                      )

if ( $#argv == 0 ) then
  set do_arch = ( ` echo ${do_arch} | sed -e 's/F/T/g' ` )
else
  foreach cmd_arch ( $* )
    set aidx = 1
    while ( "${aidx}" <= $#arch_names )
      if ( "${arch_names[$aidx]}" == "${cmd_arch}" ) then
        break
      endif

      @ aidx++
    end

    if ( "${aidx}" > $#arch_names ) then
      echo Error -- '"'${cmd_arch}'"' is not a supported architecture.
      exit 1
    else
      set do_arch[$aidx] = T
    endif
  end
endif

#
# Do the builds.
#
cd ${bdir}

if ( $?do_make_delete ) then
  make delete
endif

@ aidx = 1
while ( "${aidx}" <= $#arch_names )
  if ( "${do_arch[$aidx]}" == "T" ) then

    if ( "${is_cray_arch[$aidx]}" == "T" ) then
      set aname = cray-${arch_names[$aidx]}
    else
      set aname = ${arch_names[$aidx]}
    endif

    echo "Architecture:" ${aname}

    #
    # Find a host to build on.
    #
    set host = ""

    set ah = ( ${arch_hosts[$aidx]} )
    @ ahidx = 1
    while ( "${ahidx}" <= $#ah && "${host}" == "" )
      set arch_host = ${ah[$ahidx]}
      ping ${arch_host} 2 >&/dev/null
      if ( "${status}" == 0 ) then
        set rsh_result = ( ` rsh ${arch_host} uname -a ` )
        echo ${rsh_result} | grep ${arch_host} >&/dev/null
        if ( "${status}" == 0 ) then
          set host = ${arch_host}
        endif
      endif

      @ ahidx++
    end

    if ( "${host}" == "" ) then
      echo "  Couldn't find an appropriate host that is up -- skipped."
    else
      echo "Host:         ${host}"

      set atarg = ${arch_targets[$aidx]}

      if ( $?build_only ||                \
           "${aname}" == "cray-t3d" ||    \
           "${aname}" == "cray-t3e" ||    \
           "${aname}" == "irix" ) then
        set dm  = "make DEBUG=yes TARGET=${atarg} arith.a >&x.d.${aname}"
        set ndm = "make           TARGET=${atarg} arith.a >&x.nd.${aname}"
      else
        set dm  = "make DEBUG=yes TARGET=${atarg}         >&x.d.${aname}"
        set ndm = "make           TARGET=${atarg}         >&x.nd.${aname}"
      endif

      rsh -n ${host}                                    \
        " setenv TARGET ${atarg} ; chdir ${bdir} ; "    \
        " make clobber ; ${dm} ; "                      \
        " make clobber ; ${ndm} "                       \
        >&x.${aname}

    endif

  endif

  @ aidx++
end
