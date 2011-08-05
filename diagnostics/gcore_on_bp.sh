#!/bin/sh

#   Copyright (C) 2003, 2005, 2007, 2008, 2009 Free Software Foundation, Inc.
#   Copyright (C) 2010 Red Hat, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

#
# gcore_on_bp.sh
# Script to generate core files on attachment and every breakpoint.
# It starts up gdb, attaches to the given PID and invokes the gcore command.
#
# This will only work as-is on x86 and x86_64. The only arch-dependant part
# is the $rip hack as I don't known a better solution to check the process
# is still running.
#

if [ "$#" -eq "0" ]
then
    echo "usage:  gcore_on_bp [-o filename] breakpoint pid[...]"
    exit 2
fi

# Need to check for -o option, but set default basename to "core".
name=core

if [ "$1" = "-o" ]
then
    if [ "$#" -lt "4" ]
    then
	# Not enough arguments.
	echo "usage:  gcore [-o filename] breakpoint pid[...]"
	exit 2
    fi
    name=$2

    # Shift over to break point
    shift; shift
fi

breakpoint=$1
# Shift over to PID list
shift

# Create a temporary file.  Use mktemp if available, but cope if it is not.
tmpfile=`mktemp ${name}.XXXXXX 2>/dev/null` || {
  tmpfile=${name}.$$
  if test -e $tmpfile; then
    echo "Could not create temporary file $tmpfile"
    exit 1
  fi
  touch $tmpfile
}
trap "rm -f $tmpfile" EXIT

# Loop through pids
for pid in $*
do
	# Write gdb script for pid $pid.  
	cat >>$tmpfile <<EOF
set \$corenb=0
attach $pid
break $breakpoint
while (\$rip > 0)
  gcore $name.$pid.\$corenb
  set \$corenb++
  continue
end
quit
EOF

	# `</dev/null' to avoid touching interactive terminal if it is
	# available but not accessible (SIGTTIN risk)
	gdb -x $tmpfile -batch </dev/null &

done
