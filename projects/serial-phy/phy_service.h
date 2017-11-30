/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * @brief IEEE 802.15.4 PHY service for connection to simulated 802.15.4
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "contiki.h"
#include "dev/pcapng.h"
#include "dev/pcapng-line.h"

#ifndef __PHY_SERVICE_H__
#define __PHY_SERVICE_H__

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
    PLME_GET_REQEST,
    PLME_GET_CONFIRM,
    PLME_SET_TRX_STATE_REQUEST,
    PLME_SET_TRX_STATE_CONFIRM,
    PLME_SET_REQUEST,
    PLME_SET_CONFIRM,
	RF_DATA_INDICATION,
    UNKNOWN_MSG_TYPE
} msg_type_sap;

/** @brief Conversion of a protocol message type to string */
inline static const char* msgTypeToString(msg_type_sap x)
{
    switch (x)
    {
        case PD_DATA_REQUEST:               return "PD_DATA_REQUEST";
        case PD_DATA_CONFIRM:               return "PD_DATA_CONFIRM";
        case PD_DATA_INDICATION:            return "PD_DATA_INDICATION";
        case PLME_CCA_REQUEST:              return "PLME_CCA_REQUEST";
        case PLME_CCA_CONFIRM:              return "PLME_CCA_CONFIRM";
        case PLME_ED_REQUEST:               return "PLME_ED_REQUEST";
        case PLME_ED_CONFIRM:               return "PLME_ED_CONFIRM";
        case PLME_GET_REQEST:               return "PLME_GET_REQEST";
        case PLME_GET_CONFIRM:              return "PLME_GET_CONFIRM";
        case PLME_SET_TRX_STATE_REQUEST:    return "PLME_SET_TRX_STATE_REQUEST";
        case PLME_SET_TRX_STATE_CONFIRM:    return "PLME_SET_TRX_STATE_CONFIRM";
        case PLME_SET_REQUEST:              return "PLME_SET_REQUEST";
        case PLME_SET_CONFIRM:              return "PLME_SET_CONFIRM";
        case RF_DATA_INDICATION:			return "RF_DATA_INDICATION";
        default:                            return "[Unknown msgType]";
    }
}

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
inline static const char* phyStateToString(phy_state x)
{
    switch (x)
    {
        case phy_BUSY:              	return "phy_BUSY";
        case phy_BUSY_RX:           	return "phy_BUSY_RX";
        case phy_BUSY_TX:				return "phy_BUSY_TX";
        case phy_FORCE_TRX_OFF:     	return "phy_FORCE_TRX_OFF";
        case phy_IDLE:              	return "phy_IDLE";
        case phy_INVALID_PARAMETER: 	return "phy_INVALID_PARAMETER";
        case phy_RX_ON:					return "phy_RX_ON";
        case phy_SUCCESS:               return "phy_SUCCESS";
        case phy_TRX_OFF:				return "phy_TRX_OFF";
        case phy_TX_ON:					return "phy_TX_ON";
        case phy_UNSUPPORT_ATTRIBUTE:   return "phy_UNSUPPORT_ATTRIBUTE";
        case phy_READ_ONLY:             return "phy_READ_ONLY";
        default:                        return "[Unknown phyState]";
    }
}

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
inline static const char* phyAttrToString(phy_pib_attr x)
{
    switch (x)
    {
        case phyCurrentChannel:     	return "phyCurrentChannel";
        case phyChannelsSupported:  	return "phyChannelsSupported";
        case phyTransmitPower:			return "phyTransmitPower";
        case phyCCAMode:     			return "phyCCAMode";
        case phyCurrentPage:        	return "phyCurrentPage";
        case phyMaxFrameDuration: 		return "phyMaxFrameDuration";
        case phySHRDuration:			return "phySHRDuration";
        case phySymbolsPerOctet:    	return "phySymbolsPerOctet";
        default:                    	return "[Unknown phyAttr]";
    }
}

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

/** @brief	The number of symbols per octet (coming from the current PHY)
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

/** ------- PHY Data Service Primitive Header Lengths --------- */

/** @brief Maximum PHY message header size */
#define maxPHYMessageHeaderSize 4;

/** @brief Size in bytes of PD_DATA_REQUEST message header */
#define SIZEOF_PD_DATA_REQUEST					3

/** @brief Size in bytes of PD_DATA_CONFIRM message header */
#define SIZEOF_PD_DATA_CONFIRM					3

/** @brief Size in bytes of PD_DATA_INDICATION message header */
#define SIZEOF_PD_DATA_INDICATION				4

/** @brief Size in bytes of PLME_CCA_REQUEST message header */
#define SIZEOF_PLME_CCA_REQUEST					2

/** @brief Size in bytes of PLME_CCA_CONFIRM message header */
#define SIZEOF_PLME_CCA_CONFIRM					3

/** @brief Size in bytes of PLME_ED_REQUEST message header */
#define SIZEOF_PLME_ED_REQUEST					2

/** @brief Size in bytes of PLME_ED_CONFIRM message header */
#define SIZEOF_PLME_ED_CONFIRM					4

/** @brief Size in bytes of PLME_GET_REQEST message header */
#define SIZEOF_PLME_GET_REQEST					3

/** @brief Size in bytes of PLME_GET_CONFIRM message header */
#define SIZEOF_PLME_GET_CONFIRM					4

/** @brief Size in bytes of PLME_SET_TRX_STATE_REQUEST message header */
#define SIZEOF_PLME_SET_TRX_STATE_REQUEST		3

