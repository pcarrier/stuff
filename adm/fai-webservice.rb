require 'sinatra/base'
require 'json'

error 404 do
    'Not found'
end
error ParseError do
    'Parsing issue'
end

module FAI
    class NoSuchDistro < Exception; end
    class CommandFailed < Exception; end

    def self.statuses
        Hash[%x{fai-chboot -l}.collect do |l|
            if l =~ /([^ ]+) +([^ ]+) +(.*)/
                [:name => $1, {:file => $2, :action => $3}]
            else
                raise ParseError.new
            end
        end]
    end

    def self.install :machine, :distro
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
        output = IO.popen "fai-chboot #{options} #{machine}" do |io|
            io.read
        end
        raise CommandFailed.new output unless $? == 0
        case $?
        if output =~ /Source file for .* not available/
            raise NoSuchDistro.new
        end
        return output
    end
end

class FAIWebAPI < Sinatra::Base
    get '/status/:machine' do |name|
        statuses = FAI.statuses
        case machine
        when 'all'
            statuses.to_json
        else
            return statuses[machine].to_json or [404, 'No such system']
        end
    end

    post '/boot/:machine/:distro' do |machine, distro|
        retcode, output = FAI.install name, machine
        begin
            output
        rescue CommandFailed => e
            [500, e.to_s]
        rescue NoSuchDistro => e
            [404, e.to_s]
        end
    end

    run! if app_file == $0
end
