#!/usr/bin/env python

# Translate user_trace.stp timestamps to seconds, relative to first traced call
# Usage: user_trace_relative_time.py < example.trace > relative.trace

import sys

first_time = False
for line in sys.stdin:
  res = line.split(" ",1)
  time, rest = res[0], res[1]
  time = float(time[0:10] + "." + time[10:])
  if not first_time:
    first_time = time
  print str(time-first_time)+"\t"+rest.rstrip()