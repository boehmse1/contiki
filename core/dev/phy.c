/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * @brief 	IEEE 802.15.4 PHY According to 2006 specification
 *
 */

#include "phy.h"

const char* msgTypeToString(msg_type_sap x)
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
        case PLME_GET_REQUEST:              return "PLME_GET_REQUEST";
        case PLME_GET_CONFIRM:              return "PLME_GET_CONFIRM";
        case PLME_SET_TRX_STATE_REQUEST:    return "PLME_SET_TRX_STATE_REQUEST";
        case PLME_SET_TRX_STATE_CONFIRM:    return "PLME_SET_TRX_STATE_CONFIRM";
        case PLME_SET_REQUEST:              return "PLME_SET_REQUEST";
        case PLME_SET_CONFIRM:              return "PLME_SET_CONFIRM";
        case RF_DATA_INDICATION:			return "RF_DATA_INDICATION";
        default:                            return "[Unknown msgType]";
    }
}

const char* phyStateToString(phy_state x)
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

const phy_state radioRetValueToPhyState(uint8_t retValue)
{
	switch (retValue)
	{
		case RADIO_RESULT_OK:
		{
			return phy_SUCCESS;
		}
		case RADIO_RESULT_NOT_SUPPORTED:	return phy_UNSUPPORT_ATTRIBUTE;
		case RADIO_RESULT_INVALID_VALUE:	return -1;
		case RADIO_RESULT_ERROR:			return -1;
		case RADIO_RESULT_READ_ONLY:		return phy_READ_ONLY;
		default:                    		return -1;
	}
}

const phy_state radioRetValueTXToPhyState(uint8_t retValue)
{
	switch (retValue)
	{
		case RADIO_TX_OK:					return phy_SUCCESS;
		case RADIO_TX_ERR:					return -1;
		case RADIO_TX_COLLISION:			return -1;
		case RADIO_TX_NOACK:				return -1;
		default:                    		return -1;
	}
}

const char* phyAttrToString(phy_pib_attr x)
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
