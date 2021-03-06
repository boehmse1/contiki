/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * Serial line PCAPNG interpreter
 * Note: Forwarded PCAPNG line event messages represents PCAPNG blocks
 *
 */

#include "dev/pcap.h"
#include "dev/pcapng.h"
#include "dev/pcapng-line.h"
#include "net/packetbuf.h"
#include "lib/ringbuf.h"

#include "sys/rtimer.h"

#define DEBUG 0
#if defined(DEBUG) && DEBUG == 1
#define print_debug(fmt, args...) printf("[Pcapng_Line]: " fmt "\n", ##args)
#define PRINTD(...)
#elif defined(DEBUG) && DEBUG == 2
#define PRINTD(fmt, args...) printf("[Pcapng_Line]: " fmt "\n", ##args)
#define print_debug(...)
#elif defined(DEBUG) && DEBUG == 3
#define printd(fmt, args...) printf("[Pcapng_Line]: " fmt "\n", ##args)
#define PRINTD(...)
#else
#define PRINTD(...)
#define print_debug(...)
#endif

#define FILEWRITE 0
#if FILEWRITE==0
#include "uart.h"
#define writefile(...)
#elif FILEWRITE==1
#include <stdio.h>
#define writefile(fmt, args...) fprintf(fmt, ##args)
#elif FILEWRITE==2
#include <stdio.h>
#endif

#ifdef PCAP_LINE_CONF_BUFSIZE
#define BUFSIZE PCAP_LINE_CONF_BUFSIZE
#else /* PCAP_LINE_CONF_BUFSIZE */
#define BUFSIZE PACKETBUF_SIZE
#endif /* PCAP_LINE_CONF_BUFSIZE */

#if (BUFSIZE & (BUFSIZE - 1)) != 0
#error PCAP_LINE_CONF_BUFSIZE must be a power of two (i.e., 1, 2, 4, 8, 16, 32, 64, ...).
#error Change PCAP_LINE_CONF_BUFSIZE in contiki-conf.h.
#endif

/* pcap states */
enum {
  PCAPNG_IDLE = 0,
  PCAPNG_READ_BLOCK = 1,
};

static uint8_t state = PCAPNG_IDLE;
static struct ringbuf rxbuf;
static uint8_t rxbuf_data[BUFSIZE];
#if FILEWRITE==1
static FILE *f;
#endif

static rtimer_clock_t rx_time;
static rtimer_clock_t tx_time;

/* default broadcast events without consumer registered */
static struct process *p = PROCESS_BROADCAST;
static uint8_t registered = 0;

PROCESS(pcapng_line_process, "PCAPNG serial driver");

process_event_t pcapng_event_shb;
process_event_t pcapng_event_idb;
process_event_t pcapng_event_epb;
//process_event_t pcapng_event_cb;

/*
 * register a consumer process for pcapng events
 */
void pcapng_line_register_consumer_process(struct process *consumer)
{
	if (!registered) {
		p = consumer;
		PRINTD("consumer process '%s' registered!\n", consumer->name);
		registered = 1;
	} else {
		PRINTD("consumer process '%s' is already registered!\n", p->name);
	}
}

/*
 * writes raw bytes to serial line
 */
void pcapng_line_write(const void *_ptr, uint32_t len)
{
	const uint8_t *ptr = _ptr;
	uint16_t i;

	for (i=0; i<len; i++ ) {
#if FILEWRITE==1
		writefile(f, "%c", *ptr++);
#elif FILEWRITE==2
		printf("%c", *ptr++);
#else
		uart_putc(*ptr++);
#endif
	}

#if FILEWRITE==1
	fflush(f);
#endif
}

