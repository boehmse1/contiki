/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * IEEE 802.15.4 PHY message passing interface
 *
 */

#include <stdint.h>

#include "contiki.h"
#include "leds.h"
#include "dev/phy.h"
#include "dev/pcapng.h"
#include "dev/pcapng-line.h"
#include "dev/serial-phy.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "sys/clock.h"
#include "pt-sem.h"
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

pcap_timeval_s SIMULATION_TIMER_OFFSET_US = { .ts_sec = 0, .ts_usec = 500 };
pcap_timeval_s START_TIME_OFFSET = { .ts_sec = 0, .ts_usec = 0 };

rtimer_clock_t rtime;
clock_time_t sim_time;
pcap_timeval_s simtime;

//#define IEEE802154_PHY_INTERFACE	0
//#define IEEE802154_MAC_INTERFACE	1

/*---------------------------------------------------------------------------*/
PROCESS(transceiver_init, "transceiver_init");
AUTOSTART_PROCESSES(&transceiver_init);
/*---------------------------------------------------------------------------*/

/**
 * @brief initialization of the transceiver
 */
void initialize()
{
	print_debug("initialize");

	/* register for pcapng events */
	pcapng_line_register_consumer_process(&transceiver_init);

	packetbuf_clear();

	/* status led */
	//leds_on(1);

	/* init pcap interfaces */
	pcapng_line_write_shb();
	pcapng_line_write_idb(DLT_IEEE802_15_4_PHY, DLT_IEEE802_15_4_LEN);
	//pcapng_line_write_idb(DLT_IEEE802_15_4_NO_FCS, DLT_IEEE802_15_4_LEN);

// todo: pcap_logMsg
// PCAPNG for log messages via Custom Block (CB)
// create virtual Interfaces to enable regular MAC and phy primitives
// SHB, IDB0 - regular MAC, IDB1 - PHY service primitives (different link types)
// live Debugging interface forwarder?
// motivation: simple debugging using wireshark & co.
}

pcap_timeval_s clock_time_add(pcap_timeval_s *a, pcap_timeval_s *b)
{
    pcap_timeval_s res;
    res.ts_sec = a->ts_sec + b->ts_sec;
    res.ts_usec = a->ts_usec + b->ts_usec;
    if (res.ts_usec>1000000) {
        res.ts_sec++;
        res.ts_usec -= 1000000;
    }
    return res;
}

pcap_timeval_s clock_time_subtract(pcap_timeval_s *a,pcap_timeval_s *b)
{
    pcap_timeval_s res;
    res.ts_sec = a->ts_sec - b->ts_sec;
    res.ts_usec = a->ts_usec - b->ts_usec;
    if (res.ts_usec<0) {
        res.ts_sec--;
        res.ts_usec += 1000000;
    }
    return res;
}

uint32_t clock_time_diff_usec(pcap_timeval_s *a,pcap_timeval_s *b)
{
    uint32_t sec = a->ts_sec - b->ts_sec;
    uint32_t usec = a->ts_usec - b->ts_usec;
    return sec*1000000L + usec;
}

uint8_t clock_time_greater(pcap_timeval_s *a,pcap_timeval_s *b)
{
    if (a->ts_sec==b->ts_sec)
        return a->ts_usec > b->ts_usec;
    else
        return (uint32_t)a->ts_sec > (uint32_t)b->ts_sec;
}


void get_rtime(pcap_timeval_s *time)
{
    rtime = rtimer_arch_now();
    time->ts_usec = 1000000 * (rtime % RTIMER_ARCH_SECOND) / RTIMER_ARCH_SECOND;
    time->ts_sec = (rtime - (rtime % RTIMER_ARCH_SECOND)) / RTIMER_ARCH_SECOND;
}

void get_time(pcap_timeval_s *time)
{
    sim_time = clock_time();
    time->ts_usec = 1000000 * (sim_time % CLOCK_SECOND) / CLOCK_SECOND;
    time->ts_sec = (sim_time - (sim_time % CLOCK_SECOND)) / CLOCK_SECOND;
}

void print_time(pcap_timeval_s *time)
{
    print_debug("Clock Time: %" PRIu32 ".%" PRIu32, time->ts_sec, time->ts_usec);
}

void print_rtime(rtimer_clock_t *time)
{
    print_debug("Arch RTime: %u", *time);
}

