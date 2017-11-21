/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * IEEE 802.15.4 radio driver
 *
 */

#ifndef __ATMEGA128RFA1_H__
#define __ATMEGA128RFA1_H__

#include <stdint.h>
#include <stdio.h>

#include "phy_service.h"

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
static inline uint8_t radio_get_operating_channel() { return pib.currentChannel; }

/**
 * @brief Get the transmission power level
 *
 * @return transmitPower of the radio module
 */
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
phy_state radio_send(uint8_t * data);

#endif /* __ATMEGA128RFA1_H__ */
