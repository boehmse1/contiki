/**
 * @file flash.c
 *
 * @brief  Implementation of an AT25 serial flash library
 *
 * Developed for At25DF041A Flash devices, it should be also working for
 * related devices from same family.
 *
 * $Id: flash.c,v 1.2 2012/11/14 12:39:05 ele Exp $
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
#include <stdio.h>
#include <avr/io.h>

#include "flash_interface.h"        // common library interface
#include "AT25family.h"             // command definitions, ...
#include "spid.h"                   // SPI driver

/* === TYPES =============================================================== */

// Describes a serial firmware flash device parameters.
typedef struct _At25Desc {

   /// Device string name.
   const char *name;
   /// JEDEC ID of device.
   const uint32_t jedecId;
   /// Size of device in bytes.
   const uint32_t size;
   /// Size of one page in bytes.
   const uint32_t pageSize;
   /// _no_ blocksize since differing over memory space
   /// sector count
   const uint32_t sectorCount;
   /// array with offsets for sector start addresses
   const uint32_t* sectorStartingAddresses;

} At25Desc;

/* === MACROS / DEFINES ==================================================== */

// Nearly each operation requires setting the 'Write Enable Latch'-Bit in the
// Status Register which mostly gets automatically reset after performing the
// respective command. For maximum safety, setting WEL should be done manually
// to prevent accidental data loss; wherever required this may be done
// automatically by uncommenting the following macro.
#define FORCE_WRITE_ENABLE

#ifdef FORCE_WRITE_ENABLE
#define AT25_FORCE_WEL()   AT25_ForceWEL()
#else
#define AT25_FORCE_WEL()   {}
#endif


#define DECLARE_SECTORS(x)      static const uint32_t sectors_##x[] = x
#define USE_SECTORS(x)          &sectors_##x[0]

/* === GLOBALS ============================================================= */
static At25Desc* pAt25Desc; // points to a device description data structure
static uint8_t regval;  	// used for evaluating the status register
static uint8_t buf[256];	// common buffer

// generate a variable per each known flash device
DECLARE_SECTORS(AT25DF021_SECTORS);
DECLARE_SECTORS(AT25DF041_SECTORS);

// Array of recognized serial dataflash chips.
static const At25Desc at25Devices[] = {
    // name,        Jedec ID,   size,               page size  , sectors, sector Offset
    {"AT25DF021"  , 0x0000431F, 256*(uint16_t)1000, 256        , 3      , USE_SECTORS(AT25DF021_SECTORS) },
    {"AT25DF041"  , 0x0001441F, 512*(uint16_t)1000, 256        , 11     , USE_SECTORS(AT25DF041_SECTORS) }
};

/* === PROTOTYPES ========================================================== */
static void AT25_InterfaceInit(void);
static At25Desc* AT25_FindDevice(uint32_t jedecId);
static inline void AT25_WriteByte(uint8_t data);
#ifdef FORCE_WRITE_ENABLE
  static void AT25_ForceWEL(void);
#endif
static void AT25_WaitReady();


/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Initialize the SPI (interface/driver) and also the serial flash device
 */
flash_status_t AT25_Init(void)
{
    AT25_InterfaceInit();
    SPI_MasterInit();
    return FL_SUCCESS;
}


/**
 * @brief Initializes the GPIO interface between MCU and flash device.
 */
static void AT25_InterfaceInit(void)
{
    // Set MOSI, SCK and CS output, ensure all others are inputs
    // especially MISO must be an input pin
    DDR_SPI |= (1<<MOSI)|(1<<SCK);
    DDR_SPI &= ~(1<<MISO);

    DDR_CS |= (1<<CS);
    CS_HI();
}

/**
 * @brief Temp: SCK output.
 */
flash_status_t AT25_Disable(void)
{
	DDR_SPI &= ~(1<<SCK);
	SPI_MasterDeactivate();
	return FL_SUCCESS;
}


/**
 * @brief Checks, whether the connected flash memory is of AT25/AT26-series.
 * @return true, if any AT25-series Flash detected, otherwise false
 */
