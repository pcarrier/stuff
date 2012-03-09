#!/usr/bin/env ruby

# List dpkg configuration files and their status.
# == if unchanged
# <> if changed
# -- if removed

require 'digest/md5'

out = %x[dpkg-query -Wf '${Conffiles}']
files_and_sums = out.split.find_all {|w| w != 'obsolete'}
file_sum_hash = Hash[*files_and_sums]

file_sum_hash.sort.each { |file, pkgsum|
  if File.exists? file
    fssum = Digest::MD5.hexdigest IO::read file
    flag = (fssum == pkgsum ? '==' or '<>'
  else
    flag = '--'
  end
  puts "#{flag} #{file}"
}
