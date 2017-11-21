/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * ATmega128RFA1 IEEE 802.15.4 radio driver
 *
 */

#include "atmega128rfa1.h"

#define DEBUG 1
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
	/* todo */
	return phy_SUCCESS;
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
	/* todo */
	return phy_SUCCESS;
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
	// todo: phy_state mapping to avr states
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
	/* todo */
	print_debug("Performing CCA...");
	return phy_IDLE; // TRX_OFF, BUSY, or IDLE
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
	print_debug("Performing ED...");
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
radio_send(uint8_t * data)
{
	/* todo: send data via radio*/
	//rf230_send((uint8_t *)msg++, datarequest->psduLength);

	/* todo: or via contiki's netstack*/
	//packetbuf_copyfrom((uint8_t *)data,
	//NETSTACK_RADIO.send(packetbuf_hdrptr(), length);
	//packetbuf_clear();

	/* old callback */
	//uint8_t length = packetbuf_totlen();
	//NETSTACK_RADIO.send(packetbuf_hdrptr(), length);
	//packetbuf_clear();
	return phy_SUCCESS;
}
