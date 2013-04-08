#!/usr/bin/env ruby

# Easily list the instances created by a key,
# sorted by reverse chronological order.
#
# Example:
# ./instances_from.rb pierre

gem 'fog'
require 'fog'
require 'time'

Fog::Compute[:aws].servers.
  all('key-name' => ARGV.first).
  sort{|s1,s2| s2.created_at <=> s1.created_at}
  .each do |s|
    printf "%s %s %-30s %-15s %s\n",
      s.created_at.iso8601,
      s.id,
      s.tags['Name']||'unknown',
      s.public_ip_address,
      s.groups.join(', ')
  end
