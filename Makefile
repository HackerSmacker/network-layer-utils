PROGS=ipx_to_udp udp_to_ipx udp_to_atalk atalk_to_udp udp_to_dnet udp_to_ip ip_to_udp
OBJS=ipx_to_udp.o udp_to_ipx.o udp_to_atalk.o atalk_to_udp.o udp_to_dnet.o udp_to_ip.o ip_to_udp.o

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


.PHONY: clean

clean:
	rm -f $(PROGS) $(OBJS)
