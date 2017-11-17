#include <stdio.h>
#include <string.h>

#include "contiki.h"
#include "phy_service.h"
#include "dev/pcapng.h"
#include "dev/pcapng-line.h"

#define DEBUG 1
#if DEBUG && DEBUG == 1
#define print_debug(fmt, args...) printf("[802154_Serial_PHY]: " fmt, ##args)
#elif DEBUG && DEBUG == 2
#define print_debug(fmt, args...) printf("DEBUG: %s:%d: " fmt, \
    __FILE__, __LINE__, ##args)
#else
#define print_debug(...)
#endif


/**
 * @brief Deserializes a stream of bytes into a PHY message
 *
 * @param stream buffer to decode the PHY message from
 * @param msg points to the deserialized PHY message
 *
 * @return number of deserialized bytes from stream on success, value < 0 on failure
 */
int
deserialize_msg(uint8_t *stream, PHY_msg *msg)
{
	uint8_t *data = stream;

	msg->type = *data++;
	msg->length = *data++;

	switch (msg->type) {
	case PD_DATA_REQUEST:
		msg->x.data_req.psduLength = *data++;
		memcpy(&msg->x.data_req.data, stream, msg->x.data_req.psduLength);
		break;
	case PD_DATA_CONFIRM:
		msg->x.data_conf.status = *data++;
		break;
	case PD_DATA_INDICATION:
		msg->x.data_ind.psduLength = *data++;
		msg->x.data_ind.ppduLinkQuality = *data++;
		memcpy(&msg->x.data_ind.data, stream, msg->x.data_ind.psduLength);
		break;
	case PLME_CCA_REQUEST:
		break;
	case PLME_CCA_CONFIRM:
		msg->x.cca_conf.status = *data++;
		break;
	case PLME_ED_REQUEST:
		break;
	case PLME_ED_CONFIRM:
		msg->x.ed_conf.status = *data++;
		msg->x.ed_conf.energyLevel = *data++;
		break;
	case PLME_GET_REQEST:
		msg->x.get_req.attribute = *data++;
		break;
	case PLME_GET_CONFIRM:
		msg->x.get_conf.status = *data++;
		msg->x.get_conf.attribute = *data++;
		switch (msg->x.get_conf.attribute) {
		case phyCurrentChannel:
			msg->x.get_conf.value.currentChannel = *data++;
			break;
		case phyChannelsSupported:
			msg->x.get_conf.value.channelsSupported |= (uint32_t) *data++;
			msg->x.get_conf.value.channelsSupported |= (uint32_t) *data++ << 8;
			msg->x.get_conf.value.channelsSupported |= (uint32_t) *data++ << 16;
			msg->x.get_conf.value.channelsSupported |= (uint32_t) *data++ << 24;
			break;
		case phyTransmitPower:
			msg->x.get_conf.value.transmitPower = *data++;
			break;
		case phyCCAMode:
			msg->x.get_conf.value.cCAMode = *data++;
			break;
		case phyCurrentPage:
			msg->x.get_conf.value.currentPage = *data++;
			break;
		case phyMaxFrameDuration:
			msg->x.get_conf.value.maxFrameDuration |= (uint16_t) *data++;
			msg->x.get_conf.value.maxFrameDuration |= (uint16_t) *data++ << 8;
			break;
		case phySHRDuration:
			msg->x.get_conf.value.sHRDuration = *data++;
			break;
		case phySymbolsPerOctet:
			msg->x.get_conf.value.symbolsPerOctet = *data++;
			break;
		default:
			return -1;
		}
		break;
	case PLME_SET_TRX_STATE_REQUEST:
		msg->x.set_trx_state_req.status = *data++;
		break;
	case PLME_SET_TRX_STATE_CONFIRM:
		msg->x.set_trx_state_conf.status = *data++;
		break;
	case PLME_SET_REQUEST:
		msg->x.set_req.attribute = *data++;
		switch (msg->x.set_req.attribute) {
		case phyCurrentChannel:
			msg->x.get_conf.value.currentChannel = *data++;
			break;
		case phyChannelsSupported:
			msg->x.set_req.value.channelsSupported |= (uint32_t) *data++;
			msg->x.set_req.value.channelsSupported |= (uint32_t) *data++ << 8;
			msg->x.set_req.value.channelsSupported |= (uint32_t) *data++ << 16;
			msg->x.set_req.value.channelsSupported |= (uint32_t) *data++ << 24;
			break;
		case phyTransmitPower:
			msg->x.set_req.value.transmitPower = *data++;
			break;
		case phyCCAMode:
			msg->x.set_req.value.cCAMode = *data++;
			break;
		case phyCurrentPage:
			msg->x.set_req.value.currentPage = *data++;
			break;
		case phyMaxFrameDuration:
			msg->x.set_req.value.maxFrameDuration |= (uint16_t) *data++;
			msg->x.set_req.value.maxFrameDuration |= (uint16_t) *data++ << 8;
			break;
		case phySHRDuration:
			msg->x.set_req.value.sHRDuration = *data++;
			break;
		case phySymbolsPerOctet:
			msg->x.set_req.value.symbolsPerOctet = *data++;
			break;
		default:
			return -1;
		}
		break;
	case PLME_SET_CONFIRM:
		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief Serializes a PHY message into a stream of bytes
 *
 * @param msg points to the PHY message to be serialized
 * @param stream buffer to encode the PHY message into
 *
 * @return number of written bytes in stream on success, value < 0 on failure
 */
int
serialize_msg(PHY_msg * msg, uint8_t buffer[aMaxPHYPacketSize])
{
	print_debug("serialize message\n");
	memcpy(&buffer, (void *)msg, msg->length);
	return 0;
}

/*---------------------------------------------------------------------------*/
/*
 * @brief Sends a PHY message to external upper layer
 */
void send_msg(PHY_msg * msg)
{
	print_debug("send message\n");

	uint32_t time;
	pcap_timeval_s timestamp;
	uint8_t size = aMaxPHYPacketSize+maxPHYMessageHeaderSize;

	uint8_t buffer[size];
	memset(&buffer, 0, sizeof(buffer));
	serialize_msg(msg, buffer);
	//memcpy(&buffer, header, hlen);
	//memcpy(&buffer+hlen, data, dlen);

	time = clock_time();
	timestamp.ts_sec = (time - (time % CLOCK_SECOND)) / CLOCK_SECOND;
	timestamp.ts_usec = 1000000 * (time % CLOCK_SECOND) / CLOCK_SECOND;

	pcapng_line_write_epb(1, &timestamp, &buffer, msg->length);
	//pcapng_line_write(&data, length);

	print_msg(msg, "to send");
}

/**
 * @brief Prints a PHY message payload
 */
void
print_msg_payload(uint8_t data[aMaxPHYPacketSize], uint8_t length, char *info)
{
	uint8_t i;
    print_debug("%s = ", info ? info : "");
    for (i = 0; (i < length) && (i < aMaxPHYPacketSize); i++) {
    	printf("%x, ", data[i]);
    }
    printf("\n");
}

/**
 * @brief Prints PHY message bytes
 */
void
print_msg_bytes(PHY_msg *msg, uint8_t length)
{
	uint8_t *data = (uint8_t *) msg;
	uint8_t i;
    print_debug("PHY message bytes = ");
    for (i = 0; (i < length) && (i < sizeof(PHY_msg)); i++) {
    	printf("%x, ", *data++);
    }
    printf("\n");
}

/**
 * @brief Prints a PHY PIB value
 */
void
print_pib_value(phy_pib_attr attr, PhyPIB_value value)
{
	switch (attr) {
	case phyCurrentChannel:
		print_debug("value = %u\n", value.currentChannel);
		break;
	case phyChannelsSupported:
		print_debug("value = %u\n", value.channelsSupported);
		break;
	case phyTransmitPower:
		print_debug("value = %u\n", value.transmitPower);
		break;
	case phyCCAMode:
		print_debug("value = %u\n", value.cCAMode);
		break;
	case phyCurrentPage:
		print_debug("value = %u\n", value.currentPage);
		break;
	case phyMaxFrameDuration:
		print_debug("value = %u\n", value.maxFrameDuration);
		break;
	case phySHRDuration:
		print_debug("value = %u\n", value.sHRDuration);
		break;
	case phySymbolsPerOctet:
		print_debug("value = %u\n", value.symbolsPerOctet);
		break;
	}
}

/**
 * @brief Prints the content of a PHY message
 */
void
print_msg(PHY_msg * msg, char *info)
{
	print_debug("PHY message: >> %s <<\n", info ? info : "");
	print_msg_bytes(msg, msg->length);
	print_debug("type = %s\n", msgTypeToString(msg->type));
	print_debug("length = %u\n", msg->length);

	switch(msg->type) {
	case PD_DATA_REQUEST:
		print_debug("psduLength = %u\n", msg->x.data_req.psduLength);
		print_msg_payload(msg->x.data_req.data, msg->x.data_req.psduLength, "data");
		break;
	case PD_DATA_CONFIRM:
		print_debug("status = %s\n", phyStateToString(msg->x.data_conf.status));
		break;
	case PD_DATA_INDICATION:
		print_debug("psduLength = %u\n", msg->x.data_ind.psduLength);
		print_debug("ppduLinkQuality = %u\n", msg->x.data_ind.ppduLinkQuality);
		print_msg_payload(msg->x.data_ind.data, msg->x.data_ind.psduLength, "data");
		break;
	case PLME_CCA_CONFIRM:
		print_debug("status = %s\n", phyStateToString(msg->x.cca_conf.status));
		break;
	case PLME_ED_CONFIRM:
		print_debug("status = %s\n", phyStateToString(msg->x.ed_conf.status));
		print_debug("energyLevel = %u\n", msg->x.ed_conf.energyLevel);
		break;
	case PLME_GET_REQEST:
		print_debug("attribute = %s\n", phyAttrToString(msg->x.get_req.attribute));
		break;
	case PLME_GET_CONFIRM:
		print_debug("status = %s\n", phyStateToString(msg->x.get_conf.status));
		print_debug("attribute = %s\n", phyAttrToString(msg->x.get_conf.attribute));
		break;
	case PLME_SET_TRX_STATE_REQUEST:
		print_debug("status = %s\n", phyStateToString(msg->x.set_trx_state_req.status));
		break;
	case PLME_SET_TRX_STATE_CONFIRM:
		print_debug("status = %s\n", phyStateToString(msg->x.set_trx_state_conf.status));
		break;
	case PLME_SET_REQUEST:
		print_debug("attribute = %s\n", phyAttrToString(msg->x.set_req.attribute));
		print_pib_value(msg->x.set_req.attribute, msg->x.set_req.value);
		break;
	case PLME_SET_CONFIRM:
		print_debug("status = %s\n", phyStateToString(msg->x.set_conf.status));
		print_debug("attribute = %s\n", phyAttrToString(msg->x.set_conf.attribute));
		break;
	default:
		break;
	}
}
