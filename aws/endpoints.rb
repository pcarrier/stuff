#!/usr/bin/env ruby

require 'json'
require 'nokogiri'
require 'open-uri'
require 'time'

doc = Nokogiri::HTML(open('http://docs.aws.amazon.com/general/latest/gr/rande.html'))

endpoints = {}

def extract_protocols str
  str.scan(/https?/i).collect{|p|p.downcase}.uniq
end

def format_region str
  minimized = str.downcase.
    gsub(/[^a-z]+/, '_').
    gsub(/^_+|_+$/, '').
    gsub(/_region$/, '').
    gsub(/endpoint_for_/, '').
    gsub(/_(actions|service_requests)$/, '').
    gsub(/amazon_/, '')
  case minimized
  when 'eu_ireland' then :ireland
  when 'asia_pacific_singapore' then :singapore
  when 'asia_pacific_tokyo' then :tokyo
  when 'us_west_oregon' then :oregon
  when 'us_east_northern_virginia' then :nvirginia
  when 'us_west_northern_california' then :norcal
  when 'south_america_sao_paulo' then :saopaulo
  when 'asia_pacific_sydney' then :sydney
  else minimized
  end
end

def format_entries entries
  Hash[
    entries.collect do |e|
      [format_region(e[:region]), {
        :endpoint => e[:endpoint],
        :protocols => extract_protocols(e[:protocol])
      }]
    end
  ]
end

doc.css('div.chapter#rande > div.section').each do |section|
  title = section.css('h2').text
  next if ['Overview', 'AWS GovCloud (US) Region'].include? title

  service_name = title.gsub(/^(AWS|Amazon) /, '').downcase
  service_name = $1 if service_name =~ /\((.*)\)$/
  service_name.gsub!(/[^a-z0-9]+/, '_')

  tables = section.css 'table'

  if tables.empty?
    hostnames = section.text.scan(/(?:[a-z0-9-]+\.)+[a-z]+/)
    raise 'Multiple hostnames but no table!' unless hostnames.length == 1
    protocols = extract_protocols section.text
    endpoints[service_name] = {'*' => {:endpoint => hostnames.first, :protocols => protocols}}
  else
    table = tables.first
    titles = table.css('thead th').collect do |r|
      case r.text
      when /region/i then :region
      when /endpoint/i then :endpoint
      when /protocol/i then :protocol
      else r.text
      end
    end
    entries = table.css('tbody tr').collect {|r| Hash[titles.zip r.css('td').collect{|c| c.text}]}

    endpoints[service_name] = format_entries entries
  end
end

puts JSON::dump({:generated => Time.now.utc.iso8601, :data => endpoints})
