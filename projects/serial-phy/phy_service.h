/*
 * Copyright (c) 2015 Sebastian Boehm (BTU-CS)
 *
 * Serial 802.15.4 PHY for connection to simulated 802.15.4 MAC
 *
 */

#include <stdint.h>

#ifndef __PHY_SERVICE_H__
#define __PHY_SERVICE_H__

/** protocol message types for PD-SAP and PLME-SAP service primitives */
typedef enum msg_type_sap{
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
    UNKNOWN_MSG_TYPE
}msg_type_sap;

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
        default:                            return "[Unknown msgType]";
    }
}

/** PHY message types for PIB */
typedef enum msgTypesPIB {
    SETTRXSTATE = 0,
    GET,
    SET,
    CCA,
    ED,
    CONF,
    SETCONF
}msg_type_pib;

/** PHY state enumeration values */
typedef enum phyState {
    phy_BUSY = SETCONF+1,
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
}phy_state;

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

/** PHY PIB attribute identifier */
typedef enum phy_pib_attr{
    phyCurrentChannel = 0,
    phyChannelsSupported,
    phyTransmitPower,
    phyCCAMode,
    phyCurrentPage,
    phyMaxFrameDuration,
    phySHRDuration,
    phySymbolsPerOctet
}phy_pib_attr;

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

typedef uint8_t phyAttrCurrentChannel;
/** The device only needs to add the rows (channel pages) for the PHY(s) it supports. */
typedef uint32_t phyAttrChannelsSupported;			/* read only */
typedef uint8_t phyAttrTransmitPower;
typedef uint8_t phyAttrCCAMode;
typedef uint8_t phyAttrCurrentPage;
typedef uint16_t phyAttrMaxFrameDuration;			/* read only */
typedef uint8_t phyAttrSHRDuration;					/* read only */
/** The possible float values 0.4, 1.6, 2.0, 8.0 represented as unsigned int here
 * MSB's b4 .. b7 represent the even, the LSB's b0 .. b3 the odd part of the value */
typedef uint8_t phyAttrSymbolsPerOctet;

typedef struct PhyPIB{
	phyAttrCurrentChannel currentChannel;
	phyAttrChannelsSupported channelsSupported;		/* read only */
	phyAttrTransmitPower transmitPower;
	phyAttrCCAMode cCAMode;
	phyAttrCurrentPage currentPage;
	phyAttrMaxFrameDuration maxFrameDuration;		/* read only */
	phyAttrSHRDuration sHRDuration;					/* read only */
	phyAttrSymbolsPerOctet symbolsPerOctet;			/* read only */
}PhyPIB;

/** @brief Union capable of holding any specific PHY PIB value */
typedef union
{
	phyAttrCurrentChannel currentChannel;
	phyAttrChannelsSupported channelsSupported;		/* read only */
	phyAttrTransmitPower transmitPower;
	phyAttrCCAMode cCAMode;
	phyAttrCurrentPage currentPage;
	phyAttrMaxFrameDuration maxFrameDuration;		/* read only */
	phyAttrSHRDuration sHRDuration;					/* read only */
	phyAttrSymbolsPerOctet symbolsPerOctet;			/* read only */
} PhyPIB_value;

/** PHY message header size */
#define maxPHYMessageHeaderSize 4;

/** hardware dependent PHY constants */
#define aMaxPHYPacketSize 127
#define aTurnaroundTime 12

/** PHY Data Service Primitive Lengths */
#define SIZEOF_PD_DATA_REQUEST					3
#define SIZEOF_PD_DATA_CONFIRM					3
#define SIZEOF_PD_DATA_INDICATION				4
#define SIZEOF_PLME_CCA_REQUEST					2
#define SIZEOF_PLME_CCA_CONFIRM					3
#define SIZEOF_PLME_ED_REQUEST					2
#define SIZEOF_PLME_ED_CONFIRM					4
#define SIZEOF_PLME_GET_REQEST					3
#define SIZEOF_PLME_GET_CONFIRM					4
#define SIZEOF_PLME_SET_TRX_STATE_REQUEST		3
#define SIZEOF_PLME_SET_TRX_STATE_CONFIRM		3
#define SIZEOF_PLME_SET_REQUEST					3
#define SIZEOF_PLME_SET_CONFIRM					4