void get_time_with_offset(pcap_timeval_s *time)
{
    pcap_timeval_s now, offset_now;
    get_time(&now);
    offset_now = clock_time_subtract(&now, &START_TIME_OFFSET);
    offset_now = clock_time_subtract(&offset_now, &SIMULATION_TIMER_OFFSET_US);
    time->ts_usec = offset_now.ts_usec;
    time->ts_sec = offset_now.ts_sec;
}

void set_time_start_offset()
{
    get_time(&START_TIME_OFFSET);
}

void set_event_time(pcap_timeval_s *time, pcapng_enhanced_packet_block_s *packet)
{
    pcap_timeval_s event_time;
    pcap_timeval_s pcap_time;
    pcap_time.ts_sec = packet->timestamp_high;
    pcap_time.ts_usec = packet->timestamp_low;
    event_time = clock_time_add(&pcap_time, &SIMULATION_TIMER_OFFSET_US);
    time->ts_usec = event_time.ts_usec;
    time->ts_sec = event_time.ts_sec;

}

void wait_until(pcap_timeval_s *targetTime)
{
    // if there's more than 200ms to wait, wait in 10ms chunks
    pcap_timeval_s curTime;
    //print_time(targetTime);

    get_time_with_offset(&curTime);

    while (targetTime->ts_sec - curTime.ts_sec >= 2
    || clock_time_diff_usec(targetTime, &curTime) >= 20000) {
        //print_time(&curTime);
        //clock_delay_usec(10000); // 10ms
        rtimer_arch_sleep(10000/64);
        get_time_with_offset(&curTime);
    }
}

/**
 * @brief Get a PHY PIB attribute
 *
 * @param attr to get
 */
void get_attribute(phy_pib_attr attr)
{
	PHY_msg conf;
	conf.type = PLME_GET_CONFIRM;
	conf.x.get_conf.attribute = attr;

	switch (attr) {
	case phyCurrentChannel:
		conf.x.get_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, (radio_value_t *)&conf.x.get_conf.value.currentChannel));
		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(phyAttrCurrentChannel);
		break;
	case phyChannelsSupported:
		conf.x.get_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.get_object(RADIO_PARAM_CHANNELS_SUPPORTED, &conf.x.get_conf.value.channelsSupported, sizeof(phyAttrChannelsSupported)));
		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(phyAttrChannelsSupported);
		break;
	case phyTransmitPower:
		conf.x.get_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, (radio_value_t *)&conf.x.get_conf.value.transmitPower));
		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(phyAttrTransmitPower);
		break;
	case phyCCAMode:
		conf.x.get_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.get_value(RADIO_PARAM_CCA_MODE, (radio_value_t *)&conf.x.get_conf.value.cCAMode));
		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(phyAttrCCAMode);
		break;
	case phyCurrentPage:
		conf.x.get_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.get_value(RADIO_PARAM_CURRENT_PAGE, (radio_value_t *)&conf.x.get_conf.value.currentPage));
		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(phyAttrCurrentPage);
		break;
	case phyMaxFrameDuration: {
		conf.x.get_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.get_object(RADIO_PARAM_MAX_FRAME_DURATION, &conf.x.get_conf.value.maxFrameDuration, sizeof(phyAttrMaxFrameDuration)));
		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(phyAttrMaxFrameDuration);
		break;
	}
	case phySHRDuration:
		conf.x.get_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.get_value(RADIO_PARAM_SHR_DURATION, (radio_value_t *)&conf.x.get_conf.value.sHRDuration));
		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(phyAttrSHRDuration);
		break;
	case phySymbolsPerOctet:
		conf.x.get_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.get_value(RADIO_PARAM_SYMBOLS_PER_OCTET, (radio_value_t *)&conf.x.get_conf.value.symbolsPerOctet));
		conf.length = SIZEOF_PLME_GET_CONFIRM + sizeof(phyAttrSymbolsPerOctet);
		break;
	default:
		conf.length = SIZEOF_PLME_GET_CONFIRM;
		conf.x.get_conf.status = phy_UNSUPPORT_ATTRIBUTE;
		break;
	}
	send_msg(&conf);
}

/**
 * @brief Set a PHY PIB attribute
 *
 * @param attr to be set
 * @param value address for the result
 */
