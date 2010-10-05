#!/usr/bin/env python
# Thanks to http://wiki.oracle.com/page/Python
# and http://code.activestate.com/recipes/307871/

# TODO SERIAL_PORT = '/dev/ttys0'

DELAY=5
VIOLENT_ARG="yes_send_a_sysrqc"

ORACLE_CONNSTR = 'scott/tiger'
ORACLE_TIMEOUT=30
ORACLE_QUERY="SELECT 1 FROM DUAL;"

ICMP_IP="10.1.1.1"
ICMP_TIMEOUT=60

violent=False

import signal, time, cx_Oracle, subprocess

class TimedOutExc(Exception):
  def __init__(self, value = "Timed Out"):
    self.value = value
  def __str__(self):
    return repr(self.value)

def TimedOutFn(f, timeout, *args, **kwargs):
  def handler(signum, frame):
    raise TimedOutExc()

  old = signal.signal(signal.SIGALRM, handler)
  signal.alarm(timeout)
  try:
    result = f(*args, **kwargs)
  finally:
    signal.signal(signal.SIGALRM, old)
    signal.alarm(0)
  return result

def timed_out(timeout):
  def decorate(f):
    def handler(signum, frame):
      raise TimedOutExc()
    def new_f(*args, **kwargs):
      old = signal.signal(signal.SIGALRM, handler)
      signal.alarm(timeout)
      try:
        result = f(*args, **kwargs)
      finally:
        signal.signal(signal.SIGALRM, old)
        signal.alarm(0)
        return result
    new_f.func_name = f.func_name
    return new_f
  return decorate

@timed_out(ORACLE_TIMEOUT)
def test_oracle():
  conn = cx_Oracle.connect(ORACLE_CONNSTR)
  curs = conn.cursor()
  curs.arraysize=50
  curs.execute(ORACLE_QUERY)
  print(curs.description)
  print(curs.fetchone())
  conn.close()

@timed_out(ICMP_TIMEOUT)
def test_icmp():
  call(["ping", "-c1","-W3600",ICMP_IP])

def server_down():
  if violent:
    call(["minicom", "-S", "./sysrqc"])
  else:
    print("We would sysrq-c if we were violent!")

if __name__ == '__main__':
  if len(sys.argv) < 2:
    print("Please use parameter "+VIOLENT_ARG+" to send sysrq-c on timeouts")
  elif sys.argv[1] == VIOLENT_ARG:
    print("We will send a sysrq-c on timeouts")
    VIOLENT=True
  if not violent:
    print("Running in friendly mode.")
  while true:
    try:
      test_oracle()
    except TimedOutExc:
      print("Ooops! Oracle does not answer!")
      try:
        test_icmp()
      except TimedOutExc:
        print("Oooooops! ICMP does not answer either!!")
        server_down()
    time.sleep(DELAY)
