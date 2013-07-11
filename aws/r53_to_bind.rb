#!/usr/bin/env ruby

gem 'fog'
require 'fog'
require 'time'
require 'resolv'

DNS = Resolv::DNS::new

# Ensures hostnames are dot-terminated
# (they can be substrings of the rvalue, think SRV record)
def format_value v
  return v if Resolv::IPv4 === v or Resolv::IPv6 === v
  w='[A-Za-z0-9-]'
  v.gsub(
    /^
      (          # Non-.-terminated hostname:
        [A-za-z]   # Not starting by a number, excludes IPs
        (?:        # Non-capturing group: sequence of
          #{w}+\.  #   dot-terminated segments
        )+
        #{w}+      # Trailing segment
      )
      (?=\s|$)   # Space or end of line, not replaced
    /x, '\1.')
end

def format_record zone, record, value
  rv = format_value value
  name = record.name.chomp(".#{zone.domain}")
  name = record.name if name.empty?
  sprintf "%-50s %-7s %-5s %s\n",
    name, record.ttl, record.type, rv
end

Fog::DNS::AWS.new.zones.each do |zone|
  records = zone.records.all!

  File.open "zones/#{zone.domain.chomp('.')}.zone", 'w' do |file|
    file.puts <<-EOS
; #{zone.id}: #{zone.domain} (#{zone.description || 'no description'}, #{records.length} records)

$ORIGIN #{zone.domain}

EOS
    records.each do |record|
      record.value.each do |value|
        file.write format_record zone, record, value
      end

      if record.alias_target
        tgt = record.alias_target['DNSName']

        file.puts "; #{record.name}: alias for #{tgt}"
        DNS.each_address tgt do |addr|
          file.write format_record zone, record, addr
        end
      end

      file.write $/
    end
  end
end