void set_attribute(phy_pib_attr attr, PhyPIB_value *value)
{
	PHY_msg conf;
	conf.type = PLME_SET_CONFIRM;
	conf.length = SIZEOF_PLME_SET_CONFIRM;
	conf.x.set_conf.attribute = attr;

	switch (attr) {
	case phyCurrentChannel:
		conf.x.set_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, value->currentChannel));
		break;
	case phyChannelsSupported:
		conf.x.set_conf.status = phy_READ_ONLY;	// read-only according to the specification
		break;
	case phyTransmitPower:
		conf.x.set_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, value->transmitPower));
		break;
	case phyCCAMode:
		conf.x.set_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_MODE, value->cCAMode));
		break;
	case phyCurrentPage:
		conf.x.set_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.set_value(RADIO_PARAM_CURRENT_PAGE, value->currentPage));
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

/**
 * @brief Receive frame from radio interface
 */
void receive()
{
	print_debug("packet received");

	static uint8_t length;
	static uint8_t buffer[PACKETBUF_SIZE + PACKETBUF_HDR_SIZE];
	static radio_value_t threshold;

	/* copy frame to buffer */
	length = packetbuf_copyto(buffer);

	/* rssi threshold from radio chip */
	NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI_THRESHOLD, &threshold);

	/* create phy service primitive */
	PHY_msg ind;
	ind.type = PD_DATA_INDICATION;
	ind.length = SIZEOF_PD_DATA_INDICATION + length;
	ind.x.data_ind.psduLength = length;
	ind.x.data_ind.ppduLinkQuality = (uint8_t)(threshold + packetbuf_attr(PACKETBUF_ATTR_RSSI));
	//ind.x.data_ind.ppduLinkQuality = (uint8_t)packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);
	memcpy(&ind.x.data_ind.data, &buffer, length);

	/* send indication */
	send_msg(&ind);

	/* clear packetbuf */
	packetbuf_clear();
}

/**
 * @brief handling of a PHY message
 *
 * @param msg address to be handled
 */
void handleMessage(PHY_msg * msg, pcap_timeval_s *targetTime)
{
    pcap_timeval_s now;
    uint16_t usec;
    PHY_msg conf;

    //print_debug("received!");
	//print_msg(msg, "received");

	get_time_with_offset(&now);

	/* init rtimer */
    //rtimer_init();

    usec = clock_time_diff_usec(targetTime, &now);
    if (usec > 0) {
        rtimer_arch_sleep(usec/64);
    }

    print_time(&now);
    print_debug("usec: %i", usec);

	switch (msg->type) {
	case PD_DATA_REQUEST:
		conf.type = PD_DATA_CONFIRM;
		conf.length = SIZEOF_PD_DATA_CONFIRM;
		/* todo: disable CRC adding by the radio driver, no radio return value on ERROR specified! */
		conf.x.data_conf.status = radioRetValueTXToPhyState(NETSTACK_RADIO.send((uint8_t *)&msg->x.data_req.data, msg->x.data_req.psduLength));
		send_msg(&conf);
		break;
	case PLME_CCA_REQUEST:
		conf.type = PLME_CCA_CONFIRM;
		conf.length = SIZEOF_PLME_CCA_CONFIRM;
		/* todo  */
		// return values TRX_OFF, BUSY, or IDLE
		conf.x.cca_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.channel_clear());
		send_msg(&conf);
		break;
	case PLME_ED_REQUEST:
		conf.type = PLME_ED_CONFIRM;
		conf.length = SIZEOF_PLME_ED_CONFIRM;
		/* todo  */
		//conf.x.ed_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.energy_detect(&conf.x.ed_conf.energyLevel));
		print_debug("Perform ED currently unsupported by radio driver!");
		conf.x.ed_conf.status = -1;
		send_msg(&conf);
		break;
	case PLME_GET_REQUEST:
		get_attribute(msg->x.get_req.attribute);
		break;
	case PLME_SET_TRX_STATE_REQUEST:
		conf.type = PLME_SET_TRX_STATE_CONFIRM;
		conf.length = SIZEOF_PLME_SET_TRX_STATE_CONFIRM;
		/* todo  */
		print_debug("Set TRX state currently unsupported by radio driver!");
		conf.x.set_trx_state_conf.status = radioRetValueToPhyState(NETSTACK_RADIO.set_value(RADIO_PARAM_PHY_STATE, msg->x.set_trx_state_req.status));
		send_msg(&conf);
		break;
	case PLME_SET_REQUEST:
		set_attribute(msg->x.set_req.attribute, &msg->x.set_req.value);
		break;
	default:
		print_debug("Message type %s unsupported!", msgTypeToString(msg->type));
		break;
	}
}

