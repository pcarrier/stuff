#!/usr/bin/env python

"""
Bash your script in a bash 'oneliner'

Usage:
$ eval $(bashin example.py foo bar)
will call example.py with the parameters foo and bar
The output of bashin can be used without the presence of example.py

Example:
Useful to run a local script through pssh, eg:
$ pssh -h list.txt -o outputs/ -t 0 "eval $(bashin myscript.py f\" o\" o b\'a\'r)"
"""

from re import match
from sys import argv, stderr, exit

try:
  from cStringIO import StringIO
except:
  from StringIO import StringIO

def str_to_printf(str):
    """
    Turn into a printf-able
    """
    io = StringIO()
    for c in str:
        if(match(r'[a-zA-Z0-9]', c)):
            io.write(c)
        else:
            io.write(r'\x%02x' % ord(c))
    res = io.getvalue()
    io.close()
    return res

def file_to_printf(name):
    return str_to_printf(open(name, 'r').read())

BASH_EXEC_PREFIX = r"""BASHIN_DEST="$(mktemp)"&&trap "rm -f \"$BASHIN_DEST\";exit" INT TERM EXIT&&printf '"""
BASH_EXEC_INTERM = r"""'>>"$BASHIN_DEST"&&chmod +x "$BASHIN_DEST"&&"$BASHIN_DEST" """
BASH_EXEC_SUFFIX = r""";exit $?"""
ESCAPE_PREFIX = "\"$(printf '"
ESCAPE_SUFFIX = "')\""

def wrap_bin(path, args):
    escaped_args = [ ESCAPE_PREFIX + str_to_printf(arg) + ESCAPE_SUFFIX for arg in args]
    return BASH_EXEC_PREFIX + file_to_printf(path) + BASH_EXEC_INTERM + " ".join(escaped_args) + BASH_EXEC_SUFFIX

if __name__ == '__main__':
    if len(argv) < 2:
        print >> stderr, "Arguments: program arguments ..."
        exit(1)
    print(wrap_bin(argv[1], argv[2:]))
