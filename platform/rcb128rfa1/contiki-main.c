/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * Initialization of RCB128RFA1 (ATmega128RFA1)
 *
 */

#define DEBUG 0
#if defined(DEBUG) && DEBUG == 1
#define PRINTD(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTD(...) /* Don't do anything in release builds */
#endif

#define SENSTERMBOARD 1			// stdout on sensor terminal board (STB)
#define SLIP_INPUT_ENABLED 0	// slip input handler (only RS232_PORT_0)
#define PCAP_INPUT_ENABLED 0	// pcap input handler (only RS232_PORT_0)
#define PCAPNG_INPUT_ENABLED 1	// pcapng input handler (only RS232_PORT_0)

#include <avr/signature.h>
#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>

#include "params.h"
//#include "radio/rf230bb/rf230bb.h"
#include "net/mac/frame802154.h"
#include "net/mac/framer-802154.h"

#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"

#include <dev/watchdog.h>
#include "dev/rs232.h"
#include "dev/serial-line.h"
#include "dev/slip-line.h"
#include "dev/pcap-line.h"
#include "dev/pcapng-line.h"
#include "dev/slip.h"

#include "flash_rcb128rfa1.h"
#include "usb-stb-128rfa1.h"
#include "leds.h"

/*-----------------------get mac address from eeprom-----------------------*/
void get_mac_from_eeprom(linkaddr_t *addr)
{
	/* configure flash device */
	AT25_Init();

	/* single eeprom read buffer */
	uint8_t buf[32];   	// read buffer

	//WAIT!!! temp
	AT25_ReadArray(EEPROM_BOARD_MAC_ADDRESS, buf, 1);

	/* read MAC addr */
	uint8_t j;
	AT25_ReadArray(EEPROM_BOARD_MAC_ADDRESS, buf, 8);
	for (j=sizeof(linkaddr_t); j>0; j--){
		addr->u8[j-1] = buf[j-1];
	}

	AT25_Disable();

	PRINTD("MAC address ");
	uint8_t i;
	for (i=sizeof(linkaddr_t); i>0; i--){
		PRINTD("%x:",addr->u8[i-1]);
	}
	PRINTD("\n");
}

/*-------------------------Low level initialization------------------------*/
void init(void)
{
  watchdog_init();
  watchdog_start();

  /* Init first rs232 port (UART0) for regular use */
  rs232_init(RS232_PORT_0, USART_BAUD_57600, USART_PARITY_NONE | USART_STOP_BITS_1 | USART_DATA_BITS_8);

  /* Init second rs232 port (UART1) for use on STB (debugging) */
#if SENSTERMBOARD
	/* Second rs232 port (UART1) for debugging */
	rs232_init(RS232_PORT_1, USART_BAUD_57600,USART_PARITY_NONE | USART_STOP_BITS_1 | USART_DATA_BITS_8);
	rs232_redirect_stdout(RS232_PORT_1);
	/* Init USB-Support from Sensor-Terminal-Board */
	usb_init();
#else
	/* First rs232 port (UART0) for debugging */
	rs232_redirect_stdout(RS232_PORT_0);
#endif

/* line input processes */
#if SLIP_INPUT_ENABLED
	rs232_set_input(RS232_PORT_0, slip_line_input_byte);
#elif PCAP_INPUT_ENABLED
	rs232_set_input(RS232_PORT_0, pcap_line_input_byte);
#elif PCAPNG_INPUT_ENABLED
	rs232_set_input(RS232_PORT_0, pcapng_line_input_byte);
#else
	rs232_set_input(RS232_PORT_0, serial_line_input_byte);
#endif

  clock_init();

  /* rtimers needed for radio cycling */
  rtimer_init();

  /* Initialize process subsystem */
  process_init();

  /* line input processes */
#if SLIP_INPUT_ENABLED
  slip_line_init();
#elif PCAP_INPUT_ENABLED
  pcap_line_init();
#elif PCAPNG_INPUT_ENABLED
  pcapng_line_init();
#else
  serial_line_init();
#endif

  /* etimers must be started before ctimer_init */
  process_start(&etimer_process, NULL);
  ctimer_init();

  /* Start radio and radio receive process */
  NETSTACK_RADIO.init();

  /* Set addresses BEFORE starting tcpip process */
  linkaddr_t addr;
 
  //linkaddr_set_node_addr(&addr);
  //get_mac_from_eeprom(&addr);

  rf230_set_pan_addr(params_get_panid(),params_get_panaddr(),(uint8_t *)&addr.u8);
  rf230_set_channel(params_get_channel());
  rf230_set_txpower(params_get_txpower());

  /* entering promiscuous mode */
  //rf230_set_prom_mode();

  /* set symbol counter for rx frame time stamping */
  //rf230_enable_sdf_time_stamping();

  /* should be zero */
  //rf230_get_sdf_time();

  /* Initialize stack protocols */
  queuebuf_init();
  NETSTACK_RDC.init();

  /* Autostart other processes */
  autostart_start(autostart_processes);
}

/*-------------------------------------------------------------------------*/
/*------------------------- Main Scheduler loop----------------------------*/
/*-------------------------------------------------------------------------*/
int
main(void)
{
  init();

  while(1) {
    process_run();
    watchdog_start();
  }
  return 0;
}
