/**
 * @file spid.h
 *
 * @brief header file for spid.c.
 *
 * $Id: spid.h,v 1.3 2012/11/14 12:39:05 ele Exp $
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

/* Prevent double inclusion */
#ifndef _SPID_H
#define _SPID_H

/* === INCLUDES ============================================================= */
#include <avr/io.h>                 // MCU IO definitions

/* === MACROS / DEFINES ==================================================== */

/** Wait for SPI interrupt flag */
#define SPI_WAIT()                      while (!(SPSR & (1 << SPIF)))

/** Dummy value written in SPDR to retrieve data from it. */
#define SPI_DUMMY_VALUE                 (0x00)

/*
 * Interconnection between At25DF041@deRFnode/gateway and deRFmega128-xxx
 * SCK  = Pin13 = X1-11 = PB1
 * MOSI = Pin15 = X1-9  = PB2
 * MISO = Pin17 = X1-7  = PB3
 * #CS  = Pin35 = X2-12 = PE5
 */
//#define DDR_SPI                         (DDRB)
//#define SCK                             (PB1)
//#define MOSI                            (PB2)
//#define MISO                            (PB3)
//#define DDR_CS                          (DDRE)
//#define PORT_CS                         (PORTE)
//#define CS                              (PE5)

/*
 * Interconnection between At25010A@RCB128RFA1
 * SCK  = Pin13 = X1-11 = PB1
 * MOSI = Pin15 = X1-9  = PB2
 * MISO = Pin17 = X1-7  = PB3
 * #CS  = Pin35 = X2-12 = PG5
 */
#define DDR_SPI                         (DDRB)
#define SCK                             (PB1)
#define MOSI                            (PB2)
#define MISO                            (PB3)
#define DDR_CS                          (DDRG)
#define PORT_CS                         (PORTG)
#define CS                              (PG5)

/** Chip (slave) select */
#define CS_LO()                         (PORT_CS &= ~(1<<CS))
#define CS_HI()                         (PORT_CS |= (1<<CS))

/* === TYPES ================================================================ */

/* === PROTOTYPES =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the MCU as single SPI master device
 */
void SPI_MasterInit(void);

void SPI_MasterDeactivate(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* === INLINE FUNCTIONS ==================================================== */


#endif /* _SPID_H */

/* EOF */
