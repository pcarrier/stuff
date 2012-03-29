require 'sinatra/base'
require 'json'

module FAI
  class NoSuchDistro < Exception; end
  class CommandFailed < Exception; end

  def self.statuses
    Hash[%x{fai-chboot -l}.collect do |l|
      if l =~ /([^ ]+) +([^ ]+) +(.*)/
        name, file, action = $1, $2, $3
        enabled = file[/\.disable$/].nil?
        next [name, {:enabled => enabled, :file => file, :action => action}]
      else
        raise Exception.new "Couldn't parse"
      end
    end]
  end

  def self.install machine, distro
    case distro
    when 'lenny'
      options = '-IFv'
    when 'squeeze'
      options = '-IFv -c squeeze'
    when 'none'
      options = '-dv'
    else
      raise NoSuchDistro.new
    end
    puts "fai-chboot #{options} #{machine}"
    output = IO.popen "fai-chboot #{options} #{machine}" do |io|
      io.read
    end
    if $? != 0 or output =~ /Source file for .* not available/
      raise CommandFailed.new output
    end
    return output
  end
end

class FAIWebAPI < Sinatra::Base
  get '/status/:machine' do |name|
    statuses = FAI.statuses
    case name
    when 'all'
      return statuses.to_json
    else
      status = statuses[name]
      return status.to_json if status
      return [404, 'No such system']
    end
  end

  post '/install/:machine/:distro' do |machine, distro|
    begin
      return FAI.install machine, distro
    rescue FAI::CommandFailed => e
      return [500, e.to_s]
    rescue FAI::NoSuchDistro => e
      return [404, e.to_s]
    end
  end
end
