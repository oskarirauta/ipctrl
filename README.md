# ipctrl
Log analysing nftables set manager

Description of title isn't final yet..

### Description

Software is for Openwrt system, it is supposed to work along side of fw4 (nftables manager).
It's idea is to parse log files and manipulate nftables sets to block malicious ip address
from connecting services on host.

System is supposed to use a point system, where different category filters provide points to ip address from suspicious log entries. When these entries duplicate in logs fast enough, double points are given to IP.
When set point level is reached, ip address is added to block list for specified time.
If ip address is not blocked, during time, it's points decrease until the reach 0 when ip address is removed from monitoring.

System is also supposed to enable blocklist downloading and processing.
Statistics and few commands are supposed to be exposed through ubus.

More or less, it's alternative to popular banip available on Openwrt, except this is written in C++ instead of shell script, and uses nftables (sets) instead of ipsets.

Some plans for global configuration options:

 - enable double point system
 - retain current data over ipctrl restart ( store to /tmp on exit )
 - use nftables timeout feature for double safety of ip releasing from blocked state

some log category options:
 - timeout value for category
 - match list
 - ignore list (if matched matches entry on ignore list, then ignore match)

also blacklist style config is supposed to be there, but nothing special planned for it.

system works with /etc/nftables.d/ files -
for each log category a set must be defined there, same for blacklists,
and rule must be set there in advance. This program does not add sets or rules by itself,
it only changes elements on sets. Why? Because it's more ideal this way for fw4.

to firewall config, script part must be added - script checks if ipctrl is enabled and restarts/starts it along-side with fw4.
This way restarting fw4 is supported by ipctrl.

### Status

Early WIP - nothing except testing so far is available for some parts

 - nft-set handling: functional, but not yet final
 - syslog tailing: functional, but not yet final
 - log file tailing: function, but not yet final + testing
 - log parser/matcher: not started yet
 - uci configuration support and model: not started yet
 - ubus support: not started yet
 - scheduler/loop: not started yet
 - main program: not started yet
