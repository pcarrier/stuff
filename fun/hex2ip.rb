#!/usr/bin/env ruby
n = ARGV[0].hex
d = n % 256
c = (n >> 8) % 256
b = (n >> 16) % 256
a = (n >> 24) % 256
puts "#{a}.#{b}.#{c}.#{d}"
