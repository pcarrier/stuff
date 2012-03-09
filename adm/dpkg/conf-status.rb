#!/usr/bin/env ruby

# List dpkg configuration files and their status.
# == if unchanged
# <> if changed
# -- if removed

require 'digest/md5'

files_and_sums = %x[dpkg-query -Wf '${Conffiles}'].split.find_all {|w| w != 'obsolete'}
file_sum_hash = Hash[*files_and_sums]

file_sum_hash.each { |file, pkgsum|
  if File.exists? file
    fssum = Digest::MD5.hexdigest IO::read file
    if fssum == pkgsum
      puts "== #{file}"
    else
      puts "<> #{file}"
    end
  else
    puts "-- #{file}"
  end
}
