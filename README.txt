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
