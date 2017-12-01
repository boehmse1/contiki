/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * IEEE 802.15.4 radio driver
 *
 */

#ifndef __ATMEGA128RFA1_H__
#define __ATMEGA128RFA1_H__

#include <serial-phy.h>
#include <stdint.h>
#include <stdio.h>

#include "radio/rf230bb/rf230bb.h"
#include "dev/radio.h"

/** @brief	Threshold for energy detection
 *
 * Note:	ATmega128RFA1 datasheet - Figure 9-18
 * 			energy level resolution 0-83
 * 			ED sensitivity -90dBm (steps are 1dBm PHY_ED_LEVEL)
 */
static const int8_t threshold = -90;

/** @brief 	Tolerance of the output power level
 *
 * See ATmega128RFA1 datasheet - 35.14.3 Transmitter Characteristics
 *
 * Note:	Shifted to MSBs b7 .. b6 of a 8-bit value
 */
static const uint8_t txPowerTolerance = 64;

/** @brief 	Tolerance of the output power level
 *
 * See ATmega128RFA1 datasheet - 35.14.3 Transmitter Characteristics
 *
 * Note:	Shifted to MSBs b7 .. b6 of a 8-bit value
 */
static const phyAttrChannelsSupported channelsSupported = 0xAABBCCDD;

/** @brief	Channel page (according to 2,4GHz O-QPSK PHY) */
static const phyAttrCurrentPage currentPage = 0;

/** @brief	Maximum frame duration (according to 2,4GHz O-QPSK PHY) */
static const phyAttrMaxFrameDuration maxFrameDuration = 266;

/** @brief	Duration of the SHR (according to 2,4GHz O-QPSK PHY) */
static const phyAttrSHRDuration sHRDuration = 10;

/** @brief	Number of symbols per octet (according to 2,4GHz O-QPSK PHY) */
static const phyAttrSymbolsPerOctet symbolsPerOctet = 2;

/** @brief	PAN information database */
static PhyPIB pib = (PhyPIB) {
		.currentChannel = 24,
		.transmitPower = (64 & 3),
		.cCAMode = 3
};

/** @brief	phy state of the transceiver */
static phy_state state = phy_IDLE;

/**
 * @brief	Conversion between internal avr implementation and official
 * 			specification phy enumeration
 *
 * According to Table 18—PHY enumerations description
 *
 * @param	state according to the avr implementation
 *
 * @return	phy_state according to the specification
 */
static inline const phy_state radioStatusToPhyState(radio_status_t state)
{
	switch (state)
	{
	/** @brief A SET/GET, an ED operation, or a transceiver state change was successful. */
	case RADIO_SUCCESS:					return phy_SUCCESS;
	/** @brief The connected device is not an Atmel AT86RF230. */
	case RADIO_UNSUPPORTED_DEVICE:		return -1;
	/** @brief A SET/GET request was issued with a parameter in the primitive that is out of the valid range. */
	case RADIO_INVALID_ARGUMENT:		return phy_INVALID_PARAMETER;
	/** @brief The requested service timed out. */
	case RADIO_TIMED_OUT:				return -1;
	/** @brief The end-user tried to do an invalid state transition. */
	case RADIO_WRONG_STATE:				return -1;
	/** @brief The radio transceiver is busy receiving or transmitting. */
	case RADIO_BUSY_STATE:				return -1;
	/** @brief The requested state transition could not be completed. */
	case RADIO_STATE_TRANSITION_FAILED:	return -1;
	/** @brief The CCA attempt has detected an idle channel. */
	case RADIO_CCA_IDLE:				return phy_IDLE;
	/** @brief The CCA attempt has detected a busy channel. */
	case RADIO_CCA_BUSY:				return phy_BUSY;
	/** @brief Transceiver is busy receiving or transmitting data. */
	case RADIO_TRX_BUSY:				return phy_BUSY_RX;	/** todo: or phy_BUSY_TX */ /**<  */
	/** @brief Measured battery voltage is lower than voltage threshold. */
	case RADIO_BAT_LOW:					return -1;
	/** @brief Measured battery voltage is above the voltage threshold. */
	case RADIO_BAT_OK:					return -1;
	/** @brief The CRC failed for the actual frame. */
	case RADIO_CRC_FAILED:				return -1;
	/** @brief The channel access failed during the auto mode. */
	case RADIO_CHANNEL_ACCESS_FAILURE:	return -1;
	/** @brief No acknowledge frame was received. */
	case RADIO_NO_ACK:					return -1;
    default:                        	return -1;
	}
}