bool AT25_Identify(void)
{
    uint32_t jedecId;

    // Read the JEDEC ID of the device
    AT25_ReadJedecID(&jedecId);

    printf("jedecId: %u\r\n",jedecId);

    // lookup in table of known devices, setup local descriptor
    pAt25Desc = AT25_FindDevice(jedecId);

    // check if device is from AT25/AT26 series
    if(  ((jedecId&0x000000FF)==0x1F)               // ATMEL
      && ((((jedecId&0x0000FF00)>>8)&0xE0)==0x40)   // AT25/26-series
      )

      return true;

    return false;
}


/**
 * @brief Looks up a given Jedec ID in table of known devices.
 * Sets local
 *
 * @return pointer to matching device descriptor, NULL if none found
 */
static At25Desc* AT25_FindDevice(uint32_t jedecId)
{
    uint8_t i = 0;
    At25Desc* pDesc = NULL;

    // Search if device is recognized
    while ((i < (sizeof(at25Devices) / sizeof(At25Desc))) && !(pDesc)) {

        if (jedecId == at25Devices[i].jedecId) {

            pDesc = (At25Desc*) &at25Devices[i];
        }

        i++;
    }

    return pDesc;
}


/**
 * @brief Internal helper function writing a single byte.
 */
static inline void AT25_WriteByte(uint8_t data)
{
    // Start SPI transaction by setting CS low
    CS_LO();

    // Send the (command) byte
    SPDR = data;
    SPI_WAIT();

    // Stop the SPI transaction by setting CS high
    CS_HI();
}


#ifdef AT25_FORCE_WEL
/**
 * @brief Forces setting of the 'Write Enable Latch' Bit in Status Register.
 * This is necessary before any erase, program, (un)protect or write status
 * register command.
 */
static void AT25_ForceWEL(void)
{
    // check if write enabled, otherwise force
    regval = AT25_ReadStatusRegister();

    if(!(regval & AT25_SR_WEL_MASK)) {
        AT25_WriteByte(AT25_WRITE_ENABLE_CMD);

        // wait for device becoming ready again
        AT25_WaitReady();
    }
}
#endif


/**
 * @brief Waits for the flash device to become ready to accept new commands.
 * Stops polling the status register at least after certain given cycles
 * (does not block forever).
 */
static void AT25_WaitReady()
{
    uint32_t i=0;

    do {
        regval = AT25_ReadStatusRegister();
        if( (regval & AT25_SR_RDBUSY_MASK) == AT25_SR_RDYBSY_READY_MASK)
            break;
        i++;
    } while(i<0x00FFFFFF);

}


// === read commands (chapter 7) ==============================================

/**
 * @brief Sequentially read up to 256 bytes from a given start address.
 *
 * @param   addr - starting address
 *          pData - user provided area to read to
 *          len - number of bytes to read, (truncated) to 256
 */
void AT25_ReadArray(uint32_t addr, uint8_t *pData, uint16_t len)
{
	// sanity check
	if(len>256)
		len=256;

    // Start SPI transaction by setting CS low
    CS_LO();

    // Send the command byte
    SPDR = AT25_READ_ARRAY_CMD_1;
    SPI_WAIT();

    // Send three address bytes
    //SPDR = ((addr&0x00FF0000)>>16);
    //SPI_WAIT();
    //SPDR = ((addr&0x0000FF00)>>8);
    //SPI_WAIT();
    //SPDR = ((addr&0x000000FF)>>0);
    //SPI_WAIT();

    // Send addr byte
    SPDR = addr;
    SPI_WAIT();

    do
    {
        // Do dummy write for initiating SPI read
        SPDR = SPI_DUMMY_VALUE;
        SPI_WAIT();

        // Upload the received byte in the user provided location
        *pData++ = SPDR;
        len--;

    } while (len > 0);

    // Stop the SPI transaction by setting CS high
    CS_HI();
}


// === program and erase commands (chapter 8) =================================

