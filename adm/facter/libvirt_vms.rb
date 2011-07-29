# Gets all the virtual machines, running or not

vms = IO.popen('virsh list --all') do |pipe|
    pipe.lines.reject {|l| l =~ /^ *Id |^-+$|^$/}.collect do |line|
        entries = line.split(' ')
        if entries.length >= 3
            "#{entries[1]}:#{entries[2..-1].join '_'}"
        else
            nil
        end
    end
end.compact.join ','

Facter.add('libvirt_vms') { setcode { vms } } unless vms.empty?
