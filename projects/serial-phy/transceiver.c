/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * IEEE 802.15.4 PHY message passing interface
 *
 */

#include <stdint.h>

#include "contiki.h"
#include "dev/pcapng.h"
#include "dev/pcapng-line.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "phy_services.h"
#include "atmega128rfa1.h"
#include "sys/clock.h"
#include <stdio.h>

#define DEBUG 1
#if DEBUG
#define print_debug printf
#else
#define print_debug(...)
#endif

/*---------------------------------------------------------------------------*/
//static uint8_t[128] *buffer;
/*---------------------------------------------------------------------------*/
PROCESS(transceiver_init, "transceiver_init");
AUTOSTART_PROCESSES(&transceiver_init);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*
 * initialization
 */
void phy_init()
{
	/* init pcap interfaces */
	//pcapng_line_write_shb();
	//pcapng_init();
	//pcapng_registerIDB(DLT_IEEE802_15_4_PHY);
	//pcapng_registerIDB(DLT_IEEE802_15_4_NO_FCS);

	/* init radio module*/
	radio_init();
}

/*---------------------------------------------------------------------------*/
/*
 * send message to external upper layer
 */
void send(const void * data, uint16_t length)
{
	pcap_write_frame(&data, length);
}

/*---------------------------------------------------------------------------*/
/*
 * receive frame from radio interface
 */
void receive()
{
	static uint16_t length;
	static uint8_t buffer[PACKETBUF_SIZE + PACKETBUF_HDR_SIZE];

	/* copy frame to buffer */
	length = packetbuf_copyto(buffer);

	/* create phy service primitive */
	struct pd_data_ind dataindication;
	dataindication.header.spid = PD_DATA_INDICATION;
	dataindication.header.length = SIZEOF_PD_DATA_INDICATION + length;
	dataindication.psduLength = length;
	dataindication.ppduLinkQuality = radio_get_rssi_threshold() + packetbuf_attr(PACKETBUF_ATTR_RSSI);

	/* send indication */
	pcap_write_frame(&dataindication, SIZEOF_PD_DATA_INDICATION);
	serial_write(&buffer, length);

	/* clear packetbuf */
	packetbuf_clear();
}

// todo: pcap_logMsg
// PCAPNG for log messages via Custom Block (CB)
// create virtual Interfaces to enable regular MAC and phy primitives
// SHB, IDB0 - regular MAC, IDB1 - PHY service primitives (different link types)
// live Debugging interface forwarder?
// motivation: simple debugging using wireshark & co.

