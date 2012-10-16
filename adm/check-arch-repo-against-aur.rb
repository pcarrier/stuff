#!/usr/bin/env ruby

require 'httpclient'
require 'httpclient/include_client'
require 'json'

class AUR
  extend HTTPClient::IncludeClient
  include_http_client

  def self.infos name
    resp = http_client.get 'http://aur.archlinux.org/rpc.php', query: {type: :info, arg: name}
    data = JSON::parse resp.body
    data['results']['Version']
  end
end

def parse metadata
  Hash[metadata.lines.find_all {|l| l[0] != '#' }.collect do |l|
    l.strip.split ' = '
  end]
end

Dir.glob('*.pkg.tar.xz').sort.collect do |file|
  Thread.new do
    metadata = %x[bsdtar xfO #{file} .PKGINFO]
    infos = parse metadata
    pkgname = infos['pkgname']
    pkgver = infos['pkgver']
    newver = AUR.infos pkgname
    res = %x[vercmp #{pkgver} #{newver}].to_i
    if res < 0
      print "OUTDATED\t#{pkgname}\t#{pkgver} #{newver}\n"
    elsif res == 0
      print "identical\t#{pkgname}\t#{pkgver} #{newver}\n"
    else
      print "TOO NEW\t#{pkgname}\t#{pkgver} #{newver}\n"
    end
  end
end.each {|t| t.join}
