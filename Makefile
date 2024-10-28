PROGS=ipx_to_udp udp_to_ipx udp_to_atalk atalk_to_udp 
OBJS=ipx_to_udp.o udp_to_ipx.o udp_to_atalk.o atalk_to_udp.o

.SUFFIXES: .c .o

.c.o:
	$(CC) -c -o $@ $<

all: $(PROGS)

ipx_to_udp: ipx_to_udp.o

atalk_to_udp: atalk_to_udp.o

udp_to_ipx: udp_to_ipx.o

udp_to_atalk: udp_to_atalk.o


.PHONY: clean

clean:
	rm -f $(PROGS) $(OBJS)
