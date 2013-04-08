#!/usr/bin/env ruby

# Easily list the instances created by a key,
# sorted by reverse chronological order.
#
# Example:
# ./instances_from.rb pierre

gem 'aws-creds'
gem 'aws-sdk'
require 'aws-sdk'
require 'aws/creds'
require 'time'

AWS.config AWS::Creds[:default].to_hash

AWS::EC2.new.instances.
  filter('key-name', ARGV.first).
  sort{|a,b|b.launch_time <=> a.launch_time}.
  each do |i|
  printf "%s %s %-30s %-15s %s\n",
      i.launch_time.iso8601,
      i.id,
      i.tags['Name']||'unknown',
      i.ip_address,
      i.groups.collect{|g|g.name}.join(', ')
end
