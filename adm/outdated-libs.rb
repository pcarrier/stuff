#!/usr/bin/env ruby

require 'set'

broken_libs = {}

pids = Dir.new("/proc").entries.grep(/^[0-9]+$/)

pids.each do |p|
  pid = p.to_i
  begin
    name = IO::read("/proc/#{pid}/cmdline")[0..-2].strip.gsub(/\0/, ' ')
    lines = IO::read("/proc/#{pid}/maps").lines
    lines.grep(/\.so.*\(deleted\)/).each do |line|
      path = line.split[5..-1].join ' '
      broken_libs[path] ||= Set.new
      broken_libs[path].add [pid, name]
    end
  rescue; end # PIDs come and go...
end

broken_libs.sort_by{|k,v|k}.each do |name, processes|
  puts name
  processes.each {|pid, name| puts "\t#{pid}\t#{name}"}
end
