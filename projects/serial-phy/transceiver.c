/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * IEEE 802.15.4 PHY message passing interface
 *
 */

#include <stdint.h>

#include "contiki.h"
#include "leds.h"
#include "dev/pcapng.h"
#include "dev/pcapng-line.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "phy_service.h"
#include "atmega128rfa1.h"
#include "sys/clock.h"
#include <stdio.h>

#define DEBUG 1
#if DEBUG && DEBUG == 1
#define print_debug(fmt, args...) printf("[TRANSCEIVER]: " fmt "\n", ##args)
#elif DEBUG && DEBUG == 2
#define print_debug(fmt, args...) printf("DEBUG: %s:%d: " fmt, \
    __FILE__, __LINE__, ##args)
#else
#define print_debug(...)
#endif

#define IEEE802154_MAC_INTERFACE	0
#define IEEE802154_PHY_INTERFACE	1

/*---------------------------------------------------------------------------*/
PROCESS(transceiver_init, "transceiver_init");
AUTOSTART_PROCESSES(&transceiver_init);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*
 * initialization
 */
void initialize()
{
	print_debug("initialize");

	/* status led */
	//leds_on(1);

	/* init pcap interfaces */
	pcapng_line_write_shb();
	pcapng_line_write_idb(DLT_IEEE802_15_4_NO_FCS, DLT_IEEE802_15_4_LEN);
	pcapng_line_write_idb(DLT_IEEE802_15_4_PHY, DLT_IEEE802_15_4_LEN);

	/* init radio module*/
	radio_init();
}

// todo: pcap_logMsg
// PCAPNG for log messages via Custom Block (CB)
// create virtual Interfaces to enable regular MAC and phy primitives
// SHB, IDB0 - regular MAC, IDB1 - PHY service primitives (different link types)
// live Debugging interface forwarder?
// motivation: simple debugging using wireshark & co.

void get_phyPIB(phy_pib_attr attr)
{
	PHY_msg conf;
	conf.type = PLME_GET_CONFIRM;
	conf.x.get_conf.status = phy_SUCCESS;
	conf.x.get_conf.attribute = attr;

	switch (attr) {
	case phyCurrentChannel:
	{
		// todo: determine attribute value
		uint8_t attr; // = rf230_get_channel();

		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		send_msg(&conf);
	}
	break;

	case phyChannelsSupported:
	{
		// todo: determine attribute value
		uint8_t attr; // = radio_get_operating_channel();

		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		send_msg(&conf);
	}
		break;

	case phyTransmitPower:
	{
		// todo: determine attribute value
		uint8_t attr = radio_get_tx_power_level();

		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		send_msg(&conf);
	}
		break;

	case phyCCAMode:
	{
		// todo: determine attribute value
		uint8_t attr = radio_get_cca_mode();

		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		send_msg(&conf);
	}
		break;

	case phyCurrentPage:
	{
		// todo: determine attribute value
		uint8_t attr;	//rf230_get_channel();

		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		send_msg(&conf);
	}
		break;
	case phyMaxFrameDuration:
	{
		// todo: determine attribute value
		uint8_t attr;	//rf230_get_channel();

		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		send_msg(&conf);
	}
		break;

	case phySHRDuration:
	{
		// todo: determine attribute value
		uint8_t attr;	//rf230_get_channel();

		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		send_msg(&conf);
	}
		break;

	case phySymbolsPerOctet:
	{
		// todo: determine attribute value
		uint8_t attr;	//rf230_get_channel();

		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		send_msg(&conf);
	}
		break;

	default:
	{
		conf.length = SIZEOF_PLME_GET_CONFIRM;
		conf.x.get_conf.status = phy_UNSUPPORT_ATTRIBUTE;
		send_msg(&conf);
	}
		break;
	}
}

void set_phyPIB(phy_pib_attr attr, void *value)
{
	PHY_msg conf;
	conf.type = PLME_SET_CONFIRM;
	conf.length = SIZEOF_PLME_SET_CONFIRM;
	conf.x.set_conf.status = phy_SUCCESS; // phy_SUCCESS, phy_READ_ONLY, phy_UNSUPPORTED_ATTRIBUTE, INVALID_INDEX or phy_INVALID_PARAMETER
	conf.x.set_conf.attribute = attr;

	switch (attr) {
	case phyCurrentChannel:
		conf.x.set_conf.status = radio_set_operating_channel(*((uint8_t *)value));
		break;
	case phyChannelsSupported:
		conf.x.set_conf.status = phy_READ_ONLY;	// read-only according to the specification
		break;
	case phyTransmitPower:
		conf.x.set_conf.status = radio_set_tx_power_level(*((uint8_t *)value));
		break;
	case phyCCAMode:
		conf.x.set_conf.status = radio_set_cca_mode(*((uint8_t *)value), *((uint8_t *)value+1));
		break;
	case phyCurrentPage:
		conf.x.set_conf.status = radio_set_current_page(*((uint8_t *)value));
		break;
	case phyMaxFrameDuration:
		conf.x.set_conf.status = phy_READ_ONLY;	// read-only according to the specification
		break;
	case phySHRDuration:
		conf.x.set_conf.status = phy_READ_ONLY;	// read-only according to the specification
		break;
	case phySymbolsPerOctet:
		conf.x.set_conf.status = phy_READ_ONLY;	// read-only according to the specification
		break;
	default:
		conf.x.set_conf.status = phy_UNSUPPORT_ATTRIBUTE;
		break;
	}

	send_msg(&conf);
}

