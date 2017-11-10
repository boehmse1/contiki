#include "uart.h"


/* LED1 toggle an RCB */
void led_toggle(void) {

	/* RCB LEDs on/off */
	DDRE |= ( 1 << PE2 );
	PORTE &= ~( 1 << PE2 );

	delay_us(100000);

	PORTE |= ( 1 << PE2 );
}

/* LEDs toggle an RCB */
void leds_toggle(void) {

	/* RCB LEDs on/off */
	DDRE |= ( 1 << PE2 );
	PORTE &= ~( 1 << PE2 );

	delay_us(1000000);

	PORTE |= ( 1 << PE2 );

	delay_us(1000000);

	DDRE |= ( 1 << PE3 );
	PORTE &= ~( 1 << PE3 );

	delay_us(1000000);

	PORTE |= ( 1 << PE3 );

	delay_us(1000000);
	DDRE |= ( 1 << PE4 );
	PORTE &= ~( 1 << PE4 );

	delay_us(1000000);

	PORTE |= ( 1 << PE4 );
}

/* init uart */
void uart_init()
{
	uint16_t ubrr = (uint16_t) ((uint32_t) F_CPU/(16*BAUD) - 1);
	UBRR0H = (uint8_t) (ubrr>>8);
	UBRR0L = (uint8_t) (ubrr);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // asynchron 8N1
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // UART RX, TX und RX Interrupt

	// Flush Receive-Buffer (entfernen evtl. vorhandener ungueltiger Werte)
	do
	{
		UDR0;
	}
	while (UCSR0A & (1 << RXC0));

}

/* Zeichen empfangen */
uint8_t uart_getc(void)
{
	while (!(UCSR0A & (1<<RXC0)))   // warten bis Zeichen verfuegbar
	;
	return UDR0;                   // Zeichen aus UDR an Aufrufer zurueckgeben
}

/* Zeichen senden */
void uart_putc(unsigned char c)
{
	while (!(UCSR0A & (1<<UDRE0)))  /* warten bis Senden moeglich */
	{
	}

	UDR0 = c;                      /* sende Zeichen */
}


/* puts ist unabhaengig vom Controllertyp */
void uart_puts(char *s)
{
	while (*s)
	{   /* so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)" */
		uart_putc(*s);
		s++;
	}

	/* RCB LEDs on/off */
	led_toggle();
}

/* get string */
void uart_gets( char* buffer, uint8_t maxLen )
{
	uint8_t nextChar;
	uint8_t stringLen = 0;

	nextChar = uart_getc();         // Warte auf und empfange das n�chste Zeichen

	// Sammle solange Zeichen, bis:
	// * entweder das String Ende Zeichen kam
	// * oder das aufnehmende Array voll ist
	while( nextChar != '\n' && stringLen < maxLen - 1 ) {
		*buffer++ = nextChar;
		stringLen++;
		nextChar = uart_getc();
	}

	// Noch ein '\0' anh�ngen um einen Standard
	// C-String daraus zu machen
	*buffer = '\0';
}