/**
 * @brief Programs the supplied data into a given page which must have been
 * erased previously. Data will be wrapped if end of page gets reached (i.e if
 * starting address is not aligned to a page border)!
 *
 * @param   addr - starting address to be programmed
 *          pData - points to the data to be written
 *          len - length of data area to be written (0<len<=256)
 *
 * @return  FL_SUCCESS if programming succeeded
 *          FL_ZERO_LEN if no data provided
 *          FL_NOT_ERASED if page was not erased before
 *          PROTECT if page was protected
 *          FL_ERROR if programming failed otherwise
 */
flash_status_t AT25_ProgramPage(uint32_t addr, uint8_t *pData, uint16_t len)
{
    uint8_t cnt;

    // at least one byte must be written
    if(len==0)
        return FL_ZERO_LEN;

    // sanity check
    if(len>256)
        len = 256;

    // previously check if associated sector locked
    if(SECTOR_PROTECTED == AT25_ReadProtectionRegister(addr))
        return FL_PROTECTED;

    // the page must have been erased previously, check if fulfilled
    // programming itself still works if not erased before,
    // but read and written data will be inconsistent
    AT25_ReadArray(addr & 0xFFFFFF00, buf, 256);
    for(cnt=0; cnt<0xFF; cnt++) {
        if(buf[cnt]!=0xFF)
            return FL_NOT_ERASED;
    }

    AT25_FORCE_WEL();

    // start transaction
    CS_LO();

    // send command
    SPDR = AT25_BYTE_PAGE_PROGRAM_CMD;
    SPI_WAIT();

    // write three address bytes
    SPDR = ((addr&0x00FF0000)>>16);
    SPI_WAIT();
    SPDR = ((addr&0x0000FF00)>>8);
    SPI_WAIT();
    SPDR = ((addr&0x000000FF)>>0);
    SPI_WAIT();

    while(len>0)
    {
        SPDR = *pData++;
        SPI_WAIT();
        len--;
    }

    // end transaction
    CS_HI();

    // wait for device becoming ready again
    AT25_WaitReady();

    // WEL gets erased always (even if failed)

    // check for programming/erase errors
    regval = AT25_ReadStatusRegister();
    if( (regval & AT25_SR_EPE_MASK) == AT25_SR_EPE_ERROR_MASK )
        return FL_ERROR;

    return FL_SUCCESS;
}


/**
 * @brief Starts a sequential programming procedure where only one byte can be
 * written on each invocation. There will be no wrap-around when reaching the
 * uppermost flash memory address. The memory area to write to must have been
 * erased before.
 *
 * @param   addr - starting address to be programmed
 *          data - the (first) data byte to be written
 *
 * @return  FL_SUCCESS if programming succeeded
 *          FL_PROTECTED if sector belonging to page is protected
 *          FL_NOT_ERASED if page has not been erased previously
 *          FL_ERROR if programming failed otherwise
 */
flash_status_t AT25_SequentialProgramStart(uint32_t addr, uint8_t data)
{
    // the amount of following bytes is yet unknown, but checking if the first
    // byte is within an erased area may be an indication for the following ones
    AT25_ReadArray(addr,buf,1);
    if(*buf!=0xFF)
    	return FL_NOT_ERASED;

    // previously check if associated sector locked
    if(SECTOR_PROTECTED == AT25_ReadProtectionRegister(addr))
        return FL_PROTECTED;

    AT25_FORCE_WEL();

    // start transaction
    CS_LO();

    // send command
	SPDR = AT25_SEQ_PROGRAMMING_MODE_CMD_1;
    SPI_WAIT();

    // write three address bytes
    SPDR = ((addr&0x00FF0000)>>16);
    SPI_WAIT();
    SPDR = ((addr&0x0000FF00)>>8);
    SPI_WAIT();
    SPDR = ((addr&0x000000FF)>>0);
    SPI_WAIT();

    // write the data byte
    SPDR = data;
    SPI_WAIT();

    // end transaction
    CS_HI();

    // wait for device becoming ready again
    AT25_WaitReady();

    // WEL bit stays set as long as AT25_WRITE_DISABLE_CMD is not executed,
    // except if sector was protected - then it gets erased immediately
    // after write attempt

    // check for programming/erase errors
    regval = AT25_ReadStatusRegister();
    if( (regval & AT25_SR_EPE_MASK) == AT25_SR_EPE_ERROR_MASK )
        return FL_ERROR;

    return FL_SUCCESS;
}


