/*
 * Copyright (c) 2015 Sebastian Boehm (BTU-CS)
 */

#include "leds.h"

/* LED1 an RCB on */
void leds_on(unsigned char led) {

	if (led==1) {
		/* RCB LED1 on */
		DDRE |= ( 1 << PE2 );
		PORTE &= ~( 1 << PE2 );
	} else if (led==2) {
		/* RCB LED2 on */
		DDRE |= ( 1 << PE3 );
		PORTE &= ~( 1 << PE3 );
	} else if (led==3) {
		/* RCB LED3 on */
		DDRE |= ( 1 << PE4 );
		PORTE &= ~( 1 << PE4 );
	}
}

/* LED1 an RCB off */
void leds_off(unsigned char led) {

	if (led==1) {
		/* RCB LED1 off */
		DDRE |= ( 1 << PE2 );
		PORTE |= ( 1 << PE2 );
	} else if (led==2) {
		/* RCB LED2 on */
		DDRE |= ( 1 << PE3 );
		PORTE |= ( 1 << PE3 );
	} else if (led==3) {
		/* RCB LED3 on */
		DDRE |= ( 1 << PE4 );
		PORTE |= ( 1 << PE4 );
	}
}
