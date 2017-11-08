/*
 * Copyright (c) 2016 Sebastian Boehm (BTU-CS)
 *
 * GPIOs for RCB128RFA1 on RoSeNetPanel (see documentation RM_Adapter)
 */


#include "gpios.h"

/* GPIOs on RCB high */
void gpios_high(unsigned char gpio) {

	if (gpio==1) PORTD |= ( 1 << PD0 );
	else if (gpio==2) PORTD |= ( 1 << PD1 );
	else if (gpio==3) PORTD |= ( 1 << PD4 );
	else if (gpio==4) PORTD |= ( 1 << PD6 );
}

/* GPIOs on RCB low */
void gpios_low(unsigned char gpio) {

	if (gpio==1) PORTD &= ~( 1 << PD0 );
	else if (gpio==2) PORTD &= ~( 1 << PD1 );
	else if (gpio==3) PORTD &= ~( 1 << PD4 );
	else if (gpio==4) PORTD &= ~( 1 << PD6 );
}

/* GPIOS input */
void gpios_set_all_input(void) {
	DDRD  &= ~( 1 << PD0 );
	DDRD  &= ~( 1 << PD1 );
	DDRD  &= ~( 1 << PD4 );
	DDRD  &= ~( 1 << PD6 );
}

/* GPIOS output */
void gpios_set_all_output(void) {
	DDRD  |= ( 1 << PD0 );
	DDRD  |= ( 1 << PD1 );
	DDRD  |= ( 1 << PD4 );
	DDRD  |= ( 1 << PD6 );
}

/* GPIOs on RCB low */
unsigned char gpios_get_state(unsigned char gpio) {

	unsigned char state = 0;

	if (gpio==1) return (PIND & (1<<PIND1));
	else if (gpio==2) return (PIND & (1<<PIND1));
	else if (gpio==3) return (PIND & (1<<PIND1));
	else if (gpio==4) return (PIND & (1<<PIND1));
	else return 0xff;
}
