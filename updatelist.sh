#!/bin/bash
china=`curl -sSL https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf | while read line; do awk -F '/' '{print $2}' | grep -v '#' ; done |  paste -sd " " -`
apple=`curl -sSL https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf | while read line; do awk -F '/' '{print $2}' | grep -v '#' ; done |  paste -sd " " -`
google=`curl -sSL https://github.com/felixonmars/dnsmasq-china-list/raw/master/google.china.conf | while read line; do awk -F '/' '{print $2}' | grep -v '#' ; done |  paste -sd " " -`
bogus=`curl -sSL https://github.com/felixonmars/dnsmasq-china-list/raw/master/bogus-nxdomain.china.conf | grep "=" | while read line; do awk -F '=' '{print $2}' | grep -v '#' ; done |  paste -sd " " -`
echo $china > src/res/china.txt
echo $google > src/res/google.txt
echo $apple > src/res/apple.txt
echo $bogus > src/res/bogus.txt