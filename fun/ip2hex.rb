#!/usr/bin/env ruby
puts '%X' % (ARGV[0].split('.').collect{|n| n.to_i}.reverse.inject(0){|r,e| (r<<8)+e})