/**
 * @brief	Conversion between internal avr implementation and official
 * 			specification phy states.
 * 			Only the "states" according to the avr implementation are mentioned.
 *
 * Because the specification does not make any assumptions about the operation
 * modes or transceiver states of the radio module, we have to map here.
 *
 * According to rf230bb.c::radio_set_trx_state() following states are possible:
 * 			RX_ON        Requested transition to RX_ON state.
 * 			TRX_OFF      Requested transition to TRX_OFF state.
 * 			PLL_ON       Requested transition to PLL_ON state.
 * 			RX_AACK_ON   Requested transition to RX_AACK_ON state.
 * 			TX_ARET_ON   Requested transition to TX_ARET_ON state.
 *
 * According to PLME-SET-TRX-STATE.request following states are possible:
 * 			RX_ON
 * 			TRX_OFF
 * 			FORCE_TRX_OFF
 * 			TX_ON
 *
 * @param	phy_state according to the specification
 *
 * @return	state according to the avr implementation
 */
static inline const uint8_t phyStateToRadioState(phy_state state)
{
	switch (state)
	{
//		case phy_BUSY_RX:           	return BUSY_RX;
//		case phy_BUSY_TX:				return BUSY_TX;
		case phy_RX_ON:					return RX_ON;
		case phy_TRX_OFF:				return TRX_OFF;
		case phy_FORCE_TRX_OFF:			return TRX_OFF;
		case phy_TX_ON:					return PLL_ON;	/**< 9.4.1.2.3 PLL_ON – PLL State */
		/** todo: 9.4.2.3 RX_AACK_ON – Receive with Automatic ACK (from extended operation mode) */
		/** todo: 9.4.2.5 TX_ARET_ON – Transmit with Automatic Retry and CSMA-CA Retry (from extended operation mode) */
		default:                        return -1;
//		9.4.1.2.1 SLEEP – Sleep State
//		9.4.1.2.6 RESET State
	}
}

/**
 * @brief Get the RSSI threshold
 *
 * @return threshold value
 */
static inline const int8_t radio_get_rssi_threshold() { return threshold; }

/**
 * @brief Get the supported channels
 *
 * @return channelsSupported stored in a 32-bit vector
 */
static inline const uint32_t radio_get_channels_supported() { return channelsSupported; }

/**
 * @brief Get the maximum frame duration
 *
 * @return maxFrameDuration according to 2,4GHz O-QPSK PHY
 */
static inline const uint16_t radio_get_max_frame_duration() { return maxFrameDuration; }

/**
 * @brief Get the duration of the SHR
 *
 * @return sHRDuration according to 2,4GHz O-QPSK PHY
 */
static inline const uint8_t radio_get_shr_duration() { return sHRDuration; }

/**
 * @brief Get the number of symbols per octed
 *
 * @return symbolsPerOctet according to 2,4GHz O-QPSK PHY
 */
static inline const uint8_t radio_get_symbols_per_octet() {return symbolsPerOctet;}

/**
 * @brief Get the current page
 *
 * @return currentPage according to 2,4GHz O-QPSK PHY
 */
static inline const uint8_t radio_get_current_page() { return currentPage; }

/**
 * @brief Set the current page
 *
 * @param page to be set
 *
 * @return PHY state phy_READ_ONLY according to 2,4GHz O-QPSK PHY
 */
static inline const phy_state radio_set_current_page(uint8_t page) { return phy_READ_ONLY; }

/**
 * @brief Get the current transmission channel
 *
 * @return current channel
 */
//static inline uint8_t radio_get_operating_channel() { return rf230_get_channel(); }
static inline uint8_t radio_get_operating_channel() { return pib.currentChannel; }

/**
 * @brief Get the transmission power level
 *
 * @return transmitPower of the radio module
 */
//static inline uint8_t radio_get_tx_power_level() {	return rf230_get_txpower(); }
static inline uint8_t radio_get_tx_power_level() {	return pib.transmitPower; }

/**
 * @brief Get the current CCA mode
 *
 * @return cCAMode of the radio module
 */
static inline uint8_t radio_get_cca_mode() { return pib.cCAMode; }

/**
 * @brief Get the TRX state
 *
 * @return PHY state
 */
static inline phy_state radio_get_trx_state() { return state; }

phy_state radio_set_operating_channel(uint8_t channel);
phy_state radio_set_tx_power_level(uint8_t power);
phy_state radio_set_cca_mode(uint8_t mode, uint8_t thres);
phy_state radio_set_trx_state(phy_state state);
phy_state radio_perform_cca();
phy_state radio_perform_ed(uint8_t *energyLevel);
phy_state radio_send(uint8_t * data, uint8_t length);

#endif /* __ATMEGA128RFA1_H__ */