void get_phyPIB(phy_pib_attr attr)
{
	struct plme_get_conf getconfirm;
	getconfirm.header.spid = PLME_GET_CONFIRM;
	getconfirm.status = phy_SUCCESS;
	getconfirm.attribute = attr;

	switch (attr) {
	case phyCurrentChannel:
	{
		// todo: determine attribute value
		uint8_t attr; // = rf230_get_channel();

		getconfirm.header.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		pcap_write_frame(&getconfirm, SIZEOF_PLME_GET_CONFIRM);
		serial_write(&attr, sizeof(attr));
	}
	break;

	case phyChannelsSupported:
	{
		// todo: determine attribute value
		uint8_t attr; // = radio_get_operating_channel();

		getconfirm.header.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		pcap_write_frame(&getconfirm, SIZEOF_PLME_GET_CONFIRM);
		serial_write(&attr, sizeof(attr));
	}
		break;

	case phyTransmitPower:
	{
		// todo: determine attribute value
		uint8_t attr; // = radio_get_tx_power_level();

		getconfirm.header.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		pcap_write_frame(&getconfirm, SIZEOF_PLME_GET_CONFIRM);
		serial_write(&attr, sizeof(attr));
	}
		break;

	case phyCCAMode:
	{
		// todo: determine attribute value
		uint8_t attr = radio_get_cca_mode();

		getconfirm.header.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		pcap_write_frame(&getconfirm, SIZEOF_PLME_GET_CONFIRM);
		serial_write(&attr, sizeof(attr));
	}
		break;

	case phyCurrentPage:
	{
		// todo: determine attribute value
		uint8_t attr;	//rf230_get_channel();

		getconfirm.header.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		pcap_write_frame(&getconfirm, SIZEOF_PLME_GET_CONFIRM);
		serial_write(&attr, sizeof(attr));
	}
		break;
	case phyMaxFrameDuration:
	{
		// todo: determine attribute value
		uint8_t attr;	//rf230_get_channel();

		getconfirm.header.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		pcap_write_frame(&getconfirm, SIZEOF_PLME_GET_CONFIRM);
		serial_write(&attr, sizeof(attr));
	}
		break;

	case phySHRDuration:
	{
		// todo: determine attribute value
		uint8_t attr;	//rf230_get_channel();

		getconfirm.header.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		pcap_write_frame(&getconfirm, SIZEOF_PLME_GET_CONFIRM);
		serial_write(&attr, sizeof(attr));
	}
		break;

	case phySymbolsPerOctet:
	{
		// todo: determine attribute value
		uint8_t attr;	//rf230_get_channel();

		getconfirm.header.length = SIZEOF_PLME_GET_CONFIRM + sizeof(attr);
		pcap_write_frame(&getconfirm, SIZEOF_PLME_GET_CONFIRM);
		serial_write(&attr, sizeof(attr));
	}
		break;

	default:
	{
		print_debug("[802154_Serial_PHY]: PLME_SET_REQUEST(STATUS) %s unsupported\n", phyAttrToString(attr));

		getconfirm.header.length = SIZEOF_PLME_GET_CONFIRM;
		getconfirm.status = phy_UNSUPPORT_ATTRIBUTE;
		pcap_write_frame(&getconfirm, SIZEOF_PLME_GET_CONFIRM);
	}
		break;
	}
}

void set_phyPIB(enum phy_pib_attr attr, void *value)
{
	struct plme_set_conf setconfirm;
	setconfirm.header.spid = PLME_SET_CONFIRM;
	setconfirm.header.length = SIZEOF_PLME_SET_CONFIRM;
	setconfirm.status = phy_SUCCESS;	// phy_SUCCESS, phy_READ_ONLY, phy_UNSUPPORTED_ATTRIBUTE, INVALID_INDEX or phy_INVALID_PARAMETER
	setconfirm.attribute = attr;

	switch (attr) {
	case phyCurrentChannel:
	{
		setconfirm.status = radio_set_operating_channel(*((uint8_t *)value));
	}
		break;

	case phyChannelsSupported:
	{
		setconfirm.status = phy_READ_ONLY;	// read-only according to the specification
	}
		break;

	case phyTransmitPower:
	{
		setconfirm.status = radio_set_tx_power_level(*((uint8_t *)value));
	}
		break;

	case phyCCAMode:
	{
		setconfirm.status = radio_set_cca_mode(*((uint8_t *)value), *((uint8_t *)value++));
	}
		break;

	case phyCurrentPage:
	{
		setconfirm.status = radio_set_current_page(*((uint8_t *)value));
	}
		break;
	case phyMaxFrameDuration:
	{
		setconfirm.status = phy_READ_ONLY;	// read-only according to the specification
	}
		break;

	case phySHRDuration:
	{
		setconfirm.status = phy_READ_ONLY;	// read-only according to the specification
	}
		break;

	case phySymbolsPerOctet:
	{
		setconfirm.status = phy_READ_ONLY;	// read-only according to the specification
	}
		break;

	default:
	{
		setconfirm.status = phy_UNSUPPORT_ATTRIBUTE;
		print_debug("[802154_Serial_PHY]: PLME_SET_REQUEST(STATUS) %s unsupported\n", phyAttrToString(attr));
	}
		break;
	}

	send(&setconfirm, SIZEOF_PLME_SET_CONFIRM);
}

