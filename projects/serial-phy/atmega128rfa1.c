/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * ATmega128RFA1 IEEE 802.15.4 radio driver
 *
 */

#include "atmega128rfa1.h"

#include "net/netstack.h"
#include "net/packetbuf.h"

#define DEBUG 0
#if DEBUG && DEBUG == 1
#define print_debug(fmt, args...) printf("[ATMEGA128RFA1]: " fmt "\n", ##args)
#elif DEBUG && DEBUG == 2
#define print_debug(fmt, args...) printf("DEBUG: %s:%d: " fmt, \
    __FILE__, __LINE__, ##args)
#else
#define print_debug(...)
#endif

/**
 * @brief Set the current transmission channel
 *
 * @param channel to be set
 *
 * @return PHY state phy_SUCCESS or phy_INVALID_PARAMETER
 */
phy_state
radio_set_operating_channel(uint8_t channel)
{
	if (channel >= RF230_MIN_CHANNEL && channel <= RF230_MAX_CHANNEL) {
		rf230_set_channel(channel);
		pib.currentChannel = rf230_get_channel();
		if (pib.currentChannel != channel)
			print_debug("Channel %u has not been correctly set! Transceiver channel is set to %u!", channel, pib.currentChannel);
		return phy_SUCCESS;
	} else
		return phy_INVALID_PARAMETER;
}

/**
 * @brief Set the transmission power level
 *
 * @param power level to be set
 *
 * @return PHY state phy_SUCCESS or phy_INVALID_PARAMETER
 */
phy_state
radio_set_tx_power_level(uint8_t power)
{
	if (power >= TX_PWR_MIN && power <= TX_PWR_MAX) {
		rf230_set_txpower(power);
		pib.transmitPower = rf230_get_txpower();
		if (pib.transmitPower != power)
			print_debug("Transmit Power %u has not been correctly set! Transceiver power is set to %u!", power, pib.transmitPower);
		return phy_SUCCESS;
	} else
		return phy_INVALID_PARAMETER;
}

/**
 * @brief Set the CCA mode
 *
 * @param mode to be set
 * @param thres for the cca modes
 *
 * @return PHY state phy_SUCCESS or phy_INVALID_PARAMETER
 */
phy_state
radio_set_cca_mode(uint8_t mode, uint8_t thres)
{
	/* todo */
	if (mode != 1) {
		print_debug("Currently only CCA Mode 1 supported!");
		return phy_INVALID_PARAMETER;
	} else
		return phy_SUCCESS;
}

/**
 * @brief Set the TRX state
 *
 * @param state to be set
 *
 * @return PHY state phy_SUCCESS, phy_RX_ON, phy_TRX_OFF, or phy_TX_ON
 */
phy_state
radio_set_trx_state(phy_state state)
{
	/* todo */
	print_debug("Set TRX state currently unsupported!");
//	radio_status_t avr_state = RX_ON;
//	return radioStatusToPhyState(avr_state);
	return phy_SUCCESS;
}

/**
 * @brief Perform a Clear Channel Assessment
 *
 * @return PHY state phy_IDLE, phy_TRX_OFF, or phy_BUSY
 */
phy_state
radio_perform_cca()
{
	if (radio_get_trx_state()==phyStateToRadioState(phy_TRX_OFF))
		return phy_TRX_OFF;
	else if (!NETSTACK_RADIO.channel_clear())
		return phy_BUSY;
	else
		return phy_IDLE;
}

/**
 * @brief Perform an Energy Detection
 *
 * @param energyLevel to write the result into
 *
 * @return PHY state phy_SUCCESS, phy_TRX_OFF, or phy_TX_ON
 */
phy_state
radio_perform_ed(uint8_t *energyLevel)
{
	/* todo  */
	print_debug("Perform ED currently unsupported by radio driver!");
	*energyLevel = 0xFF;
	return phy_SUCCESS;
}

/**
 * @brief Send data via the radio interface
 *
 * @param data to be send
 *
 * @return PHY state phy_SUCCESS, phy_RX_ON, phy_TRX_OFF or phy_BUSY_TX
 */
phy_state
radio_send(uint8_t * data, uint8_t length)
{
//	print_debug("radio_send length %u:", length);
//	const uint8_t *ptr = data;
//	uint16_t i;
//	for (i=0; i<length; i++ ) {
//		printf("%X, ", *ptr++);
//	}

	packetbuf_copyfrom(data, length);
	uint8_t result = NETSTACK_RADIO.send(packetbuf_hdrptr(), packetbuf_totlen());
	packetbuf_clear();

	return radioRetValueToPhyState(result);
}