/*---------------------------------------------------------------------------*/
void
pcapng_line_write_shb(void) // options?
{
	pcapng_block_header_s bh;
	pcapng_section_header_block_s shb;

	/* write block header */
	bh.block_type = PCAPNG_BLOCK_TYPE_SHB;
	bh.block_total_length = (uint32_t)(sizeof(bh) + sizeof(shb) + 4); // todo options len

	/* write fixed block content */
	shb.magic 				= PCAPNG_MAGIC_NUMBER;
	shb.version_major 		= PCAPNG_VERSION_MAJOR;
	shb.version_minor 		= PCAPNG_VERSION_MINOR;
	shb.section_length_high = PCAPNG_SECTION_LENGTH_UNDEFINED;
	shb.section_length_low 	= PCAPNG_SECTION_LENGTH_UNDEFINED;

	pcapng_line_write(&bh, sizeof(bh));
	pcapng_line_write(&shb, sizeof(shb));
	pcapng_line_write(&bh.block_total_length, 4);
}

/*---------------------------------------------------------------------------*/
void
pcapng_line_write_idb(uint16_t data_link, uint32_t snap_len)
{
	pcapng_block_header_s bh;
	pcapng_interface_description_block_s idb;

	/* write block header */
	bh.block_type 			= PCAPNG_BLOCK_TYPE_IDB;
	bh.block_total_length 	= (uint32_t)(sizeof(bh) + sizeof(idb) + 4); // todo: options len

	/* write block content */
	idb.linktype    = data_link;
	idb.reserved    = 0;
	idb.snaplen     = snap_len;

	pcapng_line_write(&bh, sizeof(bh));
	pcapng_line_write(&idb, sizeof(idb));
	pcapng_line_write(&bh.block_total_length, 4);
}

/*---------------------------------------------------------------------------*/
void
pcapng_line_write_epb(uint32_t interface, pcap_timeval_s *ts, const void * data, uint32_t length)
{
	pcapng_block_header_s bh;
	pcapng_enhanced_packet_block_s epb;

	/* padding */
	uint8_t pad_len;
	uint8_t pad[] = {0,0,0,0};

	/* set length of padding bytes */
	if (length%4) {
	  pad_len = 4 - (length%4);
	} else {
	  pad_len = 0;
	}

	/* write block header */
	bh.block_type 			= PCAPNG_BLOCK_TYPE_EPB;
	bh.block_total_length 	= (uint32_t)(sizeof(bh) + sizeof(epb) + length + pad_len + 4); // todo: options len

	/* write block content */
	epb.interface_id 		= interface;
	epb.timestamp_high		= ts->ts_sec;
	epb.timestamp_low		= ts->ts_usec;
	epb.captured_len 		= length;
	epb.packet_len 			= length;

#if DEBUG == 3
	/* for time measurement */
	//rtimer_clock_t time;
	//tx_time = rtimer_arch_now();
	//time = tx_time - rx_time;
	//epb.timestamp_high		= (time - (time % RTIMER_ARCH_SECOND)) / RTIMER_ARCH_SECOND;
	//epb.timestamp_low		= 1000000 * (time % RTIMER_ARCH_SECOND) / RTIMER_ARCH_SECOND;
#endif

	pcapng_line_write(&bh, sizeof(bh));
	pcapng_line_write(&epb, sizeof(epb));
	pcapng_line_write(data, length);
	pcapng_line_write(&pad, pad_len);
	pcapng_line_write(&bh.block_total_length, 4);

#if DEBUG == 3
	/* for time measurement */
	rtimer_clock_t time;
	tx_time = rtimer_arch_now();
	time = tx_time - rx_time;
	epb.timestamp_high		= (time - (time % RTIMER_ARCH_SECOND)) / RTIMER_ARCH_SECOND;
	epb.timestamp_low		= 1000000 * (time % RTIMER_ARCH_SECOND) / RTIMER_ARCH_SECOND;
	printd("Response Time (us): %lu", (epb.timestamp_high * 1000000) + epb.timestamp_low);
#endif
}

/*---------------------------------------------------------------------------*/
//void
//pcapng_line_write_cb(const void * data, uint32_t length)
//{
//
//}

