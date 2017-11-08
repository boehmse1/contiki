/*
 * Copyright (c) 2016 Sebastian Boehm (BTU-CS)
 *
 * Serial line PCAP interpreter
 * Note: Forwarded PCAP line event message only include PCAP Frame Header + Payload
 *
 */

#include "dev/pcap.h"
//#include "dev/rs232.h"
#include "dev/pcap-line.h"
#include "net/packetbuf.h"
#include "lib/ringbuf.h"
#include <stdio.h>

#define DEBUG 0
#if DEBUG
#define PRINTD(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTD(...)
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
  PCAP_IDLE = 0,
  PCAP_READ_GLOBAL_HDR = 1,
  PCAP_AWAIT_FRAME = 2,
  PCAP_READ_FRAME = 3,
};

static uint8_t state = PCAP_IDLE;
static struct ringbuf rxbuf;
static uint8_t rxbuf_data[BUFSIZE];

PROCESS(pcap_line_process, "Pcap serial driver");

process_event_t pcap_line_event_message;

/*
 * writes raw bytes to serial line
 */
void serial_write(const void *_ptr, uint16_t len)
{
	const uint8_t *ptr = _ptr;
	uint16_t i;

	for (i=0; i<len; i++ ) {
		putchar(*ptr++);
	}
}

/*---------------------------------------------------------------------------*/
/*
 * writes data with pcap frame header
 */
void pcap_write_frame(const void * data, uint16_t length)
{
	/* get actual time */
	uint32_t time;
	time = clock_time();

	/* create pcap data frame and write to serial line */
	if(length>0) {
		/* create pcap header */
		static pcap_pkthdr_s pcapheader;
		pcapheader.ts.ts_sec = (time - (time % CLOCK_SECOND)) / CLOCK_SECOND;
		pcapheader.ts.ts_usec = 1000000 * (time % CLOCK_SECOND) / CLOCK_SECOND;
		pcapheader.incl_len = length;
		pcapheader.orig_len = length;

		/* write frame header and data */
		serial_write(&pcapheader, sizeof(pcapheader));
		serial_write(&data, length);
	}
}

/*---------------------------------------------------------------------------*/
/*
 * writes raw bytes to serial line
 */
void pcap_write_hdr(uint8_t datalink)
{
	/* write global pcap header */
	pcap_file_header_s pcap_global = {PCAP_MAGIC_NUMBER, PCAP_VERSION_MAJOR, PCAP_VERSION_MINOR, 0, 0, PCAP_MAX_PACKET_SIZE, datalink};
	serial_write(&pcap_global, sizeof(pcap_global));
}

