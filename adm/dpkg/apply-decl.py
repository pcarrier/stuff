#!/usr/bin/env python

# apply-decl.py: apply apt definition files
# call with a list of defs on stdin or in files listed as parameters.
#
# Format:
# # Example comment
# -example-to-remove
# +example-to-install

from __future__ import with_statement 

import apt
import fileinput

cache = apt.Cache()

#cache.update()
#cache.open(None)

with cache.actiongroup():
    for line in fileinput.input():
        if len(line) < 3:
            continue
        action, name = line[0], line[1:].strip()
        if action == '#':
            continue
        if cache.has_key(name):
            if action == '+':
                cache[name].mark_install()
            elif action == '-':
                cache[name].mark_delete()
            else:
                raise Exception("Unknown action {0}".format(action))
        else:
            raise Exception("Unknown package {0}".format(name))

#changed = apt.cache.FilteredCache(cache)
#changed.set_filter(apt.cache.MarkedChangesFilter())
print(cache.get_changes())

cache.commit()
