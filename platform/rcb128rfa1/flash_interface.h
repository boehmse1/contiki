/**
 * @file flash_interface.h
 *
 * @brief header file for flash.c.
 *
 * This file describes an API for AT25-family Flash devices, which was
 * developed from scratch. Each given reference relates to the AT25DF041A
 * datasheet available at:
 * http://www.atmel.com/dyn/resources/prod_documents/doc3668.pdf
 *
 * $Id: flash_interface.h,v 1.4 2012/11/14 12:39:05 ele Exp $
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

/* prevent double inclusion */
#ifndef _FLASH_INTERFACE_H
#define _FLASH_INTERFACE_H

/* === INCLUDES ============================================================ */
#include <inttypes.h>
#include <stdbool.h>
#include "AT25family.h"     // commands and register bits/masks

/* === MACROS / DEFINES ==================================================== */

/* === TYPES =============================================================== */

/** sector protection status */
typedef enum
{
    SECTOR_UNPROTECTED = 0x00,
    SECTOR_PROTECTED
} protect_status_t;

/** blocksize for erase operations */
typedef enum
{
    BLOCKSIZE_4K,
    BLOCKSIZE_32K,
    BLOCKSIZE_64K
} blocksize_t;

/** status code for read, write and erase operations */
typedef enum
{
    FL_SUCCESS = 0x00,
    FL_WRONG_WORKING_MODE,
    FL_PROTECTED,
    FL_PAGE_BORDER_REACHED,
    FL_NOT_ERASED,
    FL_ZERO_LEN,
    FL_ERROR
} flash_status_t;


/* === PROTOTYPES ========================================================== */

// --- generic interface -----------------------------------------------------
flash_status_t AT25_Init(void);
flash_status_t AT25_Disable(void);
bool AT25_Identify(void);

// --- read commands (chapter 7) ---------------------------------------------
void AT25_ReadArray(uint32_t addr, uint8_t *pData, uint16_t len);

// --- program and erase commands (chapter 8) --------------------------------
flash_status_t AT25_ProgramPage(uint32_t addr, uint8_t *pData, uint16_t len);
flash_status_t AT25_SequentialProgramStart(uint32_t addr, uint8_t data);
flash_status_t AT25_SequentialProgramByte(uint8_t data);
void AT25_SequentialProgramLeave(void);
flash_status_t AT25_BlockErase(blocksize_t blksz, uint32_t addr);
flash_status_t AT25_ChipErase(void);

// --- protection commands and features (chapter 9) --------------------------
void AT25_WriteEnable(void);
void AT25_WriteDisable(void);
flash_status_t AT25_ProtectSector(uint32_t addr);
flash_status_t AT25_UnprotectSector(uint32_t addr);
void AT25_GlobalProtect(void);
void AT25_GlobalUnprotect(void);
void AT25_SetSPRL(void);
void AT25_ClearSPRL(void);
protect_status_t AT25_ReadProtectionRegister(uint32_t addr);

// --- status register commands (chapter 10) ---------------------------------
uint8_t AT25_ReadStatusRegister(void);
void AT25_WriteStatusRegister(uint8_t data);

// --- other commands and functions (chapter 11) -----------------------------
void AT25_ReadJedecID(uint32_t *pData);
void AT25_DeepPowerDown(void);
void AT25_DeepPowerDownResume(void);

// --- helper functions ------------------------------------------------------
void AT25_ShowSectorProtectionState(void);
void AT25_InterpretStatusRegister(uint8_t data);
void dump_data(uint8_t* pData, uint16_t len, char* title);

/* === INLINE FUNCTIONS ==================================================== */

#endif /* _FLASH_INTERFACE_H */

/* EOF */
