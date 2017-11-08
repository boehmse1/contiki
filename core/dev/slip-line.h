/*
 * Copyright (c) 2015 Sebastian Boehm (BTU-CS)
 *
 * Slip line parser
 *
 */

#ifndef __SLIP_LINE_H__
#define __SLIP_LINE_H__

#include "contiki.h"

extern process_event_t slip_line_event_message;

int slip_line_input_byte(unsigned char c);

void slip_line_init(void);

PROCESS_NAME(slip_line_process);

#endif /* __SLIP_LINE_H__ */