void set_trx_state(enum phyState set_state)
{
	struct plme_set_trx_state_conf  settrxstateconfirm;
	settrxstateconfirm.header.spid = PLME_SET_TRX_STATE_CONFIRM;
	settrxstateconfirm.header.length = SIZEOF_PLME_SET_TRX_STATE_CONFIRM;
	settrxstateconfirm.status = phy_SUCCESS;	// phy_SUCCESS, phy_RX_ON, phy_TRX_OFF or phy_TX_ON

	// todo: read actual state
	//phy_state state = radio_get_trx_state();

	// todo: set state
	//state = radio_set_trx_state(set_state); // phy_RX_ON, phy_TRX_OFF, phy_FORCE_TRX_OFF or phy_TX_ON

	// todo: phy_state mapping to avr states
	print_debug("[802154_Serial_PHY]: PLME_SET_TRX_STATE_REQUEST(STATUS) %s unsupported\n", phyStateToString(set_state));

	send(&settrxstateconfirm, SIZEOF_PLME_SET_TRX_STATE_CONFIRM);
}

/*
 * handling of incoming messages
 */
void handleMessage(uint8_t * msg)
{
	/* check and decode service primitive type and length */
	struct pd_plme_hdr *header;
	header = (struct pd_plme_hdr *)msg;

	print_debug("[802154_Serial_PHY]: Got Message %s, length %u\n", msgTypeToString(header->spid), header->length);

	switch (header->spid) {
	case PD_DATA_REQUEST:
	{
		struct pd_data_req *datarequest;
		struct pd_data_conf dataconfirm;
		datarequest = (struct pd_data_req *)msg;

		print_debug("[802154_Serial_PHY]: PD_DATA_REQUEST(PSDULENGTH): %u\n", datarequest->psduLength);

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

		// send confirmation
		dataconfirm.header.spid = PD_DATA_CONFIRM;
		dataconfirm.header.length = SIZEOF_PD_DATA_CONFIRM;
		dataconfirm.status = phy_SUCCESS; // todo: phy_SUCCESS, phy_RX_ON, phy_TRX_OFF or phy_BUSY_TX
		send(&dataconfirm, SIZEOF_PD_DATA_CONFIRM);
	}
		break;

	case PLME_CCA_REQUEST:
	{
		struct plme_cca_req *ccarequest;
		struct plme_cca_conf ccaconfirm;
		ccarequest = (struct plme_cca_req *)msg;

		print_debug("[802154_Serial_PHY]: PLME_CCA_REQUEST(%u)\n", header->spid);

		// send confirmation
		ccaconfirm.header.spid = PLME_CCA_CONFIRM;
		ccaconfirm.header.length = SIZEOF_PLME_CCA_CONFIRM;
		ccaconfirm.status = phy_TRX_OFF; // todo: phy_TRX_OFF, phy_BUSY or phy_IDLE
		send(&ccaconfirm, SIZEOF_PLME_CCA_CONFIRM);
	}
		break;

	case PLME_ED_REQUEST:
	{
		struct plme_ed_req *edrequest;
		struct plme_ed_conf edconfirm;
		edrequest = (struct plme_ed_req *)msg;

		print_debug("[802154_Serial_PHY]: PLME_ED_REQUEST(%u)\n", header->spid);

		// send confirmation
		edconfirm.header.spid = PLME_ED_CONFIRM;
		edconfirm.header.length = SIZEOF_PLME_ED_CONFIRM;
		edconfirm.status = phy_SUCCESS;	// todo: phy_SUCCESS, phy_TRX_OFF or phy_TX_ON
		edconfirm.energyLevel = 0x00;	// todo: energy level
		send(&edconfirm, SIZEOF_PLME_ED_CONFIRM);
	}
		break;

	case PLME_GET_REQEST:
	{
		struct plme_get_req *getrequest;
		getrequest = (struct plme_get_req *)msg;

		print_debug("[802154_Serial_PHY]: PLME_GET_REQUEST(ATTRIBUTE) %s\n", phyAttrToString(getrequest->attribute));

		get_phyPIB(getrequest->attribute);
	}
		break;

	case PLME_SET_TRX_STATE_REQUEST:
	{
		struct plme_set_trx_state_req  *settrxstaterequest;
		settrxstaterequest = (struct plme_set_trx_state_req  *)msg;

		print_debug("[802154_Serial_PHY]: PLME_SET_TRX_STATE_REQUEST(STATUS) %s\n", phyStateToString(settrxstaterequest->status));

		set_trx_state(settrxstaterequest->status);
	}
	break;

	case PLME_SET_REQUEST:
	{
		struct plme_set_req *setrequest;
		setrequest = (struct plme_set_req *)msg;

		print_debug("[802154_Serial_PHY]: PLME_SET_REQUEST(ATTRIBUTE) %s\n", phyAttrToString(setrequest->attribute));

		set_phyPIB(setrequest->attribute, (void *)msg + SIZEOF_PLME_SET_REQUEST);
	}
		break;

	default:
		print_debug("[802154_Serial_PHY]: Received Message with unknown message type %i!\n", header->spid);
		break;
	}
}


