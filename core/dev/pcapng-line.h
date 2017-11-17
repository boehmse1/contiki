/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * Serial line PCAPNG interpreter
 * Note: Forwarded PCAPNG line event messages represents PCAPNG blocks
 *
 */
#ifndef __PCAPNG_LINE_H__
#define __PCAPNG_LINE_H__

#include "contiki.h"
#include "dev/pcap.h"
#include "dev/pcapng.h"

/**
 * PCAPNG events posted when a PCAPNG block has been received.
 *
 * This events are posted when an entire PCAPNG block has been received
 * from the serial port. A data pointer to the incoming line of input
 * is sent together with the event.
 */
extern process_event_t pcapng_event_shb;
extern process_event_t pcapng_event_idb;
extern process_event_t pcapng_event_epb;
//extern process_event_t pcapng_event_cb;

void pcapng_line_write(const void *_ptr, uint32_t len);

void pcapng_line_write_shb(void);

void pcapng_line_write_idb(uint16_t datalink, uint32_t snaplen);

void pcapng_line_write_epb(uint32_t interface, pcap_timeval_s *ts, const void * data, uint32_t length);

void pcapng_line_write_cb(const void * data, uint32_t length);

void pcapng_line_read_shb(uint8_t * ptr, pcapng_section_header_block_s * section);

void pcapng_line_read_idb(uint8_t * ptr, pcapng_interface_description_block_s * interface);

void pcapng_line_read_epb(uint8_t * ptr, pcapng_enhanced_packet_block_s * packet);

/**
 * Get one byte of input from the serial driver.
 *
 * This function is to be called from the actual RS232 driver to get
 * one byte of serial data input.
 *
 * For systems using low-power CPU modes, the return value of the
 * function can be used to determine if the CPU should be woken up or
 * not. If the function returns non-zero, the CPU should be powered
 * up. If the function returns zero, the CPU can continue to be
 * powered down.
 *
 * \param c The data that is received.
 *
 * \return Non-zero if the CPU should be powered up, zero otherwise.
 */
int pcapng_line_input_byte(unsigned char c);

void pcapng_line_init(void);

PROCESS_NAME(pcapng_line_process);

#endif /* __PCAPNG_LINE_H__ */
