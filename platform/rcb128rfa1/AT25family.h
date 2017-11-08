/**
 * @file AT25family.h
 *
 * @brief AT25-family flash device constants
 *
 * This file contains command definitions and register bitmasks for AT25
 * flash devices.
 *
 * $Id: AT25family.h,v 1.2 2012/11/14 12:39:05 ele Exp $
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
#ifndef _AT25_FAMILY_H
#define _AT25_FAMILY_H

/* === INCLUDES ============================================================ */

/* === MACROS / DEFINES ==================================================== */

/** dataflash command definitions, generic for whole AT25-family */
#define AT25_READ_ARRAY_CMD_1               (0x0B)
#define AT25_READ_ARRAY_CMD_2               (0x03)

#define AT25_BLOCK_ERASE_4K_CMD             (0x20)
#define AT25_BLOCK_ERASE_32K_CMD            (0x52)
#define AT25_BLOCK_ERASE_64K_CMD            (0xD8)
#define AT25_CHIP_ERASE_CMD_1               (0x60)
#define AT25_CHIP_ERASE_CMD_2               (0xC7)
#define AT25_BYTE_PAGE_PROGRAM_CMD          (0x02)
#define AT25_SEQ_PROGRAMMING_MODE_CMD_1     (0xAD)
#define AT25_SEQ_PROGRAMMING_MODE_CMD_2     (0xAF)

#define AT25_WRITE_ENABLE_CMD               (0x06)
#define AT25_WRITE_DISABLE_CMD              (0x04)
#define AT25_SECTOR_PROTECT_CMD             (0x36)
#define AT25_SECTOR_UNPROTECT_CMD           (0x39)
#define AT25_READ_SECTOR_PROTECT_REG_CMD    (0x3C)
#define AT25_READ_STATUS_REGISTER_CMD       (0x05)
#define AT25_WRITE_STATUS_REGISTER_CMD      (0x01)

#define AT25_READ_DEVICE_IDS_CMD            (0x9F)
#define AT25_DEEP_POWER_DOWN_CMD            (0xB9)
#define AT25_DEEP_POWER_RESUME_CMD          (0xAB)

/** SPI status register bit definitions */
#define AT25_SR_SPRL_MASK                   (0x80)  // sector protection register lock
#define AT25_SR_SPM_MASK                    (0x40)  // sequential programming mode
#define AT25_SR_EPE_MASK                    (0x20)  // erase/program error
#define AT25_SR_WPP_MASK                    (0x10)  // write protection pin
#define AT25_SR_SWP_MASK                    (0x0C)  // software protection
#define AT25_SR_WEL_MASK                    (0x02)  // write enable
#define AT25_SR_RDBUSY_MASK                 (0x01)  // ready/busy status

#define AT25_SR_SPRL_UNLOCKED_MASK          (0<<7)  // sector protection registers are unlocked
#define AT25_SR_SPRL_LOCKED_MASK            (1<<7)  // sector protection registers are locked

#define AT25_SR_SPM_BYTE_PAGE_MASK          (0<<6)  // byte/page programming mode
#define AT25_SR_SPM_SEQUENTIAL_MASK         (1<<6)  // sequential programming mode

#define AT25_SR_EPE_SUCCESS_MASK            (0<<5)  // erase or program operation was successful
#define AT25_SR_EPE_ERROR_MASK              (1<<5)  // erase or program error detected

#define AT25_SR_WPP_NOTASSERTED_MASK        (0<<4)  // write protect signal is not asserted
#define AT25_SR_WPP_ASSERTED_MASK           (1<<4)  // write protect signal is asserted

#define AT25_SR_SWP_PROTNONE_MASK           (0<<2)  // no sector is software protected
#define AT25_SR_SWP_PROTSOME_MASK           (1<<2)  // some sectors are software protected
#define AT25_SR_SWP_PROTALL_MASK            (3<<2)  // all sectors are software protected

#define AT25_SR_WEL_DISABLED_MASK           (0<<1)  // device is not write enabled
#define AT25_SR_WEL_ENABLED_MASK            (1<<1)  // device is write enabled

#define AT25_SR_RDYBSY_READY_MASK           (0<<0)  // device is ready
#define AT25_SR_RDYBSY_BUSY_MASK            (1<<0)  // device is busy with internal operations


/** device specifics for members of AT25 Family */

/** sector starting addresses */
#define AT25DF021_SECTOR_0                  (0x000000)
#define AT25DF021_SECTOR_1                  (0x010000)
#define AT25DF021_SECTOR_2                  (0x020000)
#define AT25DF021_SECTOR_3                  (0x030000)

#define AT25DF021_SECTORS { AT25DF021_SECTOR_0, AT25DF021_SECTOR_1, AT25DF021_SECTOR_2, \
                            AT25DF021_SECTOR_3 }

#define AT25DF041_SECTOR_0                  (0x000000)
#define AT25DF041_SECTOR_1                  (0x010000)
#define AT25DF041_SECTOR_2                  (0x020000)
#define AT25DF041_SECTOR_3                  (0x030000)
#define AT25DF041_SECTOR_4                  (0x040000)
#define AT25DF041_SECTOR_5                  (0x050000)
#define AT25DF041_SECTOR_6                  (0x060000)
#define AT25DF041_SECTOR_7                  (0x070000)
#define AT25DF041_SECTOR_8                  (0x078000)
#define AT25DF041_SECTOR_9                  (0x07A000)
#define AT25DF041_SECTOR_A                  (0x07C000)

#define AT25DF041_SECTORS { AT25DF041_SECTOR_0, AT25DF041_SECTOR_1, AT25DF041_SECTOR_2, \
                            AT25DF041_SECTOR_3, AT25DF041_SECTOR_4, AT25DF041_SECTOR_5, \
                            AT25DF041_SECTOR_6, AT25DF041_SECTOR_7, AT25DF041_SECTOR_8, \
                            AT25DF041_SECTOR_9, AT25DF041_SECTOR_A }

/* === TYPES =============================================================== */

/* === PROTOTYPES ========================================================== */

/* === INLINE FUNCTIONS ==================================================== */

#endif /* _AT25_FAMILY_H */

/* EOF */