PROCESS_THREAD(transceiver_init, ev, data)
{
	static struct pcapng_section_header_block_t section;
	static struct pcapng_interface_description_block_t interface;
	static struct pcapng_enhanced_packet_block_t packet;

	const uint8_t *ptr;
	uint16_t i;


	PROCESS_BEGIN();

	/* starting */
	print_debug("[802154_Serial_PHY]: Starting...\n");
	//leds_on(1);

	/* init module */
	phy_init();

	/* wait for pcapng stream init */
	PROCESS_WAIT_EVENT_UNTIL(ev == pcapng_event_shb);
	pcapng_line_read_shb(&section, (uint8_t *)data);
//	section = (struct pcapng_section_header_block_t *)(data + sizeof(struct pcapng_block_header_t));
	print_debug("Section (Magic: %" PRIu32 ", Version: %" PRIu16 ".%" PRIu16 ", Length: %lu)\n",
			section.magic,
			section.version_major,
			section.version_minor,
			section.section_length
	);

	/* wait for interface description */
	PROCESS_WAIT_EVENT_UNTIL(ev == pcapng_event_idb);
	pcapng_line_read_idb(&interface, (uint8_t *)data);
//	interface = (struct pcapng_interface_describtion_block_t *)(data + sizeof(struct pcapng_block_header_t));
	print_debug("Interface 0 (Linktype: %" PRIu16 ", Snaplen: %" PRIu32 ")\n",
			interface.linktype,
			interface.snaplen
	);

	print_debug("Section (Magic: %" PRIu32 ", Version: %" PRIu16 ".%" PRIu16 ", Length: %lu)\n",
				section.magic,
				section.version_major,
				section.version_minor,
				section.section_length
		);

	/* wait for packet input */
	while (1) {
		memset(&packet, 0, sizeof(packet));
		PROCESS_WAIT_EVENT_UNTIL(ev == pcapng_event_epb);

		ptr = (uint8_t *)data;
		i = 0;

		print_debug("HEXDUMP: ");
		for (i=0; i<sizeof(packet); i++ ) {
			print_debug("0x%02x, ", *ptr++);
		}
		print_debug("\n");

		pcapng_line_read_epb(&packet, (uint8_t *)data);
//		packet = (struct pcapng_enhanced_packet_block_t *)(data + sizeof(struct pcapng_block_header_t));
		print_debug("Packet (Interface: %" PRIu32 ", Time: %" PRIu32 ".%" PRIu32 ",Length: %" PRIu32 ")\n",
				packet.interface_id,
				packet.timestamp_high,
				packet.timestamp_low,
				packet.packet_len
		);
		/* handle encapsulated message */
		handleMessage((uint8_t *)data + sizeof(pcapng_block_header_s) + sizeof(pcapng_enhanced_packet_block_s));
	}

	PROCESS_END();
}