/*---------------------------------------------------------------------------*/
void
pcapng_line_read_shb(uint8_t * ptr, pcapng_section_header_block_s * section)
{
	uint8_t *data = ptr + sizeof(pcapng_block_header_s);

	section->magic |= (uint32_t) *data++;
	section->magic |= (uint32_t) *data++ << 8;
	section->magic |= (uint32_t) *data++ << 16;
	section->magic |= (uint32_t) *data++ << 24;
	section->version_major |= (uint16_t) *data++;
	section->version_major |= (uint16_t) *data++ << 8;
	section->version_minor |= (uint16_t) *data++;
	section->version_minor |= (uint16_t) *data++ << 8;
	section->section_length_high |= (uint32_t) *data++;
	section->section_length_high |= (uint32_t) *data++ << 8;
	section->section_length_high |= (uint32_t) *data++ << 16;
	section->section_length_high |= (uint32_t) *data++ << 24;
	section->section_length_low |= (uint32_t) *data++;
	section->section_length_low |= (uint32_t) *data++ << 8;
	section->section_length_low |= (uint32_t) *data++ << 16;
	section->section_length_low |= (uint32_t) *data++ << 24;
}

/*---------------------------------------------------------------------------*/
void
pcapng_line_read_idb(uint8_t * ptr, pcapng_interface_description_block_s * interface)
{
	uint8_t *data = ptr + sizeof(pcapng_block_header_s);

	interface->linktype |= (uint16_t) *data++;
	interface->linktype |= (uint16_t) *data++ << 8;
	interface->reserved |= (uint16_t) *data++;
	interface->reserved |= (uint16_t) *data++ << 8;
	interface->snaplen |= (uint32_t) *data++;
	interface->snaplen |= (uint32_t) *data++ << 8;
	interface->snaplen |= (uint32_t) *data++ << 16;
	interface->snaplen |= (uint32_t) *data++ << 24;
}

/*---------------------------------------------------------------------------*/
void
pcapng_line_read_epb(uint8_t * ptr, pcapng_enhanced_packet_block_s * packet)
{
	uint8_t *data = ptr + sizeof(pcapng_block_header_s);

	packet->interface_id |= (uint32_t) *data++;
	packet->interface_id |= (uint32_t) *data++ << 8;
	packet->interface_id |= (uint32_t) *data++ << 16;
	packet->interface_id |= (uint32_t) *data++ << 24;
	packet->timestamp_high |= (uint32_t) *data++;
	packet->timestamp_high |= (uint32_t) *data++ << 8;
	packet->timestamp_high |= (uint32_t) *data++ << 16;
	packet->timestamp_high |= (uint32_t) *data++ << 24;
	packet->timestamp_low |= (uint32_t) *data++;
	packet->timestamp_low |= (uint32_t) *data++ << 8;
	packet->timestamp_low |= (uint32_t) *data++ << 16;
	packet->timestamp_low |= (uint32_t) *data++ << 24;
	packet->captured_len |= (uint32_t) *data++;
	packet->captured_len |= (uint32_t) *data++ << 8;
	packet->captured_len |= (uint32_t) *data++ << 16;
	packet->captured_len |= (uint32_t) *data++ << 24;
	packet->packet_len |= (uint32_t) *data++;
	packet->packet_len |= (uint32_t) *data++ << 8;
	packet->packet_len |= (uint32_t) *data++ << 16;
	packet->packet_len |= (uint32_t) *data++ << 24;
}

