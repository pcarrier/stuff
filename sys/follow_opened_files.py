#!/usr/bin/env python
# Track opens and closes in a strace
#
# Copyright (c) 2010, Red Hat Inc.
# Permission to use, copy, modify, and/or distribute this software
# for any purpose with or without fee is hereby granted, provided that
# the above copyright notice and this permission notice appear in all copies.
#
# Maintainer: Pierre Carrier <prc@redhat.com>
#
# TODO:
# - improve performance
# - track dup(), dup2(), F_DUPFD, etc.
# - clean up regexps and matching to improve performance
# - document?

import sys, re
opened = {}
counter = 0
for line in sys.stdin.readlines():
	change = False
	try:
		if "open(" in line and "= -" not in line:
			fd = re.match(r'.*= (-?[0-9]+).*', line).group(1)
			fn = re.match(r'.*"([^"]+)".*', line).group(1)
			opened[fd] = fn
			counter += 1
			change = True
		if "close(" in line and "= -" not in line:
			fd = re.match(r'.*\((.*)\).*', line).group(1)
			print "closing "+str(fd) + ":" + opened[fd]
			del opened[fd]
			counter -= 1
			change = True
	except KeyError:
		print >> sys.stderr, "Trying to remove unknown fd: " + line.strip()
	if change == True:
		print str(counter)+"\t"+line.strip()
		print "\t" + str(opened)