PROCESS_THREAD(transceiver_init, ev, data)
{
//	static pcapng_section_header_block_s section;
//	static pcapng_interface_description_block_s interface;
	static pcapng_enhanced_packet_block_s packet;
	static PHY_msg msg;
	static uint8_t packetCounter = 0;
    pcap_timeval_s time;
    pcap_timeval_s time2;
    pcap_timeval_s time3;
    pcap_timeval_s time4;
    rtimer_clock_t rt;
    rtimer_clock_t rt2;
    clock_time_t t;
    clock_time_t t2;
    pcap_timeval_s pcap_time;

	PROCESS_BEGIN();

	/* init module */
	initialize();

    /* print time */
    get_rtime(&time);
    rt = rtime;
    rtimer_arch_sleep(1); // 10us
    get_rtime(&time2);
    rt2 = rtime;

    print_time(&time);
    print_rtime(&rt);
    print_debug(" -- wait 64us -- ");
    print_time(&time2);
    print_rtime(&rt2);
    print_debug(" ");

    /* print time */
    get_time(&time3);
    t = sim_time;
    rtimer_arch_sleep(200000/64); // 200ms
    get_time(&time4);
    t2 = sim_time;

    print_time(&time3);
    print_rtime(&t);
    print_debug(" -- wait 200ms -- ");
    print_time(&time4);
    print_rtime(&t2);
    print_debug(" ");

/* print time */
get_time(&time3);
t = sim_time;
rtimer_arch_sleep(100000/64); // 100ms
get_time(&time4);
t2 = sim_time;

print_time(&time3);
print_rtime(&t);
print_debug(" -- wait 100ms -- ");
print_time(&time4);
print_rtime(&t2);
print_debug(" ");

/* print time */
get_time(&time3);
t = sim_time;
rtimer_arch_sleep(50000/64); // 50ms
get_time(&time4);
t2 = sim_time;

print_time(&time3);
print_rtime(&t);
print_debug(" -- wait 50ms -- ");
print_time(&time4);
print_rtime(&t2);
print_debug(" ");

	/* wait for pcapng stream init */
//	PROCESS_WAIT_EVENT_UNTIL(ev == pcapng_event_shb);
//	pcapng_line_read_shb((uint8_t *)data, &section);
//	print_debug("Section (Magic: %" PRIu32 ", Version: %" PRIu16 ".%" PRIu16 ", Length: %llu)",
//			section.magic,
//			section.version_major,
//			section.version_minor,
//			section.section_length
//	);

	/* wait for interface description */
//	PROCESS_WAIT_EVENT_UNTIL(ev == pcapng_event_idb);
//	pcapng_line_read_idb((uint8_t *)data, &interface);
//	print_debug("Interface 0 (Linktype: %" PRIu16 ", Snaplen: %" PRIu32 ")",
//			interface.linktype,
//			interface.snaplen
//	);

	/* wait for packet input */
	while (1) {
		packetCounter++;
		memset(&packet, 0, sizeof(packet));
		memset(&msg, 0, sizeof(msg));
		PROCESS_WAIT_EVENT_UNTIL(ev == pcapng_event_epb);
		pcapng_line_read_epb((uint8_t *)data, &packet);
		print_debug("Packet %u (Interface: %" PRIu32 ", Time: %" PRIu32 ".%" PRIu32 ",Length: %" PRIu32 ")",
				packetCounter,
				packet.interface_id,
				packet.timestamp_high,
				packet.timestamp_low,
				packet.packet_len
		);

		/* deserialize encapsulated message */
		deserialize_msg((uint8_t *)data + sizeof(pcapng_block_header_s) + sizeof(pcapng_enhanced_packet_block_s), &msg);

		/* start simulation time */
		if (packetCounter==1) {
		    set_time_start_offset();
		}

        /* set event time */
        set_event_time(&pcap_time, &packet);

        /* wait until time arrives */
        get_time_with_offset(&time);

        if (clock_time_greater(&pcap_time, &time)) {
            wait_until(&pcap_time);
            //print_debug("handle message now!");
            handleMessage(&msg, &pcap_time);
        } else {
            print_debug("ERROR: event time is behind execution!.");
            print_time(&time);
        }


	}

	PROCESS_END();
}