/*---------------------------------------------------------------------------*/
int
pcapng_line_input_byte(unsigned char c)
{
  /* Add character */
  ringbuf_put(&rxbuf, c);

  /* Wake up consumer process */
  process_poll(&pcapng_line_process);
  return 1;
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(pcapng_line_process, ev, data)
{
  static uint8_t buf[BUFSIZE*2];
  static pcapng_block_header_s header;
  static uint16_t ptr;
  static int c;

  PROCESS_BEGIN();

  pcapng_event_shb = process_alloc_event();
  pcapng_event_idb = process_alloc_event();
  pcapng_event_epb = process_alloc_event();
  //pcapng_event_cb = process_alloc_event();

  while (1) {
	  /* get character from buffer */
	  c = ringbuf_get(&rxbuf);
	  PRINTD("PCAPNG_RECEIVED_CHARACTER %i on pos %i", c, ptr);

	  /* if buffer is empty */
	  if (c == -1) {
		  PROCESS_YIELD();

	  /* character received in buffer */
	  } else {
		  /* switch state */
		  switch (state) {

		  case PCAPNG_IDLE:
			  /* read bytes of a pcapng block into buffer */
			  if (ptr < BUFSIZE-1) {
				  buf[ptr++] = (uint8_t) c;
			  }
			  /* read block type and length */
			  if (ptr == 1) header.block_type |= (uint32_t) c;
			  else if (ptr == 2) header.block_type |= (uint32_t) c << 8;
			  else if (ptr == 3) header.block_type |= (uint32_t) c << 16;
			  else if (ptr == 4) header.block_type |= (uint32_t) c << 24;
			  else if (ptr == 5) header.block_total_length |= (uint32_t) c;
			  else if (ptr == 6) header.block_total_length |= (uint32_t) c << 8;
			  else if (ptr == 7) header.block_total_length |= (uint32_t) c << 16;
			  else if (ptr == 8) header.block_total_length |= (uint32_t) c << 24;
			  /* if we have a valid pcapng block type with length*/
			  if (ptr == 8) {
				  PRINTD("PCAPNG_BLOCK(%s, %u)", pcapngBlockTypeToString(header.block_type), header.block_total_length);
				  if (pcapngBlockTypeValid(header.block_type)) {
					  /* read the pcapng block */
					  state = PCAPNG_READ_BLOCK;
					  /* set length of padding bytes */
					  if (header.block_total_length%4) {
						  PRINTD("ERROR: Total block length must ne a multiple of 4!");
					  }
				  } else {
					  /* reset the packet header and pointer */
					  header.block_type = 0;
					  header.block_total_length = 0;
					  ptr = 0;
				  }
			  }
			  break;

		  case PCAPNG_READ_BLOCK:
			  /* just read the whole pcapng block into buffer */
			  if (ptr < (header.block_total_length)) {
				  buf[ptr++] = (uint8_t) c;
			  }
			  /* last byte of block */
			  if (ptr == (header.block_total_length)) {

#if DEBUG == 3
				  /* for time measurement */
				  rx_time = rtimer_arch_now();
#endif

				  /* broadcast event by type */
				  switch (header.block_type) {

				  case PCAPNG_BLOCK_TYPE_SHB:
					  process_post(p, pcapng_event_shb, buf);
					  break;
				  case PCAPNG_BLOCK_TYPE_IDB:
					  process_post(p, pcapng_event_idb, buf);
					  break;
				  case PCAPNG_BLOCK_TYPE_EPB:
					  process_post(p, pcapng_event_epb, buf);
					  break;
				  default:
					  PRINTD("PCAPNG_BLOCK_TYPE(%u, %s) UNSUPPORTED!\n", header.block_type, pcapngBlockTypeToString(header.block_type));
					  break;
				  }
				  PRINTD("PCAPNG_BROADCAST_EVENT(%s)\n", pcapngBlockTypeToString(header.block_type));
				  /* back to idle */
				  state = PCAPNG_IDLE;
				  /* reset the packet header and pointer */
				  header.block_type = 0;
				  header.block_total_length = 0;
				  ptr = 0;
			  }
			  break;
		  }
	  }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
void
pcapng_line_init(void)
{
  ringbuf_init(&rxbuf, rxbuf_data, sizeof(rxbuf_data));
  process_start(&pcapng_line_process, NULL);
#if DEBUG == 3
  printd("Measurement/Debug Mode");
#endif
  /* todo: init pcap constants e.g., time */
#if FILEWRITE==1
  f = fopen("test2.pcapng", "wb");
#endif
}
/*---------------------------------------------------------------------------*/
