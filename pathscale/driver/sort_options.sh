#!/bin/sh
#
#
#  Copyright (C) 2000, 2001 Silicon Graphics, Inc.  All Rights Reserved.
#
#  Path64 is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3, or (at your option)
#  any later version.
#
#  Path64 is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
#  License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Path64; see the file COPYING.  If not, write to the Free
#  Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
#  02110-1301, USA.
#
#  Special thanks goes to SGI for their continued support to open source
#
#
# $1 == source dir, $2 == OPTIONS file
# $3 == optional cpp defines
# need C locale for sort to work as expected
LANG=C
dir=$1
file=$2
shift
shift
# Mac port: /usr/bin/cpp works on both Fedora and Darwin
cpp -traditional -P "$@" $dir/$file > tmp.options.cpp
awk -f $dir/sort_options.awk tmp.options.cpp
# note that some linux versions of sort are broken,
# so we actually re-sort within table for safety.
sort -r tmp.options > tmp.options.sort
echo "%%% OPTIONS"
cat tmp.options.sort
echo "%%% COMBINATIONS"
cat tmp.options.combo
rm -f tmp.options*
