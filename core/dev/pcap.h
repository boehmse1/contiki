/*
 * Copyright (c) 2015 Sebastian Boehm (BTU-CS)
 *
 * Static PCAP library for IEEE 802.15.4 in Contiki
 *
 */

#include "contiki.h"
#include <stdint.h>

#ifndef __PCAP_H__
#define __PCAP_H__

#define PCAP_MAX_PACKET_SIZE        65535
#define PCAP_FRAME_HEADER_LEN		16
#define PCAP_GLOBAL_HEADER_LEN		24

#define PCAP_MAGIC_NUMBER			0xa1b2c3d4
#define PCAP_MAGIC_NUMBER_0			0xd4
#define PCAP_MAGIC_NUMBER_1			0xc3
#define PCAP_MAGIC_NUMBER_2			0xb2
#define PCAP_MAGIC_NUMBER_3			0xa1
#define PCAP_VERSION_MAJOR			2
#define PCAP_VERSION_MINOR			4

/* link types */
#define DLT_IEEE802_11_RADIO		127
#define DLT_IEEE802_15_4_PHY		147
#define DLT_IEEE802_15_4_LINUX		191
#define DLT_IEEE802_15_4			195
#define DLT_IEEE802_15_4_NONASK_PHY	215
#define DLT_IEEE802_15_4_NO_FCS		230

/* link type length */
#define DLT_IEEE802_15_4_LEN		128

typedef struct pcap_timeval_t {
	    uint32_t ts_sec;         	/* timestamp seconds */
        uint32_t ts_usec;        	/* timestamp microseconds */
}pcap_timeval_s;

typedef struct pcap_pkthdr_t {
		pcap_timeval_s ts;			/* time stamp */
        uint32_t incl_len;      	/* number of octets of packet saved in file */
        uint32_t orig_len;       	/* actual length of packet */
}pcap_pkthdr_s;

typedef struct pcap_file_header_t {
        uint32_t magic_number;   	/* magic number */
        uint16_t version_major;  	/* major version number */
        uint16_t version_minor;  	/* minor version number */
        int32_t  thiszone;       	/* GMT to local correction */
        uint32_t sigfigs;        	/* accuracy of timestamps */
        uint32_t snaplen;        	/* max length of captured packets, in octets */
        uint32_t network;        	/* data link type */
}pcap_file_header_s;

#endif /* __PCAP_H__ */
