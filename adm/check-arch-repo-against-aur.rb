#!/usr/bin/env ruby

require 'httparty'

class AUR
  include HTTParty
  format :json
  base_uri 'https://aur.archlinux.org'

  def self.infos name
    get('/rpc.php', query: {type: :info, arg: name}).parsed_response['results']
  end
end

def parse metadata
  Hash[metadata.lines.find_all {|l| l[0] != '#' }.collect do |l|
    l.strip.split ' = '
  end]
end

Dir.glob('*.pkg.tar.xz').sort.each do |file|
  metadata = %x[bsdtar xfO #{file} .PKGINFO]
  infos = parse metadata
  pkgname = infos['pkgname']
  pkgver = infos['pkgver']
  newver = AUR.infos(pkgname)['Version']
  res = %x[vercmp #{pkgver} #{newver}].to_i
  if res < 0
    puts "OUTDATED\t#{pkgname}\t#{pkgver} #{newver}"
  elsif res == 0
    puts "identical\t#{pkgname}\t#{pkgver} #{newver}"
  else
    puts "TOO NEW\t#{pkgname}\t#{pkgver} #{newver}"
  end
end
