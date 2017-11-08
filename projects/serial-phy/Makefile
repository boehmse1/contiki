CONTIKI = ../..
CONTIKI_PROJECT = transceiver
all: $(CONTIKI_PROJECT)

UIP_CONF_RPL=0
UIP_CONF_IPV6=0

CFLAGS+= -DPROJECT_CONF_H=\"project-conf.h\" 

ifeq ($(TARGET),mbxxx)
CFLAGS+= -DSNIFFER_FRAMES_HAVE_FCS
endif

PROJECT_SOURCEFILES+= transceiver-rdc.c atmega128rfa1.c
include $(CONTIKI)/Makefile.include

sniff:
	@./../../../../contiki/tools/sky/serialdump-linux -b57600 /dev/ttyUSB1 | wireshark -k -i -

serialdebug:
	@./../../../../contiki/tools/sky/serialdump-linux -b57600 /dev/ttyUSB1

serialfiledump:
	@./../../../../contiki/tools/sky/serialdump-linux -b57600 /dev/ttyUSB1 > log.pcap