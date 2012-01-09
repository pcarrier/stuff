#!/usr/bin/env python

# Copyright (c) 2010, Pierre Carrier <perso@gcarrier.fr>,
#                     Eric Williams <erwillia@redhat.com>
# Permission to use, copy, modify, and/or distribute this software for any purpose
# with or without fee is hereby granted, provided that the above copyright notice
# and this permission notice appear in all copies.

import itertools

class subwords:
    def __iter__(self): return self
    def __init__(self, words, removable):
        self.words, self.removable = words, removable
        self.words_set = set(words)
    def next(self):
        while True:
            try: word = self.words.pop()
            except IndexError: raise StopIteration
            if self.removable in word:
                short = word.replace(self.removable, "")
                if short in self.words_set:
                    return word, short

def main():
    import sys
    def cleanup(x): return x.strip().lower()
    def pretty(transf): print transf[0] + " > " + transf[1]
    words = map(cleanup, sys.stdin.xreadlines())
    map(pretty, subwords(words, sys.argv[1]))

if __name__ == "__main__":
    main()