/*---------------------------------------------------------------------------*/
int
pcap_line_input_byte(unsigned char c)
{
  /* Add character */
  ringbuf_put(&rxbuf, c);

  /* Wake up consumer process */
  process_poll(&pcap_line_process);
  return 1;
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(pcap_line_process, ev, data)
{
  static uint8_t buf[BUFSIZE];
  static uint32_t pkt_len;
  static pcap_file_header_s header;
  static int ptr;
  static int hdr_ptr;
  static int c;

  PROCESS_BEGIN();

  pcap_line_event_message = process_alloc_event();

  /* debug print */
  PRINTD("PCAP_IDLE(RINGBUF %i)\n",ringbuf_size(&rxbuf));

  header.magic_number = PCAP_MAGIC_NUMBER;

  while (1) {
	  /* get character from buffer */
	  c = ringbuf_get(&rxbuf);

	  /* if buffer is empty */
	  if (c == -1) {
		  PROCESS_YIELD();

	  /* character received in buffer */
	  } else {
		  /* debug print */
		  PRINTD("PCAP_RECEIVED_CHARACTER %i", c);

		  /* switch state */
		  switch (state) {

		  case PCAP_IDLE:
			  /* try to parse pcap magic number */
			  if (c == PCAP_MAGIC_NUMBER_0 && hdr_ptr == 0) hdr_ptr++;
			  else if (c == PCAP_MAGIC_NUMBER_1 && hdr_ptr == 1) hdr_ptr++;
			  else if (c == PCAP_MAGIC_NUMBER_2 && hdr_ptr == 2) hdr_ptr++;
			  else if (c == PCAP_MAGIC_NUMBER_3 && hdr_ptr == 3) {
				  /* pcap magic number detected */
				  hdr_ptr++;
				  /* waiting for a pcap frame now */
				  state = PCAP_READ_GLOBAL_HDR;
			  }

			  break;

		  case PCAP_READ_GLOBAL_HDR:
			  /* read network */
			  hdr_ptr++;
			  if (hdr_ptr == 5) header.version_major |= (uint16_t) c;
			  else if (hdr_ptr == 6) header.version_major |= (uint16_t) c << 8;
			  else if (hdr_ptr == 7) header.version_minor |= (uint16_t) c;
			  else if (hdr_ptr == 8) header.version_minor |= (uint16_t) c << 8;
			  else if (hdr_ptr == 9) header.thiszone |= (uint32_t) c;
			  else if (hdr_ptr == 10) header.thiszone |= (uint32_t) c << 8;
			  else if (hdr_ptr == 11) header.thiszone |= (uint32_t) c << 16;
			  else if (hdr_ptr == 12) header.thiszone |= (uint32_t) c << 24;
			  else if (hdr_ptr == 13) header.sigfigs |= (uint32_t) c;
			  else if (hdr_ptr == 14) header.sigfigs |= (uint32_t) c << 8;
			  else if (hdr_ptr == 15) header.sigfigs |= (uint32_t) c << 16;
			  else if (hdr_ptr == 16) header.sigfigs |= (uint32_t) c << 24;
			  else if (hdr_ptr == 17) header.snaplen |= (uint32_t) c;
			  else if (hdr_ptr == 18) header.snaplen |= (uint32_t) c << 8;
			  else if (hdr_ptr == 19) header.snaplen |= (uint32_t) c << 16;
			  else if (hdr_ptr == 20) header.snaplen |= (uint32_t) c << 24;
			  else if (hdr_ptr == 21) header.network |= (uint32_t) c;
			  else if (hdr_ptr == 22) header.network |= (uint32_t) c << 8;
			  else if (hdr_ptr == 23) header.network |= (uint32_t) c << 16;
			  else if (hdr_ptr == 24) header.network |= (uint32_t) c << 24;
			  /* we have a complete pcap file header */
			  if (hdr_ptr == 24) {
				  /* debug print */
				  PRINTD("PCAP_GLOBAL_HEADER\n");
				  /* waiting for a pcap frame now */
				  state = PCAP_AWAIT_FRAME;
				  /* reset the header pointer */
				  hdr_ptr = 0;
			  }

			  break;

		  case PCAP_AWAIT_FRAME:
			  /* try to parse pcap magic number */
			  if (c == PCAP_MAGIC_NUMBER_0 && hdr_ptr == 0) hdr_ptr++;
			  else if (c == PCAP_MAGIC_NUMBER_1 && hdr_ptr == 1) hdr_ptr++;
			  else if (c == PCAP_MAGIC_NUMBER_2 && hdr_ptr == 2) hdr_ptr++;
			  else if (c == PCAP_MAGIC_NUMBER_3 && hdr_ptr == 3) {
				  /* pcap magic number detected */
				  hdr_ptr++;
				  /* waiting for a pcap frame now */
				  state = PCAP_READ_GLOBAL_HDR;
			  }
			  /* parse pcap frame */
			  else {
				  /* copy character to buffer */
				  if (ptr < BUFSIZE-1) {
					  buf[ptr++] = (uint8_t) c;
					  hdr_ptr++;
				  }
				  /* parse payload length from record header */
				  if (hdr_ptr == 13) pkt_len |= (uint32_t) c;
				  else if (hdr_ptr == 14) pkt_len |= (uint32_t) c << 8;
				  else if (hdr_ptr == 15) pkt_len |= (uint32_t) c << 16;
				  else if (hdr_ptr == 16) pkt_len |= (uint32_t) c << 24;
				  /* we have a complete record header */
				  if (hdr_ptr == 16) {
					  /* debug print */
					  PRINTD("PCAP_FRAME(%lu)\n", (long unsigned int) pkt_len);
					  /* parse the packet payload */
					  state = PCAP_READ_FRAME;
					  /* reset the header pointer */
					  hdr_ptr = 0;
				  }
			  }
			  break;

		  case PCAP_READ_FRAME:
			  /* copy character to buffer */
			  if (ptr < BUFSIZE-1 && ptr < pkt_len + PCAP_FRAME_HEADER_LEN) {
				  buf[ptr++] = (uint8_t) c;
			  }
			  /* last byte of frame */
			  if (ptr == pkt_len + PCAP_FRAME_HEADER_LEN) {
				  /* Broadcast event */
				  /* TODO: Übergeben von &header, &frame, &payload anstelle von buf */
				  process_post(PROCESS_BROADCAST, pcap_line_event_message, buf);
				  /* debug print */
				  PRINTD("PCAP_WRITE_BYTES(%i)\n", ptr);
				  /* back to wait frame */
				  state = PCAP_AWAIT_FRAME;
				  /* reset the packet pointer */
				  ptr = 0;
				  /* reset the packet length */
				  pkt_len = 0;
			  }
			  break;
		  }
	  }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
void
pcap_line_init(void)
{
  ringbuf_init(&rxbuf, rxbuf_data, sizeof(rxbuf_data));
  process_start(&pcap_line_process, NULL);
}
/*---------------------------------------------------------------------------*/