void set_trx_state(enum phyState set_state)
{
	// todo: read actual state
	//phy_state state = radio_get_trx_state();

	// todo: set state
	//state = radio_set_trx_state(set_state); // phy_RX_ON, phy_TRX_OFF, phy_FORCE_TRX_OFF or phy_TX_ON

	// todo: phy_state mapping to avr states

	PHY_msg conf;
	conf.type = PLME_SET_TRX_STATE_CONFIRM;
	conf.length = SIZEOF_PLME_SET_TRX_STATE_CONFIRM;
	conf.x.set_trx_state_conf.status = phy_SUCCESS;	// phy_SUCCESS, phy_RX_ON, phy_TRX_OFF or phy_TX_ON
	send_msg(&conf);
}

/*---------------------------------------------------------------------------*/
/*
 * receive frame from radio interface
 */
void receive()
{
	print_debug("packet received");

	static uint16_t length;
	static uint8_t buffer[PACKETBUF_SIZE + PACKETBUF_HDR_SIZE];

	/* copy frame to buffer */
	length = packetbuf_copyto(buffer);

	/* create phy service primitive */
	PHY_msg ind;
	ind.type = PD_DATA_INDICATION;
	ind.length = SIZEOF_PD_DATA_INDICATION + length;
	ind.x.data_ind.psduLength = length;
	ind.x.data_ind.ppduLinkQuality = radio_get_rssi_threshold() + packetbuf_attr(PACKETBUF_ATTR_RSSI);
	memcpy(&ind.x.data_ind.data, &buffer, length);

	/* send indication */
	send_msg(&ind);

	/* clear packetbuf */
	packetbuf_clear();
}

/*
 * handling of incoming messages
 */
void handleMessage(PHY_msg * msg)
{
	print_msg(msg, "received");

	switch (msg->type) {
	case PD_DATA_REQUEST:
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

		PHY_msg conf;
		conf.type = PD_DATA_CONFIRM;
		conf.length = SIZEOF_PD_DATA_CONFIRM;
		conf.x.data_conf.status = phy_SUCCESS; // todo: phy_SUCCESS, phy_RX_ON, phy_TRX_OFF or phy_BUSY_TX
		send_msg(&conf);
	}
		break;

	case PLME_CCA_REQUEST:
	{
		PHY_msg conf;
		conf.type = PLME_CCA_CONFIRM;
		conf.length = SIZEOF_PLME_CCA_CONFIRM;
		conf.x.cca_conf.status = phy_TRX_OFF; // todo: phy_TRX_OFF, phy_BUSY or phy_IDLE
		send_msg(&conf);
	}
		break;

	case PLME_ED_REQUEST:
	{
		PHY_msg conf;
		conf.type = PLME_ED_CONFIRM;
		conf.length = SIZEOF_PLME_ED_CONFIRM;
		conf.x.ed_conf.status = phy_SUCCESS;	// todo: phy_SUCCESS, phy_TRX_OFF or phy_TX_ON
		conf.x.ed_conf.energyLevel = 0x00;	// todo: energy level
		send_msg(&conf);
	}
		break;

	case PLME_GET_REQEST:
	{
		get_phyPIB(msg->x.get_req.attribute);
	}
		break;

	case PLME_SET_TRX_STATE_REQUEST:
	{
		set_trx_state(msg->x.set_trx_state_req.status);
	}
	break;

	case PLME_SET_REQUEST:
	{
		set_phyPIB(msg->x.set_req.attribute, &msg->x.set_req.value);
	}
		break;

	default:
		print_debug("Message type %i unsupported!", msg->type);
		break;
	}
	print_debug("\n");
}


PROCESS_THREAD(transceiver_init, ev, data)
{
	static pcapng_section_header_block_s section;
	static pcapng_interface_description_block_s interface;
	static pcapng_enhanced_packet_block_s packet;
	static PHY_msg msg;

	PROCESS_BEGIN();

	/* init module */
	initialize();

	/* wait for pcapng stream init */
	PROCESS_WAIT_EVENT_UNTIL(ev == pcapng_event_shb);
	pcapng_line_read_shb((uint8_t *)data, &section);
	print_debug("Section (Magic: %" PRIu32 ", Version: %" PRIu16 ".%" PRIu16 ", Length: %llu)",
			section.magic,
			section.version_major,
			section.version_minor,
			section.section_length
	);

	/* wait for interface description */
	PROCESS_WAIT_EVENT_UNTIL(ev == pcapng_event_idb);
	pcapng_line_read_idb((uint8_t *)data, &interface);
	print_debug("Interface 0 (Linktype: %" PRIu16 ", Snaplen: %" PRIu32 ")",
			interface.linktype,
			interface.snaplen
	);
	/* wait for packet input */
	while (1) {
		memset(&packet, 0, sizeof(packet));
		memset(&msg, 0, sizeof(msg));
		PROCESS_WAIT_EVENT_UNTIL(ev == pcapng_event_epb);
		pcapng_line_read_epb((uint8_t *)data, &packet);
		print_debug("Packet (Interface: %" PRIu32 ", Time: %" PRIu32 ".%" PRIu32 ",Length: %" PRIu32 ")",
				packet.interface_id,
				packet.timestamp_high,
				packet.timestamp_low,
				packet.packet_len
		);
		/* deserialize and handle encapsulated message */
		deserialize_msg((uint8_t *)data + sizeof(pcapng_block_header_s) + sizeof(pcapng_enhanced_packet_block_s), &msg);
		handleMessage(&msg);
	}

	PROCESS_END();
}
