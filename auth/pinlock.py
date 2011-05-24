#!/usr/bin/env python

# Terminal locker relying on a PIN file to avoid PAM/setuid
#
# Copyright (c) 2011, Pierre Carrier <pierre@gcarrier.fr>
# Permission to use, copy, modify, and/or distribute this software
# for any purpose with or without fee is hereby granted, provided that
# the above copyright notice and this permission notice appear in all copies.

# TODO: A SUID version that
# 1) Uses pam, of course
# 2) Is "resilient to its own crashes" thanks to:
# 2.1) A mechanism like 'stty -isig' to disable keyboard control of
#      the underlying processes
# 2.2) A SIGSTOP of the PPID
# 3) Traps a max of signals, including segfaults, and
#    handles all errors no matter what (worst case notify & discard)


from getopt import getopt, GetoptError
from getpass import getpass
from hashlib import sha256
from os import getuid, stat, chmod, ttyname
from os.path import join, isfile, exists
from pwd import getpwnam, getpwuid
from signal import signal, SIG_IGN, SIGINT, SIGTSTP, SIGQUIT
from socket import gethostname
from sys import stdout, stderr, argv, exit
from time import sleep

def ignore_signals():
  """The process will ignore keyboard signals"""
  for sig_number in (SIGINT, SIGTSTP, SIGQUIT):
    signal(sig_number, SIG_IGN)

def get_pin(pw):
  return(getpass('%s@%s locked by %s. PIN? ' % (gethostname(), ttyname(stdout.fileno()), pw.pw_name)))

def pin_path(pw):
  return join(pw.pw_dir, '.pinlock')
  
def read_pinhash(pw):
  pinfilename = pin_path(pw)
  if (not exists(pinfilename)):
    print >> stderr, 'No pin file (%s) found. Let\'s create one!' % pinfilename
    set_pin(pw)
    exit(0)
  if (not isfile(pinfilename)):
    print >> stderr, "%s is not a file" % pw_file
    exit(2)
  if(stat(pinfilename).st_mode & 077):
    print >> stderr, 'You shouldn\'t allow anyone else but you to do anything with %s. please "chmod go= %s"' % (pinfilename, pinfilename)
    exit(3)
  try:
    return(open(pinfilename, 'r').read().strip())
  except:
    print >> stderr, 'Couldn\'t read the pin file, %s' % pinfilename
    exit(4)

def set_pin(pw):
  pin  = getpass('Please enter the desired PIN: ')
  pin_confirm = getpass('Please confirm the desired PIN: ')
  if(pin != pin_confirm):
    print >> stderr, 'The PINs differ!'
    exit(5)
  pinfilename = pin_path(pw)
  try:
    pinfile = open(pinfilename, 'w')
    pinfile.write(sha256(pin).hexdigest())
    pinfile.close()
  except:
    print >> stderr, 'Could not write in %s' % pinfilename
    exit(6)
  try:
    chmod(pinfilename, stat(pinfilename).st_mode & ~077)
  except:
    print >> stderr, 'Could not update the permissions of %s' % pinfilename
  exit(0)

def usage(pw):
  print """Usage: %s [-h] [-i] [-c]
  -h for this help
  -i for in-place input (no fullscreen)
  -c to change the password
The PIN in hashed with SHA256 and stored in %s""" % (argv[0], pin_path(pw))

def main():
  pw = getpwuid(getuid())
  fullscreen = True
  try:
    opts, args = getopt(argv[1:], 'chi')
  except GetoptError, err:
    print >> stderr, str(err)
    usage(pw)
    exit(7)
  for opt, value in opts:
    if opt == '-h':
      usage(pw)
      exit(0)
    elif opt == '-c':
      set_pin(pw)
    elif opt == '-i':
      fullscreen = False

  ignore_signals()
  password = None
  pinhash = read_pinhash(pw)

  if fullscreen:
    from curses import setupterm, tigetstr
    setupterm()
    print tigetstr("smcup"),
    stdout.flush()

  password = get_pin(pw)
  sleep_time = 1.0
  while (sha256(password).hexdigest() != pinhash):
    print >> stderr, 'Incorrect password! Please wait for %gs.' % sleep_time
    sleep(sleep_time)
    sleep_time *= 2
    password = get_pin(pw)

  if(fullscreen):
    print tigetstr("rmcup"),
    stdout.flush()

if __name__ == "__main__":
  main()