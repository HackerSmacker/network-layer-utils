PROGS=ipx_to_udp udp_to_ipx udp_to_atalk atalk_to_udp udp_to_dnet udp_to_ip ip_to_udp udp_to_ip_bsd ip_to_udp_bsd
OBJS=ipx_to_udp.o udp_to_ipx.o udp_to_atalk.o atalk_to_udp.o udp_to_dnet.o udp_to_ip.o ip_to_udp.o udp_to_ip_bsd.o ip_to_udp_bsd.o

.SUFFIXES: .c .o

.c.o:
	$(CC) -c -o $@ $<

all: $(PROGS)

ipx_to_udp: ipx_to_udp.o

atalk_to_udp: atalk_to_udp.o

udp_to_ipx: udp_to_ipx.o

udp_to_atalk: udp_to_atalk.o

udp_to_ip: udp_to_ip.o

ip_to_udp: ip_to_udp.o

udp_to_dnet: udp_to_dnet.o
	$(CC) -o udp_to_dnet udp_to_dnet.o -ldnet

udp_to_ip_bsd: udp_to_ip_bsd.o

ip_to_udp_bsd: ip_to_udp_bsd.o

.PHONY: clean

clean:
	rm -f $(PROGS) $(OBJS)
