/**
 * @file flash_rcb128rfa1.h
 *
 * @brief definitions for rc128rfa1 AT25010A eeprom data.
 * Atmel AVR2044: RCB128RFA1 – Hardware User Manual
 *
 * Copyright (c) 2017, Sebastian Boehm (BTU-CS)
 *
 */

#include "flash_interface.h"

/* prevent double inclusion */
#ifndef _FLASH_RCB128RFA1_H
#define _FLASH_RCB128RFA1_H

/** ID EEPROM mapping - Atmel AVR2044, page 10f */
typedef enum
{
    EEPROM_BOARD_MAC_ADDRESS = 0x00,
    EEPROM_BOARD_SERIAL_NUM = 0x08,
    EEPROM_BOARD_FAM = 0x10,
    EEPROM_BOARD_REV = 0x11,
    EEPROM_BOARD_FEATURE = 0x14,
    EEPROM_BOARD_CAL_OSC_16MHZ = 0x15,
    EEPROM_BOARD_CAL_RC_3_6V = 0x16,
    EEPROM_BOARD_CAL_RC_2_0V = 0x17,
    EEPROM_BOARD_ANT_GAIN = 0x18,
    EEPROM_BOARD_NAME = 0x20,
    EEPROM_CRC = 0x3E
} eeprom_id;

#endif /* _FLASH_RCB128RFA1_H */

/* EOF */
