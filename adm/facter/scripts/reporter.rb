#!/usr/bin/env ruby

%w{puppet net/http net/https socket facter json}.each {|m| require m}

raise Exception.new "Usage: reporter.rb server" if ARGV.length != 1

Puppet.parse_config
$LOAD_PATH << Puppet[:libdir]

http = Net::HTTP.new ARGV[0], 443
http.use_ssl = true
http.ca_path = '/etc/ssl/certs'
http.verify_mode = OpenSSL::SSL::VERIFY_PEER

resp = http.post "/report/#{Socket.gethostname}",
                 JSON::dump(Facter.to_hash),
                 {'Content-Type' => 'application/json'}

unless resp.code == '200'
  raise Exception.new "Server responded with #{resp.inspect}"
end