/**
 * @brief Writes a byte while being in sequential programming mode.
 * Only one byte can be written on each invocation, there will be no wrap-around
 * upon reaching the uppermost flash memory address. Here programming will be
 * terminated automatically.
 *
 * @param   data - the data byte to be written
 *
 * @return  FL_SUCCESS if programming succeeded
 *          PROTECT if page was protected
 *          FL_PAGE_BORDER_REACHED if sequential programming mode was left
 *            since page border has been reached
 *          FL_WRONG_WORKING_MODE if device is operating in byte/page programming mode
 *          FL_ERROR if programming failed otherwise
 */
flash_status_t AT25_SequentialProgramByte(uint8_t data)
{
    // determine if sequential programming mode already entered
    regval = AT25_ReadStatusRegister();
    if( (regval&AT25_SR_SPM_MASK) != AT25_SR_SPM_SEQUENTIAL_MASK )
        return FL_WRONG_WORKING_MODE;

    // it is impossible to check, if desired region is erased since the flash device
    // delays write operations (at last until sequential programming mode is left)

    // reasserting WEL bit is not necessary

    // start transaction
    CS_LO();

    // send command
    SPDR = AT25_SEQ_PROGRAMMING_MODE_CMD_1;
    SPI_WAIT();

    // write the data byte
    SPDR = data;
    SPI_WAIT();

    // end transaction
    CS_HI();

    // wait for device becoming ready again
    AT25_WaitReady();

    // if the SPM mode flag was reset, the page border has been reached
    regval = AT25_ReadStatusRegister();
    if( (regval&AT25_SR_SPM_MASK) != AT25_SR_SPM_SEQUENTIAL_MASK)
        return FL_PAGE_BORDER_REACHED;

    // WEL bit in status register stays set as long as AT25_WRITE_DISABLE_CMD
    // is not executed, except if sector was protected - then it gets erased
    // immediately after write attempt
    if( (regval&AT25_SR_WEL_MASK) != AT25_SR_WEL_ENABLED_MASK)
        return FL_PROTECTED;

    // check for programming/erase errors
    if( (regval & AT25_SR_EPE_MASK) == AT25_SR_EPE_ERROR_MASK )
        return FL_ERROR;

    return FL_SUCCESS;
}


/**
 * @brief leaves the sequential programming mode
 */
void AT25_SequentialProgramLeave(void)
{
    AT25_WriteByte(AT25_WRITE_DISABLE_CMD);

    // wait for device becoming ready again
    AT25_WaitReady();
}


/**
 * @brief erases a given block entire Flash memory (sets all bits to '1')
 * requires that the WEL bit has been set by executing WRITE_ENABLE_CMD previously
 *
 * @param   addr  - the starting address of the block
 *          blksz - the desired blocksize
 *
 * @return  FL_SUCCESS if erasing succeeded
 *          FL_PROTECTED if any of the affected pages were protected
 *          FL_ERROR if no affected page was protected, but erasing failed
 */
flash_status_t AT25_BlockErase(blocksize_t blksz, uint32_t addr)
{
    uint8_t cmd;

    // previously check if associated sector locked
    if(SECTOR_PROTECTED == AT25_ReadProtectionRegister(addr))
        return FL_PROTECTED;

    // prepare command
    switch(blksz)
    {
        case BLOCKSIZE_64K: cmd = AT25_BLOCK_ERASE_64K_CMD; break;
        case BLOCKSIZE_32K: cmd = AT25_BLOCK_ERASE_32K_CMD; break;
        case BLOCKSIZE_4K:
        default:            cmd = AT25_BLOCK_ERASE_4K_CMD; break;
    }

    AT25_FORCE_WEL();

    // start transaction
    CS_LO();

    // perform block erase
    SPDR = cmd;
    SPI_WAIT();

    // write three address bytes
    SPDR = ((addr&0x00FF0000)>>16);
    SPI_WAIT();
    SPDR = ((addr&0x0000FF00)>>8);
    SPI_WAIT();
    SPDR = ((addr&0x000000FF)>>0);
    SPI_WAIT();

    // end transaction
    CS_HI();

    // wait for device becoming ready again
    AT25_WaitReady();

    // WEL gets erased always (even if failed)

    // check for programming/erase errors
    regval = AT25_ReadStatusRegister();
    if( (regval & AT25_SR_EPE_MASK) == AT25_SR_EPE_ERROR_MASK )
        return FL_ERROR;

    return FL_SUCCESS;
}


