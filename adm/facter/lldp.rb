# Gets all the values from lldpctl

lldp_infos = IO.popen('lldpctl -f keyvalue') do |pipe|
    pipe.readlines.collect do |line|
        line.chomp.split '='
    end
end

answers_per_itf = {}

lldp_infos.each do |k, v|
    if prefix = k[/lldp\.[^.]+/]
        itf = prefix[5..-1]
        if k =~ /lldp\.[^.]+\.via/
            answers_per_itf[itf] ||= 0
            answers_per_itf[itf] += 1
        end
    end
end

answers_per_itf.each do |itf, answers|
    Facter.add("lldp_#{itf}_answers") { setcode { answers } }
end

Hash[lldp_infos].find_all do |k, v|
    k =~ /(chassis\.name|port\.descr|port\.mac|vlan-id)$/
end.each do |k, v|
    itf = k[/lldp\.[^.]+/][5..-1]
    if answers_per_itf[itf] == 1
        fkey = k.gsub /[^a-zA-Z0-9]/, '_'
        Facter.add(fkey) { setcode { v } }
    end
end
