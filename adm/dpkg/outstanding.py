#!/usr/bin/env python

# Shows all the packages that would be affected by a dist-upgrade

from __future__ import with_statement 
import apt

cache = apt.Cache()
#cache.update()
#cache.open(None)
cache.upgrade(True)
changed = apt.cache.FilteredCache(cache)
changed.set_filter(apt.cache.MarkedChangesFilter())

names = [package.name for package in cache.get_changes()]

for name in sorted(names):
  print(name)
