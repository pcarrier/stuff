#!/usr/bin/env python2.5
# Primitive throttler
#
# Copyright (c) 2011, Pierre Carrier <pierre@gcarrier.fr>
# Permission to use, copy, modify, and/or distribute this software
# for any purpose with or without fee is hereby granted, provided that
# the above copyright notice and this permission notice appear in all copies.
#
# This code was written as an exercice for an interview.
# I don't expect to use it, I don't expect anyone to.

from getopt import getopt, GetoptError
from sys import exit, argv
from time import time, sleep

from sys import stdin, stdout, stderr
from fcntl import fcntl, F_GETFL, F_SETFL
from select import select
from os import read, write, O_NONBLOCK

in_f, out_f = stdin, stdout
in_fd, out_fd = in_f.fileno(), out_f.fileno()

# Time for each iteration under which we enforce the bandwidth
# (seconds, overwritten with -r)
time_resolution = 0.2

# Maximum bandwidth
# (bytes per second, overwritten with -b)
bandwidth = 10.0

# Maximum chunk read at a time
# (bytes, overwritten with -m)
# Lower = more CPU cycles and ctx switching, less latency
max_chunk = 1024

def usage():
    print """Usage: %s [-b bandwidth (bps)] [-r resolution (s)] [-m max_chunk (b)]
    defaults to %g bps, %g ms, %i bytes
    print read the code for more explanations""" % \
      (argv[0], bandwidth, time_resolution, max_chunk)

def blocking(fd, blocking):
  """Makes the file descriptor non-blocking"""
  fl = fcntl(fd, F_GETFL)
  if blocking:
    fl &= ~(O_NONBLOCK)
  else:
    fl |= O_NONBLOCK
  fcntl(fd, F_SETFL, fl)

def loop():
  allowed_per_iter = int(bandwidth * time_resolution)

  iter_start = time()

  while True:
    iter_end = iter_start + time_resolution
    bytes_still_writeable = allowed_per_iter

    while(time() < iter_end and bytes_still_writeable > 0):

      select([in_fd],[],[]) # Passive wait for data/EOF
      chunk = read(in_fd, min(max_chunk, bytes_still_writeable))

      just_read = len(chunk)
      if(just_read == 0): exit(0) # EOF
      bytes_still_writeable -= just_read

      try: written = write(out_fd, chunk)
      except: exit(1) # Broken pipe, out of disk space, etc.

      if(bytes_still_writeable == 0):
        sleep(iter_end - time()) # Wait until the next round

    iter_start = iter_end

if __name__ == "__main__":
  blocking(in_fd, True)  # read often from stdin
  blocking(in_fd, False) # but block on write
  
  try:
    opts, args = getopt(argv[1:], 'hr:b:m:')
  except GetoptError, err:
    print >> stderr, str(err)
    usage()
    exit(2)

  for opt, value in opts:
    try:
      if opt == "-h":
        usage()
        exit(0)
      elif opt == "-b":
        bandwidth = float(value)
      elif opt == "-m":
        max_chunk = int(value)
      elif opt == "-r":
        time_resolution = float(value)
    except ValueError:
      print >> stderr, "Numbers are expected for all attributes!"
      exit(2)

  loop()