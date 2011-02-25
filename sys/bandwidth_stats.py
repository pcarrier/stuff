#!/usr/bin/env python
# Bandwidth statistics for Linux interfaces
#
# Example:
# bandwith_stats.py -d 5 lo eth1 eth3
#
# Copyright (c) 2010, Red Hat Inc.
# Permission to use, copy, modify, and/or distribute this software
# for any purpose with or without fee is hereby granted, provided that
# the above copyright notice and this permission notice appear in all copies.
#
# Maintainer: Pierre Carrier <prc@redhat.com>
#
# TODO:
# - Use powers of 1000 instead of 1024 for packets

import getopt, sys, time

STATS = {
    'rx_bytes': 'RX',
    'tx_bytes': 'TX',
    'rx_packets': 'RXp',
    'tx_packets': 'TXp'
}

def read_stats(itf):
    res = {}
    stats_dir = '/sys/class/net/'+itf+'/statistics/'
    for stat in STATS:
        res[stat] = int(open(stats_dir+stat, 'r').read())
    return res

def usage():
    print """Usage: %s [-d delay] [interface ...]
    defaults to eth0, every second""" % sys.argv[0]

def formatted_stat(name, value):
    value = float(value)
    suffixes = [' ', 'K', 'M', 'G', 'T']
    power_of_1024 = 0
    while(value >= 1024):
        value /= 1024
        power_of_1024 += 1
    return "%s:%7.2f%s" % (name, value, suffixes[power_of_1024])

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hd:')
    except getopt.GetoptError, err:
        print >> sys.stderr, str(err)
        usage()
        sys.exit(1)
    delay = 1.0
    for opt, value in opts:
        if opt == '-d':
            try:
                delay = float(value)
            except:
                print >> sys.stderr, "Could not parse the delay"
		sys.exit(2)
        elif opt == '-h':
            usage()
            sys.exit(0)

    interfaces = args or ['eth0']
    stats = {}

    for itf in interfaces:
        stats[itf] = read_stats(itf)
    
    while True:
        time.sleep(delay)
        for itf in interfaces:
            line = itf+":\t"
            previous = stats[itf]
            stats[itf] = read_stats(itf)
            current = stats[itf]
            for stat in STATS:
                line += formatted_stat(STATS[stat], (current[stat]-previous[stat])/delay) + ", "
            print(line[:-2])
        if len(interfaces) > 1:
            print '-'

if __name__ == "__main__":
    main()
