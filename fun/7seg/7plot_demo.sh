#!/bin/sh

# A test script for the 7seg library
# It's stupid, but it illustrates what the library allows.
#
# Copyright (c) 2011, Pierre Carrier <code@gcarrier.fr>
# Permission to use, copy, modify, and/or distribute this software
# for any purpose with or without fee is hereby granted, provided that
# the above copyright notice and this permission notice appear in all
# copies.


show_date() {
        ./7seg 7 80 "$(date +"%Y.%m.%d")" | sed 's/^/                                                                      /'
}

while true; do
        clear
        ./7seg 15 150 "$(date +"%H:%M:%S")"
        show_date
        sleep 1
        clear
        ./7seg 15 150 "$(date +"%H:%M;%S")"
        show_date
        sleep 1
done