/**
 * @brief erases the entire Flash memory (sets all bits to '1')
 * requires that the WEL bit has been set by executing WRITE_ENABLE_CMD previously
 *
 * @return  FL_SUCCESS if erasing succeeded
 *          FL_PROTECTED if any of the devices pages were protected
 *          FL_ERROR if no affected sector was protected, but erasing failed
 */
flash_status_t AT25_ChipErase(void)
{
    regval = AT25_ReadStatusRegister();
    if ((regval & AT25_SR_SWP_MASK) != AT25_SR_SWP_PROTNONE_MASK)
        return FL_PROTECTED;

    // enable critical write operation
    AT25_FORCE_WEL();

    // send command
    AT25_WriteByte(AT25_CHIP_ERASE_CMD_1);

    // wait for device becoming ready again
    AT25_WaitReady();

    // WEL gets erased always (even if failed)

    // check for programming/erase errors
    regval = AT25_ReadStatusRegister();
    if( (regval & AT25_SR_EPE_MASK) == AT25_SR_EPE_ERROR_MASK )
        return FL_ERROR;

    return FL_SUCCESS;
}


// === protection commands and features (chapter 9) ===========================

/**
 * @brief Enables the write mode.
 */
void AT25_WriteEnable(void)
{
    AT25_WriteByte(AT25_WRITE_ENABLE_CMD);

    // wait for device becoming ready again
    AT25_WaitReady();
}


/**
 * @brief Disables the write mode.
 */
void AT25_WriteDisable(void)
{
    AT25_WriteByte(AT25_WRITE_DISABLE_CMD);

    // wait for device becoming ready again
    AT25_WaitReady();
}


/**
 * @brief Protects a sector belonging to a given address
 *
 * @param   addr - any address within desired sector
 *
 * @return  FL_SUCCESS if protecting succeeded
 *          FL_ERROR if SPRL forbids protecting of sectors
 */
flash_status_t AT25_ProtectSector(uint32_t addr)
{
    // check if SPRL is set (=protecting forbidden)
    regval = AT25_ReadStatusRegister();
    if((regval & AT25_SR_SPRL_MASK) == AT25_SR_SPRL_LOCKED_MASK)
        return FL_ERROR;

    AT25_FORCE_WEL();

    // start transaction
    CS_LO();

    // send command
    SPDR = AT25_SECTOR_PROTECT_CMD;
    SPI_WAIT();

    // followed by three address bytes
    SPDR = ((addr&0x00FF0000)>>16);
    SPI_WAIT();
    SPDR = ((addr&0x0000FF00)>>8);
    SPI_WAIT();
    SPDR = ((addr&0x000000FF)>>0);
    SPI_WAIT();

    // end transaction
    CS_HI();

    // wait for device becoming ready again
    AT25_WaitReady();

    // WEL gets always reset, so it may not indicate success/failure

    return FL_SUCCESS;
}


/**
 * @brief Unprotects a sector belonging to a given address.
 *
 * @param   addr - any address within desired sector
 *
 * @return  FL_SUCCESS if unprotecting succeeded
 *          FL_ERROR if SPRL forbids unprotecting of sectors
 */
flash_status_t AT25_UnprotectSector(uint32_t addr)
{
    // check if SPRL is set (=unprotecting forbidden)
    regval = AT25_ReadStatusRegister();
    if((regval & AT25_SR_SPRL_MASK) == AT25_SR_SPRL_LOCKED_MASK)
        return FL_ERROR;

    AT25_FORCE_WEL();

    // start transaction
    CS_LO();

    // write command code
    SPDR = AT25_SECTOR_UNPROTECT_CMD;
    SPI_WAIT();

    // followed by three address bytes
    SPDR = ((addr&0x00FF0000)>>16);
    SPI_WAIT();
    SPDR = ((addr&0x0000FF00)>>8);
    SPI_WAIT();
    SPDR = ((addr&0x000000FF)>>0);
    SPI_WAIT();

    // end transaction
    CS_HI();

    // wait for device becoming ready again
    AT25_WaitReady();

    // WEL gets always reset, so it may not indicate success/failure

    return FL_SUCCESS;
}


