#!/usr/bin/env ruby
printf "%02x:%02x:%02x:%02x:%02x:%02x\n", *(6.times.map{rand(256)})
