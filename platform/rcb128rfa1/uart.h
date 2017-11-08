#ifndef F_CPU
#warning "F_CPU war noch nicht definiert, wird nun nachgeholt mit 8000000"
#define F_CPU 8000000
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#define delay_us( us )   ( _delay_us( ( us ) ) )

#define BAUD 38400

#define UART_MAXSTRLEN 10

void uart_putc(unsigned char c);