/**
 * @brief Globally protects all sectors,
 * leave the SPRL Bit (global lock bit) as it is.
 */
void AT25_GlobalProtect(void)
{
    // leave SPRL bit untouched (see datasheet, p.19)
    AT25_WriteStatusRegister(0x7F);
}


/**
 * @brief Globally unprotects all sectors,
 * leaves the SPRL Bit (global lock bit) as it is.
 */
void AT25_GlobalUnprotect(void)
{
    // leave SPRL bit untouched (see datasheet, p.19)
    AT25_WriteStatusRegister(0x00);
}


/**
 * @brief Sets the SPRL bit in status register without changing the
 * sector protection states.
 */
void AT25_SetSPRL(void)
{
    // leave individual sector protections untouched (see datasheet, p.20)
    AT25_WriteStatusRegister(0xF0);
}


/**
 * @brief Clears the SPRL bit in status register without changing the
 * sector protection states.
 */
void AT25_ClearSPRL(void)
{
    // leave individual sector protections untouched (see datasheet, p.20)
    AT25_WriteStatusRegister(0x0F);
}


/**
 * @brief Checks whether the sector belonging to a given address is locked or not.
 *
 * @param  addr - any address within desired sector
 *
 * @return SECTOR_PROTECTED if sector associated to the address is locked,
 *         SECTOR_UNPROTECTED otherwise
 */
protect_status_t AT25_ReadProtectionRegister(uint32_t addr)
{
    unsigned char status = 0;

    // start transaction
    CS_LO();

    // Send the command byte
    SPDR = AT25_READ_SECTOR_PROTECT_REG_CMD;
    SPI_WAIT();

    // followed by three address bytes
    SPDR = ((addr&0x00FF0000)>>16);
    SPI_WAIT();
    SPDR = ((addr&0x0000FF00)>>8);
    SPI_WAIT();
    SPDR = ((addr&0x000000FF)>>0);
    SPI_WAIT();

    // Do dummy write for initiating SPI read
    SPDR = SPI_DUMMY_VALUE;
    SPI_WAIT();
    status = SPDR;

    // end transaction
    CS_HI();

    // wait for device becoming ready again
    AT25_WaitReady();

    return status?SECTOR_PROTECTED:SECTOR_UNPROTECTED;
}


// === status register commands (chapter 10) ==================================

/**
 * @brief Reads out the status register (1 byte).
 * Suitable for polling since register content gets updated permanently.
 *
 * @return the current status registers content
 */
uint8_t AT25_ReadStatusRegister(void)
{
	// start transaction
    CS_LO();

    // Send the command byte
    SPDR = AT25_READ_STATUS_REGISTER_CMD;
    SPI_WAIT();

    // Do dummy write for initiating SPI read
    SPDR = SPI_DUMMY_VALUE;
    SPI_WAIT();

    // end transaction
    CS_HI();

    return SPDR;
}


/**
 * @brief Writes a new value to the device status register.
 *
 * @param data - the desired status register value
 */
void AT25_WriteStatusRegister(uint8_t data)
{
    AT25_FORCE_WEL();

    CS_LO();

    // Send the command byte
    SPDR = AT25_WRITE_STATUS_REGISTER_CMD;
    SPI_WAIT();

    // Send the new status register value
    SPDR = data;
    SPI_WAIT();

    CS_HI();

    // wait for device becoming ready again
    AT25_WaitReady();

    // normally we should evaluate the status register
    // (check if WEL was reset; WEL reset <-> flash write protected)
    // but currently there is a permanent pullup.
}


// === other commands and functions (chapter 11) ==============================

/**
 * @brief Read out the manufacturer and device IDs.
 *
 * @param pData - pointer to location where to write the Jedec ID
 */
