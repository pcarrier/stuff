#!/usr/bin/env python

"""
Copyright (c) 2010, Red Hat Inc.
Permission to use, copy, modify, and/or distribute this software
for any purpose with or without fee is hereby granted, provided that
the above copyright notice and this permission notice appear in all copies.

Maintainer: Pierre Carrier <prc@redhat.com>

Thanks to http://wiki.oracle.com/page/Python
& http://code.activestate.com/recipes/307871

Whenever possible, please try this script in non-violent mode, then with SYSRQ_KEY = "h", only finally with "c".
"""

VIOLENT_ARG = "yes-send-a-sysrq-i-am-sure"
TTY_PATH = "/dev/ttyS0"
SYSRQ_KEY = "c"
ORACLE_CONNSTR = 'HR/redhat@10.33.1.60:1521/XE'
ORACLE_QUERY = "select 1+1 from dual"

ICMP_IP = "10.33.1.60"
ICMP_TIMEOUT = 5

DELAY = 1

violent = False

import sys, os, termios, time, subprocess, cx_Oracle

def log(str):
  print time.ctime() + ": " + str

def test_oracle():
  conn = cx_Oracle.Connection(ORACLE_CONNSTR, threaded = True)
  conn.ping()
  curs = conn.cursor()
  curs.arraysize=50
  curs.execute(ORACLE_QUERY)
  print(curs.fetchone())
  conn.close()

def test_icmp():
  return subprocess.call(["/bin/ping", "-c1", "-W"+str(ICMP_TIMEOUT), ICMP_IP])

def send_sysrq(key):
  try:
    fd = os.open(TTY_PATH, os.O_RDWR | os.O_SYNC) # no buffering
  except Exception, e:
    log("Cannot open " + TTY_PATH + ": " + e.__str__())
  else:
    try:
      termios.tcsendbreak(fd, 0)
      os.write(fd, key)
      termios.tcdrain(fd)
    except Exception, e:
      log("We could not send the sysrq! " + e.__str__())
    else:
      log("sysrq sent!")
    os.close(fd)

def server_down():
  if violent:
    send_sysrq(SYSRQ_KEY)
  else:
    log("We would sysrq-c if we were violent!")

if __name__ == '__main__':
  if len(sys.argv) < 2:
    print("Please use parameter within:\n - " + VIOLENT_ARG + " to send a sysrq on timeouts\n - test-sysrq to send a sysrq-t\n - anything else for simulation")
    sys.exit(0)
  elif sys.argv[1] == "test-sysrq":
    send_sysrq("h")
    sys.exit(0)
  elif sys.argv[1] == VIOLENT_ARG:
    print("We will send a sysrq-c on timeouts")
    violent = True
  if not violent:
    print("Running in friendly mode.")
  while True:
    try:
      test_oracle()
    except Exception, e:
      log("Oops! Oracle does not answer properly! " + e.__str__())
      try:
        if test_icmp() != 0:
          log("ping failed with "+icmp_result)
          raise Exception()
      except Exception, e:
        log("Oops! ping does not answer properly either!! " + e.__str__())
        server_down()
      else:
        log("Saved by the ping")
    else:
      log("Oracle test successful")
    time.sleep(DELAY)
