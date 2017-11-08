/*
 * Copyright (c) 2015 Sebastian Boehm (BTU-CS)
 *
 * Slip line parser
 *
 */

#include "dev/rs232.h"
#include "dev/slip-line.h"
#include "lib/ringbuf.h"
#include <stdio.h>

#ifdef SLIP_LINE_CONF_BUFSIZE
#define BUFSIZE SLIP_LINE_CONF_BUFSIZE
#else /* SLIP_LINE_CONF_BUFSIZE */
#define SLIP_BUFSIZE 128
#endif /* SLIP_LINE_CONF_BUFSIZE */

#if (BUFSIZE & (BUFSIZE - 1)) != 0
#error SLIP_LINE_CONF_BUFSIZE must be a power of two (i.e., 1, 2, 4, 8, 16, 32, 64, ...).
#error Change SLIP_LINE_CONF_BUFSIZE in contiki-conf.h.
#endif

#define SLIP_END     0300
#define SLIP_ESC     0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

#define SLIP_MAX_PACKET_SIZE 128

/* slip states */
enum {
  STATE_IDLE = 0,
  STATE_INPUT = 1,
  STATE_ESC = 2,
};

static uint8_t state = STATE_IDLE;
static struct ringbuf rxbuf;
static uint8_t rxbuf_data[SLIP_BUFSIZE];

PROCESS(slip_line_process, "Slip serial driver");

process_event_t slip_line_event_message;

/*---------------------------------------------------------------------------*/
int
slip_line_input_byte(unsigned char c)
{
  static uint8_t overflow = 0; /* Buffer overflow: ignore until SLIP_END */

  if(!overflow) {
    /* Add character */
    if(ringbuf_put(&rxbuf, c) == 0) {
      /* Buffer overflow: ignore the rest of the line */
      overflow = 1;
    }
  } else {
    /* Buffer overflowed:
     * Only (try to) add terminator characters, otherwise skip */
	printf("error: slip input buffer overflowed!");
    if(c == SLIP_END && ringbuf_put(&rxbuf, c) != 0) {
      overflow = 0;
    }
  }

  /* Wake up consumer process */
  process_poll(&slip_line_process);
  return 1;
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(slip_line_process, ev, data)
{
  static uint8_t buf[SLIP_BUFSIZE];
  static int ptr;
  static int c;

  PROCESS_BEGIN();

  slip_line_event_message = process_alloc_event();
  ptr = 0;

  while (1) {
	  /* get character from buffer */
	  c = ringbuf_get(&rxbuf);

	  /* if buffer is empty */
	  if (c == -1) {
		  PROCESS_YIELD();

	  /* character received in buffer */
	  } else {

		  /* switch state */
		  switch (state) {

		  case STATE_IDLE:
			  /* start of frame */
			  if (c == SLIP_END) {
				  /* space for packetsize */
				  ptr++;
				  /* state changed to input */
				  state = STATE_INPUT;
			  }
			  break;

		  case STATE_INPUT:
			  /* esc character */
			  if (c == SLIP_ESC) {
				  /*state changed to esc */
				  state = STATE_ESC;

			  /* end of frame */
			  } else if (c == SLIP_END) {
				  /* copy packetsize at first position: len | packetdata */
				  buf[0] = (char) --ptr;
				  /* Broadcast event */
				  process_post(PROCESS_BROADCAST, slip_line_event_message, buf);
				  /* reset packet pointer */
				  ptr = 0;
				  /* wait for next packets */
				  state = STATE_IDLE;

			  /* copy character to buffer */
			  } else {
				  if (ptr < SLIP_BUFSIZE-1) {
					  buf[ptr++] = (uint8_t) c;
				  }
			  }
			  break;

		  case STATE_ESC:
			  /* esc_end character */
			  if (c == SLIP_ESC_END) {
				  c = SLIP_END;

			  /* esc_esc character */
			  } else if (c == SLIP_ESC_ESC) {
				  c = SLIP_ESC;

			  /* it's not a slip packet */
			  } else {
				  /* reset packet pointer */
				  ptr = 0;
				  /* wait for next packets */
				  state = STATE_IDLE;
				  break;
			  }

			  /* copy character to buffer */
			  if (ptr < SLIP_BUFSIZE-1) {
				  buf[ptr++] = (uint8_t) c;
			  }

			  /* back to input */
			  state = STATE_INPUT;
			  break;
		  }
	  }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
void
slip_line_init(void)
{
  ringbuf_init(&rxbuf, rxbuf_data, sizeof(rxbuf_data));
  process_start(&slip_line_process, NULL);
}
/*---------------------------------------------------------------------------*/
