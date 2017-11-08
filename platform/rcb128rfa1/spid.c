/**
 * @file spid.c
 *
 * @brief  Implementation of a generic SPI driver for AVR-based MCUs.
 *
 * $Id: spid.c,v 1.2 2012/11/14 12:39:05 ele Exp $
 *
 */
/*
 * @author    dresden elektronik ingenieurtechnik gmbh: http://www.dresden-elektronik.de
 * @author    Support email: wireless@dresden-elektronik.de
 *
 * Copyright (c) 2011, dresden elektronik ingenieurtechnik gmbh. All rights reserved.
 *
 * Licensed under dresden elektronik's Limited License Agreement --> deEULA.txt
 */

/* === INCLUDES ============================================================ */
#include "spid.h"

/* === TYPES =============================================================== */

/* === MACROS / DEFINES ==================================================== */

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Initialize the MCU as single SPI master device
 */
void SPI_MasterInit(void)
{
    /* Set PB0 (AVRs original SS Pin) to H level to ensure SPI master mode */
    DDRB |= (1<<PB0);
    PORTB |= (1<<PB0);

    /* Enable the SPI, MCU=SPI master, FOSC/16 */
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);

    /*
     * Select the serial clock SCK to be (FOSC/4) and double it
     * (i.e. if CPU runs at 8MHz, SPI clock will be 4MHz;
     * the At25D041 flash supports at least 33MHz)
     */
    //SPCR &= ~(_BV(SPR0) | _BV(SPR1));
    //SPCR &= _BV(SPR0) | ~(_BV(SPR1));
    //SPSR = _BV(SPI2X);
}

void SPI_MasterDeactivate(void)
{
	DDRB &= ~(1<<PB0);
	PORTB &= ~(1<<PB0);
}

/* EOF */