/** -------------- PHY Data Service Primitives --------------- */

/** pd data request */
typedef struct pd_data_req
{
    uint8_t psduLength;       			// ≤ aMaxPHYPacketSize
    uint8_t data[aMaxPHYPacketSize];	/**< payload (uninterpreted byte sequence) */
} pd_data_req;

/** pd data confirm */
typedef struct pd_data_conf
{
	uint8_t status;					// SUCCESS, RX_ON, TRX_OFF, BUSY_TX
} pd_data_conf;

/** pd data indication */
typedef struct pd_data_ind
{
	uint8_t psduLength;       			// ≤ aMaxPHYPacketSize
	uint8_t ppduLinkQuality;  			// 0x00-0xff
	uint8_t data[aMaxPHYPacketSize];	/**< payload (uninterpreted byte sequence) */
} pd_data_ind;

/** ----------- PHY Management Service Primitives ------------ */

/** plme cca request */
typedef struct plme_cca_req
{
} plme_cca_req;

/** plme cca confirm */
typedef struct plme_cca_conf
{
	uint8_t status;						// TRX_OFF, BUSY, IDLE
} plme_cca_conf;

/** --------------------------- */

/** plme ed request */
typedef struct plme_ed_req
{
} plme_ed_req;

/** plme ed confirm */
typedef struct plme_ed_conf
{
	uint8_t status;						// SUCCESS, TRX_OFF, TX_ON
    uint8_t energyLevel;				// 0x00-0xff
} plme_ed_conf;

/** --------------------------- */

/** plme get request */
typedef struct plme_get_req
{
	uint8_t attribute;					// See phy_pib_attr
} plme_get_req;

/** plme get confirm */
typedef struct plme_get_conf
{
	uint8_t status;						// SUCCESS, UNSUPPORTED_ATTRIBUTE
	uint8_t attribute;					// See phy_pib_attr
	PhyPIB_value value;					// specific PIB attribute value
} plme_get_conf;

/** --------------------------- */

/** plme set trx state request */
typedef struct plme_set_trx_state_req
{
	uint8_t status;						// RX_ON, TRX_OFF, FORCE_TRX_OFF, TX_ON
} plme_set_trx_state_req;

/** plme set trx state request */
typedef struct plme_set_trx_state_conf
{
	uint8_t status;						// SUCCESS, RX_ON, TRX_OFF, TX_ON
} plme_set_trx_state_conf;

/** --------------------------- */

/** plme set request */
typedef struct plme_set_req
{
	uint8_t attribute;					// See phy_pib_attr
    PhyPIB_value value;					// specific PIB attribute value
} plme_set_req;

/** plme set confirm */
typedef struct plme_set_conf
{
	uint8_t status;						// SUCCESS, UNSUPPORTED_ATTRIBUTE, INVALID_PARAMETER,READ_ONLY
	uint8_t attribute;					// See phy_pib_attr
} plme_set_conf;

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

/** @brief Compound SDU message */
typedef struct
{
  uint8_t type;		/**< PHY SDU type, e.g. PD_DATA_REQUEST */
  uint8_t length;	/**< PHY SDU length */
  PHY_msg_x x;		/**< specific SDU */
} PHY_msg;

int serialize_msg(PHY_msg * msg, uint8_t buffer[aMaxPHYPacketSize]);
int deserialize_msg(uint8_t *stream, PHY_msg *msg);
void send_msg(PHY_msg * msg);
void print_msg_payload(uint8_t data[aMaxPHYPacketSize], uint8_t length, char *info);
void print_msg_bytes(PHY_msg *msg, uint8_t length);
void print_msg(PHY_msg * msg, char *info);

#endif /* __PHY_SERVICE_H__ */
