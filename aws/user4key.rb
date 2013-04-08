#!/usr/bin/env ruby

require 'fog'

# Find the user owning the key kid.
# Can take a regexp.
def find_key kid
  Fog::AWS[:iam].users.find do |u|
    u.access_keys.find { |k| kid === k.id }
  end
end

# Simple CLI
if __FILE__ == $0
  u = find_key Regexp.new ARGV.first
  raise 'not found' if u.nil?
  puts "#{u.id} (#{u.user_id})"
end
