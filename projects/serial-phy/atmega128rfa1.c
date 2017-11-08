/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * ATmega128RFA1 IEEE 802.15.4 radio driver
 *
 */

#include <stdint.h>

#include "phy_services.h"

/*---------------------------------------------------------------------------*/
/* energy level resolution 0-83, ED sensitivity -90dBm, steps are 1dBm PHY_ED_LEVEL
 * ATmega128RFA1 datasheet - Figure 9-18
 */
#define THRESHOLD 		-90

static struct PhyPIB *pib;

void radio_init()
{
	//pib = (struct PhyPIB*) malloc(sizeof(struct PhyPIB));
	pib->phyCurrentChannel = 24;
	//pib->phyChannelsSupported = ;
	pib->phyTransmitPower = 0;
	pib->phyCCAMode = 0;
	pib->phyCurrentPage = 0;
	pib->phyMaxFrameDuration = 0;
	pib->phySHRDuration = 0;
	pib->phySymbolsPerOctet = 0;
}

int8_t radio_get_rssi_threshold()
{
	return THRESHOLD;
}

uint8_t radio_get_operating_channel()
{
	return pib->phyCurrentChannel;
}

enum phyState radio_set_operating_channel(uint8_t channel)
{
	/* todo */
	return phy_SUCCESS;
}

//uint8_t radio_get_channels_supported()
//{
//	return pib->phyChannelsSupported;
//}

uint8_t radio_get_tx_power_level()
{
	return pib->phyTransmitPower;
}

enum phyState radio_set_tx_power_level(uint8_t power)
{
	/* todo */
	return phy_SUCCESS;
}

uint8_t radio_get_cca_mode()
{
	return pib->phyCCAMode;
}

enum phyState radio_set_cca_mode(uint8_t mode, uint8_t thres)
{
	/* todo */
	return phy_SUCCESS;
}

uint8_t radio_get_current_page()
{
	return pib->phyCurrentPage;
}

enum phyState radio_set_current_page(uint8_t page)
{
	/* todo */
	return phy_READ_ONLY;
}

uint16_t radio_get_max_frame_duration()
{
	return pib->phyMaxFrameDuration;
}

uint8_t radio_get_shr_duration()
{
	return pib->phySHRDuration;
}

float radio_get_symbols_per_octet()
{
	return pib->phySymbolsPerOctet;
}
