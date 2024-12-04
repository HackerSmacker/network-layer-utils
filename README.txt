Various programs to perform data transfer on as many network protocols as I could get to work.
Warning: most of these are very lazily written!
These programs are designed to accept an MPEG transport stream as their test payloads, which has a packet payload size of 1316. The "common protocol" between all of these is UDP.
 
LINUX:
ipx_to_udp: IPX datagrams -> UDP
udp_to_ipx: UDP -> IPX datagrams
atalk_to_udp: AppleTalk DDP (protocol number 0xFF) -> UDP
udp_to_atalk: UDP -> AppleTalk DDP
udp_to_dnet: UDP -> DECnet NSP session
udp_to_ip: UDP -> IP raw 
ip_to_udp: IP -> UDP raw

BSD:
udp_to_ip_bsd: UDP -> IP
ip_to_udp_bsd: IP -> UDP



USAGE:
udp_to_atalk:
Let's say you have a Linux machine (that you can run this program on) with the
address 1.47, and you wish to send packets to a Windows box at 1.10. If 
for some reason "show appletalk arp" (Cisco IOS) or "cat /proc/net/atalk/arp" 
(Linux) does not show the destination node, you can open the AppleTalk 
Control Panel applet (under Network Settings) on Windows, then close it to
force it to send out a new RTMP beacon. After determining the destination 
address, run "./udp_to_atalk 5003 0 10 150" to transmit packets to 0.10 port
150 -- the 0 part of 0.10 tells the kernel to automatically figure out a route
to that host by checking the ARP table.

udp_to_ipx:
If you're using Linux, you need to run "ipx_configure" to set the default route,
otherwise there won't be one. Check "/proc/net/ipx/interface" to figure out
what Linux thinks the default network interface is, then check the 
"/proc/net/ipx/route" file to figure out what the destination internal network
number is supposed to be. For example, I want to send packets to
373FFF93:AA0004002504, with the router net being 00000001. That means that
00000001 isn't the actual network address, so, that means you need to run
the program like this:
"./udp_to_ipx 5003 373FFF93 AA0004002704 5000". This sends packets to that 
destination and network, on port 0x5000. For a broadcast, do 
00000000.ffffffffffff:5000 for the destination.
