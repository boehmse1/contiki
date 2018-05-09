/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * Static PCAPNG library for Contiki
 *
 */
 
#include "contiki.h"
#include <stdio.h>

#ifndef __PCAPNG_H__
#define __PCAPNG_H__

#define PCAPNG_MAX_PACKET_SIZE      	65535

#define PCAPNG_MAGIC_NUMBER				0x1a2b3c4d
#define PCAPNG_VERSION_MAJOR			1
#define PCAPNG_VERSION_MINOR			0
#define PCAPNG_SECTION_LENGTH_UNDEFINED	-1

/* options */
#define PCAPNG_OPT_ENDOFOPT				0
#define PCAPNG_OPT_COMMENT				1
#define PCAPNG_OPT_SHB_HARDWARE			2
#define PCAPNG_OPT_SHB_OS				3
#define PCAPNG_OPT_SHB_USERAPPL			4
#define PCAPNG_OPT_IDB_IF_NAME			2

/** block types */
typedef enum pcapng_block_type{
	PCAPNG_BLOCK_TYPE_IDB = 0x00000001,	/* Interface Description Block */
	PCAPNG_BLOCK_TYPE_SPB = 0x00000003, /* Simple Packet Block */
	PCAPNG_BLOCK_TYPE_NRB = 0x00000004, /* Name Resolution Block */
	PCAPNG_BLOCK_TYPE_ISB = 0x00000005, /* Interface Statistics Block */
	PCAPNG_BLOCK_TYPE_EPB = 0x00000006, /* Enhanced Packet Block */
	PCAPNG_BLOCK_TYPE_SHB = 0x0A0D0D0A,	/* Section Header Block */
	PCAPNG_BLOCK_TYPE_CB = 0x00000BAD	/* Custom Block */
}pcapng_block_type;

static inline const char* pcapngBlockTypeToString(pcapng_block_type x)
{
    switch (x)
    {
        case PCAPNG_BLOCK_TYPE_IDB:		return "Interface Description Block";
        case PCAPNG_BLOCK_TYPE_SPB:		return "Simple Packet Block";
        case PCAPNG_BLOCK_TYPE_NRB:		return "Name Resolution Block";
        case PCAPNG_BLOCK_TYPE_ISB:		return "Interface Statistics Block";
        case PCAPNG_BLOCK_TYPE_EPB:		return "Enhanced Packet Block";
        case PCAPNG_BLOCK_TYPE_SHB:		return "Section Header Block";
        case PCAPNG_BLOCK_TYPE_CB:		return "Custom Block";
        default:                    	return "[Unknown block_type]";
    }
}

static inline uint8_t pcapngBlockTypeValid(pcapng_block_type x)
{
    switch (x)
    {
    	case PCAPNG_BLOCK_TYPE_SHB:
    	case PCAPNG_BLOCK_TYPE_IDB:
    	case PCAPNG_BLOCK_TYPE_EPB:		return 1;
    	default:						return 0;
    }
}

typedef struct pcapng_block_header_t {
	pcapng_block_type block_type;	/* block identifier */
	uint32_t block_total_length;	/* block total length */
	/* variable block_body */
	/* uint32_t block_total_length */
}pcapng_block_header_s;

typedef struct pcapng_option_header_t {
	uint16_t option_code;
	uint16_t option_length;
	uint32_t option_value1; 		/* aligned to 32 bits */
	uint32_t option_value2; 		/* aligned to 32 bits */
	uint32_t option_value3; 		/* aligned to 32 bits */
	uint32_t option_value4; 		/* aligned to 32 bits */
	/* uint16_t option_code == opt_endofopt */
	/* uint16_t option_length == 0 */
}pcapng_option_header_s;

typedef struct pcapng_section_header_block_t {
	uint32_t magic;					/* magic number */
	uint16_t version_major;			/* major version number */
	uint16_t version_minor;			/* minor version number */
	//uint64_t section_length; 		/* might be -1 for unknown */
	uint32_t section_length_high; 	/* for convenience with small data types */
	uint32_t section_length_low; 	/* for convenience with small data types */

	/* ... Options ... */
}pcapng_section_header_block_s;

typedef struct pcapng_interface_description_block_t {
	uint16_t linktype;				/* data link type */
	uint16_t reserved;
	uint32_t snaplen;				/* max length of captured packets, in octets */
	/* ... Options ... */
}pcapng_interface_description_block_s;

typedef struct pcapng_enhanced_packet_block_t {
	uint32_t interface_id;			/* interface identifier */
	uint32_t timestamp_high;		/* timestamp high */
	uint32_t timestamp_low;			/* timestamp low */
	uint32_t captured_len;			/* capture length */
	uint32_t packet_len;			/* packet lenght */
	/* ... Packet Data ... */
	/* ... Padding ... */
	/* ... Options ... */
}pcapng_enhanced_packet_block_s;

typedef struct pcapng_simple_packet_block_t {
	uint32_t packet_len;			/* packet lenght */
	/* ... Packet Data ... */
	/* ... Padding ... */
}pcapng_simple_packet_block_s;

typedef struct pcapng_name_resolution_block_t {
	uint16_t record_type;			/* record type */
	uint16_t record_len;			/* record length */
	/* ... Record ... */
}pcapng_name_resolution_block_s;

typedef struct pcapng_interface_statistics_block_t {
	uint32_t interface_id;			/* interface identifier */
	uint32_t timestamp_high;		/* timestamp high */
	uint32_t timestamp_low;			/* timestamp low */
	/* ... Options ... */
}pcapng_interface_statistics_block_s;

#endif /* __PCAPNG_H__ */
