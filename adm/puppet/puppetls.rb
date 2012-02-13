#!/usr/bin/env ruby

%w{yaml puppet}.each { |m| require m }

def display_resource res, length_to_strip
  rel_path = res.title[length_to_strip .. - 1]

  if res[:ensure] == 'absent'
    description = 'GETS REMOVED'
  else
    source = res[:source]
    source = 'a "content" parameter' if source.nil? or source.empty?
    description = "content from #{source}"
  end

  puts "#{rel_path}\n  declared in #{res.file}:#{res.line}\n  #{description}"
end

def display_dir cat, path, prefix, length_to_strip
  cat.resources.find_all { |res|
    res.type == 'File' and res.title.start_with? prefix
  }.each { |res|
    display_resource res, length_to_strip
  }
end

def display_file cat, path, length_to_strip
  res = cat.resources.find { |res|
    res.type == 'File' and res.title == path
  }
  display_resource res, length_to_strip
end

if __FILE__ == $PROGRAM_NAME
  Puppet.parse_config
  catalog = YAML::load IO::read Dir.glob(File.join(Puppet[:clientyamldir], 'catalog', '*')).first

  if ARGV.length > 0
    path = File.expand_path ARGV[0]
  else
    path = Dir.pwd
  end

  if path == '/'
    prefix = path
    length_to_strip = 0
    display_dir catalog, path, prefix, length_to_strip
  elsif File.directory? path
    prefix = path + '/'
    length_to_strip = path.length + 1
    display_dir catalog, path, prefix, length_to_strip
  else
    prefix = path
    length_to_strip = 0
    display_file catalog, path, length_to_strip
  end
end