/** @brief Size in bytes of PLME_SET_TRX_STATE_CONFIRM message header */
#define SIZEOF_PLME_SET_TRX_STATE_CONFIRM		3

/** @brief Size in bytes of PLME_SET_REQUEST message header */
#define SIZEOF_PLME_SET_REQUEST					3

/** @brief Size in bytes of PLME_SET_CONFIRM message header */
#define SIZEOF_PLME_SET_CONFIRM					4

/** @brief Size in bytes of PLME_SET_CONFIRM message header */
#define SIZEOF_RF_INDICATION					3

/** -------------- PHY Data Service Primitives --------------- */

/** @brief pd data request */
typedef struct pd_data_req
{
    uint8_t psduLength;       			/**< encapsulated payload length*/
    uint8_t data[aMaxPHYPacketSize];	/**< payload byte sequence */
} pd_data_req;

/** @brief pd data confirm */
typedef struct pd_data_conf
{
	phy_state status;					/**< PHY state */
} pd_data_conf;

/** @brief pd data indication */
typedef struct pd_data_ind
{
	uint8_t psduLength;       			/**< encapsulated payload length*/
	uint8_t ppduLinkQuality;  			/**< link quality of the PPDU */
	uint8_t data[aMaxPHYPacketSize];	/**< payload byte sequence */
} pd_data_ind;

/** ----------- PHY Management Service Primitives ------------ */

/** @brief plme cca request */
typedef struct plme_cca_req
{
} plme_cca_req;

/** @brief plme cca confirm */
typedef struct plme_cca_conf
{
	phy_state status;					/**< PHY state */
} plme_cca_conf;

/** --------------------------- */

/** @brief plme ed request */
typedef struct plme_ed_req
{
} plme_ed_req;

/** @brief plme ed confirm */
typedef struct plme_ed_conf
{
	phy_state status;					/**< PHY state */
    uint8_t energyLevel;				/**< ED energy level */
} plme_ed_conf;

/** --------------------------- */

/** @brief plme get request */
typedef struct plme_get_req
{
	phy_pib_attr attribute;				/**< PHY PIB attribute identifier */
} plme_get_req;

/** @brief plme get confirm */
typedef struct plme_get_conf
{
	phy_state status;					/**< PHY state */
	phy_pib_attr attribute;				/**< PHY PIB attribute identifier */
	PhyPIB_value value;					/**< specific PIB attribute value */
} plme_get_conf;

/** --------------------------- */

/** @brief plme set trx state request */
typedef struct plme_set_trx_state_req
{
	phy_state status;					/**< PHY state */
} plme_set_trx_state_req;

/** @brief plme set trx state request */
typedef struct plme_set_trx_state_conf
{
	phy_state status;					/**< PHY state */
} plme_set_trx_state_conf;

/** --------------------------- */

/** @brief plme set request */
typedef struct plme_set_req
{
	phy_pib_attr attribute;				/**< PHY PIB attribute identifier */
    PhyPIB_value value;					/**< specific PIB attribute value */
} plme_set_req;

/** @brief plme set confirm */
typedef struct plme_set_conf
{
	phy_state status;					/**< PHY state */
	phy_pib_attr attribute;				/**< PHY PIB attribute identifier */
} plme_set_conf;

/** --------------- PHY RF Service Primitives ---------------- */

/** @brief plme cca request */
typedef struct rf_ind
{
	uint8_t ppduLength;					/**< encapsulated payload length*/
	uint8_t data[aMaxPHYPacketSize];	/**< payload byte sequence */
} rf_ind;

/** --------------------------- */

/** @brief Union capable of holding any specific PHY messages */
typedef union
{
	pd_data_req data_req;						/**< PHY_DATA_req */
	pd_data_conf data_conf;						/**< PHY_DATA_conf */
	pd_data_ind data_ind;						/**< PHY_DATA_ind */
	plme_cca_req cca_req;						/**< PHY_CCA_req */
	plme_cca_conf cca_conf;						/**< PHY_CCA_conf */
	plme_ed_req ed_req;							/**< PHY_ED_req */
	plme_ed_conf ed_conf;						/**< PHY_ED_conf */
	plme_get_req get_req;						/**< PHY_GET_req */
	plme_get_conf get_conf;						/**< PHY_GET_conf */
	plme_set_trx_state_req set_trx_state_req;	/**< PHY_SET_TRX_STATE_req */
	plme_set_trx_state_conf set_trx_state_conf;	/**< PHY_SET_TRX_STATE_conf */
	plme_set_req set_req;						/**< PHY_SET_TRX_STATE_req */
	plme_set_conf set_conf;						/**< PHY_SET_TRX_STATE_conf */
} PHY_msg_x;

/** @brief Compound PHY message */
typedef struct
{
	msg_type_sap type;		/**< PHY message type, e.g. PD_DATA_REQUEST */
	uint8_t length;			/**< PHY message length */
	PHY_msg_x x;			/**< specific PHY message */
} PHY_msg;

int serialize_msg(PHY_msg * msg, uint8_t buffer[aMaxPHYPacketSize]);
int deserialize_msg(uint8_t *stream, PHY_msg *msg);
void send_msg(PHY_msg * msg);
void print_msg_payload(void *data, uint8_t length, char *info);
void print_msg(PHY_msg * msg, char *info);
void send_test(void);

#endif /* __PHY_SERVICE_H__ */
