#!/usr/bin/env ruby

=begin
=======================================
Synchronize desktop launchers' position
    Ruby prototype for Gnome, RHEL5
******* Not endorsed by Red Hat *******
=======================================

WARNING
-------

This script will NOT work with a recent nautilus.
It should work nautilus 2.16.
Icons positions are stored in gconf in last releases.

LICENSE
-------

Copyright (c) 2010, Pierre Carrier <prc@redhat.com>

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

EXAMPLE
-------

To synchronize from $SRCUSER to $DSTUSER, use:

script.rb /home/${SRCUSER}/.nautilus/metafiles/file:%2F%2F%2Fhome%2F${SRCUSER}%2FDesktop.xml \
          /home/${DSTUSER}/.nautilus/metafiles/file:%2F%2F%2Fhome%2F${DSTUSER}%2FDesktop.xml

=end

require "rexml/document"
include REXML

srcfile = File.new(ARGV[0])
srcdoc = REXML::Document.new srcfile
srcfile.close

dstfile = File.new(ARGV[1])
dstdoc = REXML::Document.new dstfile
dstfile.close

XPath.each srcdoc, "//directory/file"  do |s|
  d = XPath.first dstdoc, "//directory/file[@name='#{s.attributes['name']}']"
  if d != nil then
    unless d.attributes['icon_position'] == s.attributes['icon_position'] then
      d.attributes['icon_position'] = s.attributes['icon_position']
      print "updated position for #{s.attributes['name']}\n"
    end
  else
    dstdoc.root.add_element 'file', s.attributes
    print "added info for #{s.attributes['name']}\n"
  end
end

dstdoc.write File.new(ARGV[1],'w')