void AT25_ReadJedecID(uint32_t *pData)
{
    uint8_t i;
    uint8_t *pHelp = (uint8_t*) pData;

    // sanity check
    if(!pData) {
        return;
    }

    // start transaction
    CS_LO();

    // Send the command byte
    SPDR = AT25_READ_DEVICE_IDS_CMD;

    SPI_WAIT();

    for(i=0; i<4; i++)
    {
        // Do dummy write for initiating SPI read
        SPDR = SPI_DUMMY_VALUE;

        SPI_WAIT();

        // Upload the received byte in the user provided location
        *pHelp++ = SPDR;
    }

    // end transaction
    CS_HI();

    // wait for device becoming ready again
    AT25_WaitReady();
}


/**
 * @brief Put the flash into deep power down mode.
 */
void AT25_DeepPowerDown(void)
{
    AT25_WriteByte(AT25_DEEP_POWER_DOWN_CMD);

    // do NOT poll the status register here, since device is asleep
    // and reacts only on a wakeup command
}


/**
 * @brief Resume from deep power down mode. This is the only command which
 * is not ignored (executed) when sleeping!
 */
void AT25_DeepPowerDownResume(void)
{
    AT25_WriteByte(AT25_DEEP_POWER_RESUME_CMD);

    // wait for device becoming ready again
    AT25_WaitReady();
}


// === helper functions =======================================================

/**
 * @brief Helper function determining (and printing out) each sectors
 * protection state.
 */
void AT25_ShowSectorProtectionState(void)
{
    uint8_t i;

    // sanity check
    if(!pAt25Desc)
        return;

    printf("=====================================================================\r\n");
    printf("sector protection state ('x'=protected): \r\n");

    for(i=0; i<pAt25Desc->sectorCount; i++)
        printf(" %02d", i);
    printf("\r\n");

    for(i=0; i<11; i++)
        printf("  %c", AT25_ReadProtectionRegister(pAt25Desc->sectorStartingAddresses[i])?'x':'-');
    printf("\r\n");
    printf("=====================================================================\r\n");
}


/**
 * @brief Print the status register interpretation in a human-readable format.
 *
 * @param data - the register value to be interpret
 */
void AT25_InterpretStatusRegister(uint8_t data)
{
    printf("=====================================================================\r\n");
    printf("Status:=0x%02X \r\n", data);
    printf(" Sector Protection Registers: %s\r\n", (data&AT25_SR_SPRL_MASK)?"locked":"unlocked");
    printf(" Programming Mode           : %s\r\n", (data&AT25_SR_SPM_MASK)?"sequential":"byte/page");
    printf(" Erase/Program Error        : %s\r\n", (data&AT25_SR_EPE_MASK)?"error":"success");
    printf(" Write Protection Pin       : %s\r\n", (data&AT25_SR_WPP_MASK)?"deasserted":"asserted");
    printf(" Software Protection Status : %s\r\n", (data&AT25_SR_SWP_MASK)?"sectors protected":"sectors unprotected");
    printf(" Write Enable Latch Status  : %s\r\n", (data&AT25_SR_WEL_MASK)?"enabled":"disabled");
    printf(" Ready/Busy Status          : %s\r\n", (data&AT25_SR_RDBUSY_MASK)?"busy":"ready");
    printf("=====================================================================\r\n");
}


/**
 * @brief Helper function dumping a given data area.
 *
 * @param   pData   - pointer to data which to dump
 *          len     - lenght of data area
 *          title   - optional description to be written out
 */
void dump_data(uint8_t* pData, uint16_t len, char* title)
{
    const uint8_t ENTRIES_PER_LINE = 16;
    uint16_t i, cnt=0;

    printf(" ## %s ## \r\n offset | data \r\n", title);

    while(1)
    {
        printf(" 0x%04X | ", cnt);
        for(i=0; i<ENTRIES_PER_LINE; i++)
        {
            printf("%02X ",*pData++);
            len--;
            if(len==0)
            {
                printf("\r\n");
                return;
            }
        }
        printf("\r\n");
        cnt+=ENTRIES_PER_LINE;
    }
}

