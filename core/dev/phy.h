/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * @brief 	IEEE 802.15.4 PHY According to 2006 specification
 *
 */

#include <stdint.h>

#include "radio.h"

#ifndef __PHY_H__
#define __PHY_H__

/** @brief Message types for PD-SAP, PLME-SAP and RF-SAP service primitives */
typedef enum msg_type_sap
{
    PD_DATA_REQUEST = 0,
    PD_DATA_CONFIRM,
    PD_DATA_INDICATION,
    PLME_CCA_REQUEST,
    PLME_CCA_CONFIRM,
    PLME_ED_REQUEST,
    PLME_ED_CONFIRM,
    PLME_GET_REQUEST,
    PLME_GET_CONFIRM,
    PLME_SET_TRX_STATE_REQUEST,
    PLME_SET_TRX_STATE_CONFIRM,
    PLME_SET_REQUEST,
    PLME_SET_CONFIRM,
	RF_DATA_INDICATION,
    UNKNOWN_MSG_TYPE
} msg_type_sap;

/** @brief Conversion of a protocol message type to string */
const char* msgTypeToString(msg_type_sap x);

/** @brief PHY message types for PIB (for convenience with simulation model) */
typedef enum msgTypesPIB
{
    SETTRXSTATE = 0,
    GET,
    SET,
    CCA,
    ED,
    CONF,
    SETCONF
} msg_type_pib;

/** @brief	PHY state enumeration values
 *
 * See IEEE Std 802.15.4-2006, Table 18—PHY enumerations description
 */
typedef enum phyState
{
    phy_BUSY = SETCONF+1,		/**< for convenience with simulation model */
    phy_BUSY_RX,
    phy_BUSY_TX,
    phy_FORCE_TRX_OFF,
    phy_IDLE,
    phy_INVALID_PARAMETER,
    phy_RX_ON,
    phy_SUCCESS,
    phy_TRX_OFF,
    phy_TX_ON,
    phy_UNSUPPORT_ATTRIBUTE,
    phy_READ_ONLY
} phy_state;

/** @brief Conversion of a PHY state to string */
const char* phyStateToString(phy_state x);

/**
 * @brief	Conversion between radio return value implementation and
 * 			official specification phy enumeration
 *
 * According to core/dev/radio.h
 *
 * @param	retValue according to contiki's radio driver (radio.h)
 *
 * @return	phy_state according to the specification
 */
const phy_state radioRetValueToPhyState(uint8_t retValue);

/**
 * @brief	Conversion between radio return value implementation and
 * 			official specification phy enumeration
 *
 * According to core/dev/radio.h
 *
 * @param	retValue according to contiki's radio driver (radio.h)
 *
 * @return	phy_state according to the specification
 */
const phy_state radioRetValueTXToPhyState(uint8_t retValue);

/** @brief	PHY PIB attribute identifier
 *
 * See Std 802.15.4-2006, Table 23—PHY PIB attributes
 */
typedef enum phy_pib_attr
{
    phyCurrentChannel = 0,
    phyChannelsSupported,
    phyTransmitPower,
    phyCCAMode,
    phyCurrentPage,
    phyMaxFrameDuration,
    phySHRDuration,
    phySymbolsPerOctet
} phy_pib_attr;

/** @brief Conversion of a PHY attribute type to string */
const char* phyAttrToString(phy_pib_attr x);

/** @brief RF transmission channel */
typedef uint8_t phyAttrCurrentChannel;

/** @brief	Supported RF channels of the current PHY
 *
 * Note: 	The device only needs to add the rows (channel pages)
 * 			for the PHY(s) it supports.
 * 			Because devices only implement one channel page in practice,
 * 			we only need to add one 32-bit vector here.
 */
typedef uint32_t phyAttrChannelsSupported;		/**< read only value */

/** @brief	Supported RF channels of the current PHY
 *
 * Note: 	The device only needs to add the rows (channel pages)
 * 			for the PHY(s) it supports.
 * 			Because devices only implement one channel page in practice,
 * 			we only need to add one 32-bit vector here.
 * 			The 2 MSBs represent the (read only) tolerance on the tx power:
 * 			00 = ± 1 dB
 * 			01 = ± 3 dB
 * 			10 = ± 6 dB
 * 			The 6 LSBs represent a signed integer in twos-complement format,
 * 			corresponding to the nominal transmit power of the device
 * 			in decibels relative to 1 mW. The lowest value of phyTransmitPower
 * 			is interpreted as less than or equal to –32 dBm.
 */
typedef uint8_t phyAttrTransmitPower;			/**< value with specific bits */

/** @brief	The CCA mode
 *
 * See Std 802.15.4-2006, 6.9.9 Clear channel assessment (CCA)
 */
typedef uint8_t phyAttrCCAMode;

/** @brief	current PHY channel page
 *
 * See Std 802.15.4-2006, Table 2—Channel page and channel number
 */
typedef uint8_t phyAttrCurrentPage;

/** @brief	The maximum frame duration (coming from the current PHY) */
typedef uint16_t phyAttrMaxFrameDuration;		/**< read only value */

/** @brief	The duration of the SHR (coming from the current PHY) */
typedef uint8_t phyAttrSHRDuration;				/**< read only value */

/** @brief	The number of symbols per octed (coming from the current PHY)
 *
 * Note:	The possible float values 0.4, 1.6, 2.0, 8.0 represented as
 * 			unsigned 8-bit integer to save space:
 * 			MSBs b4 .. b7 represent the even,
 * 			LSBs b0 .. b3 the odd part of the value
 */
typedef uint8_t phyAttrSymbolsPerOctet;			/**< read only value */

/** @brief PHY PIB database */
typedef struct PhyPIB
{
	phyAttrCurrentChannel currentChannel;
	phyAttrChannelsSupported channelsSupported;
	phyAttrTransmitPower transmitPower;
	phyAttrCCAMode cCAMode;
	phyAttrCurrentPage currentPage;
	phyAttrMaxFrameDuration maxFrameDuration;
	phyAttrSHRDuration sHRDuration;
	phyAttrSymbolsPerOctet symbolsPerOctet;
} PhyPIB;

/** @brief Union capable of holding any specific PHY PIB value */
typedef union
{
	phyAttrCurrentChannel currentChannel;
	phyAttrChannelsSupported channelsSupported;
	phyAttrTransmitPower transmitPower;
	phyAttrCCAMode cCAMode;
	phyAttrCurrentPage currentPage;
	phyAttrMaxFrameDuration maxFrameDuration;
	phyAttrSHRDuration sHRDuration;
	phyAttrSymbolsPerOctet symbolsPerOctet;
} PhyPIB_value;

/** @brief Maximum PHY DATA message payload size */
#define aMaxPHYPacketSize 127

#endif /* __PHY_H__ */
