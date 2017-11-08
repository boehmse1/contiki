/*
 * Copyright (c) 2016 Sebastian Boehm (BTU-CS)
 *
 * GPIOs for RCB128RFA1 on RoSeNetPanel (see documentation RM_Adapter)
 */

#ifndef __GPIOS_H__
#define __GPIOS_H__

#include "contiki.h"

void gpios_high(unsigned char);
void gpios_low(unsigned char);
void gpios_set_all_input(void);
void gpios_set_all_output(void);
unsigned char gpios_get_state(unsigned char gpio);

#endif /* __GPIOS_H__ */
