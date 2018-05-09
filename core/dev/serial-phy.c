/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * @brief IEEE 802.15.4 PHY service routines for marshalling and debug printing
 *
 */

#include "serial-phy.h"

#define DEBUG 0
#if DEBUG && DEBUG == 1
#define print_debug(fmt, args...) printf("[802154_PHY]: " fmt, ##args)
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
		memcpy(&msg->x.data_req.data, data, msg->x.data_req.psduLength);
		data += msg->x.data_req.psduLength;
		break;
	case PD_DATA_CONFIRM:
		msg->x.data_conf.status = *data++;
		break;
	case PD_DATA_INDICATION:
		msg->x.data_ind.psduLength = *data++;
		msg->x.data_ind.ppduLinkQuality = *data++;
		memcpy(&msg->x.data_ind.data, data, msg->x.data_ind.psduLength);
		data += msg->x.data_ind.psduLength;
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
	case PLME_GET_REQUEST:
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
			msg->x.set_req.value.currentChannel = *data++;
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

	return (int) (data - stream);
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
	uint8_t pos = 0;

	buffer[pos++] = msg->type;
	buffer[pos++] = msg->length;

	switch (msg->type) {
	case PD_DATA_REQUEST:
		buffer[pos++] = msg->x.data_req.psduLength;
		memcpy(&buffer[pos], (void *)msg->x.data_req.data, msg->x.data_req.psduLength);
		pos += msg->x.data_req.psduLength;
		break;
	case PD_DATA_CONFIRM:
		buffer[pos++] = msg->x.data_conf.status;
		break;
	case PD_DATA_INDICATION:
		buffer[pos++] = msg->x.data_ind.psduLength;
		buffer[pos++] = msg->x.data_ind.ppduLinkQuality;
		memcpy(&buffer[pos], (void *)msg->x.data_ind.data, msg->x.data_ind.psduLength);
		pos += msg->x.data_ind.psduLength;
		break;
	case PLME_CCA_REQUEST:
		break;
	case PLME_CCA_CONFIRM:
		buffer[pos++] = msg->x.cca_conf.status;
		break;
	case PLME_ED_REQUEST:
		break;
	case PLME_ED_CONFIRM:
		buffer[pos++] = msg->x.ed_conf.status;
		buffer[pos++] = msg->x.ed_conf.energyLevel;
		break;
	case PLME_GET_REQUEST:
		buffer[pos++] = msg->x.get_req.attribute;
		break;
	case PLME_GET_CONFIRM:
		buffer[pos++] = msg->x.get_conf.status;
		buffer[pos++] = msg->x.get_conf.attribute;
		switch (msg->x.get_conf.attribute) {
		case phyCurrentChannel:
			buffer[pos++] = msg->x.get_conf.value.currentChannel;
			break;
		case phyChannelsSupported:
			buffer[pos++] = (uint8_t) msg->x.get_conf.value.channelsSupported;
			buffer[pos++] = (uint8_t) (msg->x.get_conf.value.channelsSupported >> 8);
			buffer[pos++] = (uint8_t) (msg->x.get_conf.value.channelsSupported >> 16);
			buffer[pos++] = (uint8_t) (msg->x.get_conf.value.channelsSupported >> 24);
			break;
		case phyTransmitPower:
			buffer[pos++] = msg->x.get_conf.value.transmitPower;
			break;
		case phyCCAMode:
			buffer[pos++] = msg->x.get_conf.value.cCAMode;
			break;
		case phyCurrentPage:
			buffer[pos++] = msg->x.get_conf.value.currentPage;
			break;
		case phyMaxFrameDuration:
			buffer[pos++] = (uint8_t) msg->x.get_conf.value.maxFrameDuration;
			buffer[pos++] = (uint8_t) (msg->x.get_conf.value.maxFrameDuration >> 8);
			break;
		case phySHRDuration:
			buffer[pos++] = msg->x.get_conf.value.sHRDuration;
			break;
		case phySymbolsPerOctet:
			buffer[pos++] = msg->x.get_conf.value.symbolsPerOctet;
			break;
		default:
			return -1;
		}
		break;
	case PLME_SET_TRX_STATE_REQUEST:
		buffer[pos++] = msg->x.set_trx_state_req.status;
		break;
	case PLME_SET_TRX_STATE_CONFIRM:
		buffer[pos++] = msg->x.set_trx_state_conf.status;
		break;
	case PLME_SET_REQUEST:
		buffer[pos++] = msg->x.set_req.attribute;
		switch (msg->x.set_req.attribute) {
		case phyCurrentChannel:
			buffer[pos++] = msg->x.set_req.value.currentChannel;
			break;
		case phyChannelsSupported:
			buffer[pos++] = (uint8_t) msg->x.set_req.value.channelsSupported;
			buffer[pos++] = (uint8_t) (msg->x.set_req.value.channelsSupported >> 8);
			buffer[pos++] = (uint8_t) (msg->x.set_req.value.channelsSupported >> 16);
			buffer[pos++] = (uint8_t) (msg->x.set_req.value.channelsSupported >> 24);
			break;
		case phyTransmitPower:
			buffer[pos++] = msg->x.set_req.value.transmitPower;
			break;
		case phyCCAMode:
			buffer[pos++] = msg->x.set_req.value.cCAMode;
			break;
		case phyCurrentPage:
			buffer[pos++] = msg->x.set_req.value.currentPage;
			break;
		case phyMaxFrameDuration:
			buffer[pos++] = (uint8_t) msg->x.set_req.value.maxFrameDuration;
			buffer[pos++] = (uint8_t) (msg->x.set_req.value.maxFrameDuration >> 8);
			break;
		case phySHRDuration:
			buffer[pos++] = msg->x.set_req.value.sHRDuration;
			break;
		case phySymbolsPerOctet:
			buffer[pos++] = msg->x.set_req.value.symbolsPerOctet;
			break;
		default:
			return -1;
		}
		break;
	case PLME_SET_CONFIRM:
		buffer[pos++] = msg->x.set_conf.status;
		buffer[pos++] = msg->x.set_conf.attribute;
		break;
	default:
		return -1;
	}

	return pos;

}

/*---------------------------------------------------------------------------*/
/*
 * @brief Sends a PHY message to external upper layer
 */
void send_msg(PHY_msg * msg)
{
	uint32_t time;
	pcap_timeval_s timestamp;
	uint8_t size = aMaxPHYPacketSize+maxPHYMessageHeaderSize;

	uint8_t buffer[size];
	memset(&buffer, 0, sizeof(buffer));
	serialize_msg(msg, buffer);

	time = clock_time();
	timestamp.ts_sec = (time - (time % CLOCK_SECOND)) / CLOCK_SECOND;
	timestamp.ts_usec = 1000000 * (time % CLOCK_SECOND) / CLOCK_SECOND;

	pcapng_line_write_epb(0, &timestamp, &buffer, msg->length);

	print_msg(msg, "to send");
}

/**
 * @brief Prints a PHY message payload
 */
void
print_msg_payload(void *data, uint8_t length, char *info)
{
#if DEBUG
	uint8_t i;
    print_debug("%s = ", info ? info : "");
    for (i = 0; (i < length) && (i < aMaxPHYPacketSize); i++) {
    	printf("%x, ", *(uint8_t *)(data+i));
    }
    printf("\n");
#endif
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
		print_debug("value = %" PRIu32 "\n", value.channelsSupported);
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
		print_debug("value = %" PRIu16 "\n", value.maxFrameDuration);
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
	print_debug("type = %s\n", msgTypeToString(msg->type));
	print_debug("length = %u\n", msg->length);

	switch(msg->type) {
	case PD_DATA_REQUEST:
		print_debug("psduLength = %u\n", msg->x.data_req.psduLength);
		print_msg_payload(&msg->x.data_req.data, msg->x.data_req.psduLength, "data");
		break;
	case PD_DATA_CONFIRM:
		print_debug("status = %s\n", phyStateToString(msg->x.data_conf.status));
		break;
	case PD_DATA_INDICATION:
		print_debug("psduLength = %u\n", msg->x.data_ind.psduLength);
		print_debug("ppduLinkQuality = %i\n", msg->x.data_ind.ppduLinkQuality);
		print_msg_payload(&msg->x.data_ind.data, msg->x.data_ind.psduLength, "data");
		break;
	case PLME_CCA_CONFIRM:
		print_debug("status = %s\n", phyStateToString(msg->x.cca_conf.status));
		break;
	case PLME_ED_CONFIRM:
		print_debug("status = %s\n", phyStateToString(msg->x.ed_conf.status));
		print_debug("energyLevel = %u\n", msg->x.ed_conf.energyLevel);
		break;
	case PLME_GET_REQUEST:
		print_debug("attribute = %s\n", phyAttrToString(msg->x.get_req.attribute));
		break;
	case PLME_GET_CONFIRM:
		print_debug("status = %s\n", phyStateToString(msg->x.get_conf.status));
		print_debug("attribute = %s\n", phyAttrToString(msg->x.get_conf.attribute));
		print_pib_value(msg->x.get_conf.attribute, msg->x.get_conf.value);
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
	print_debug("\n");
}

/** @brief test function */
void
send_test(void)
{
	PHY_msg msg;
	uint8_t type;
	uint8_t attr;

	for (type=PD_DATA_REQUEST; type<=PLME_SET_CONFIRM; type++) {
		msg.type = type;
		switch (type) {
		case PD_DATA_REQUEST:
			msg.x.data_req.psduLength = 2;
			msg.x.data_req.data[0] = 0xAA;
			msg.x.data_req.data[1] = 0xBB;
			msg.length = msg.x.data_req.psduLength + SIZEOF_PD_DATA_REQUEST;
			send_msg(&msg);
			break;
		case PD_DATA_INDICATION:
			msg.x.data_ind.psduLength = 2;
			msg.x.data_ind.ppduLinkQuality = 0xFF;
			msg.x.data_ind.data[0] = 0xCC;
			msg.x.data_ind.data[1] = 0xDD;
			msg.length = msg.x.data_ind.psduLength + SIZEOF_PD_DATA_INDICATION;
			send_msg(&msg);
			break;
		case PLME_CCA_REQUEST:
			msg.length = SIZEOF_PLME_CCA_REQUEST;
			send_msg(&msg);
			break;
		case PLME_ED_REQUEST:
			msg.length = SIZEOF_PLME_ED_REQUEST;
			send_msg(&msg);
			break;
		case PLME_GET_REQUEST:
			msg.length = SIZEOF_PLME_GET_REQEST;
			for (attr=phyCurrentChannel; attr<=phySymbolsPerOctet; attr++) {
				msg.x.set_req.attribute = attr;
				send_msg(&msg);
			}
			break;
		case PLME_SET_TRX_STATE_REQUEST:
			msg.x.set_trx_state_req.status = phy_RX_ON;
			msg.length = SIZEOF_PLME_SET_TRX_STATE_REQUEST;
			send_msg(&msg);
			break;
		case PLME_SET_REQUEST:
			for (attr=phyCurrentChannel; attr<=phySymbolsPerOctet; attr++) {
				msg.x.set_req.attribute = attr;
				switch (attr) {
				case phyCurrentChannel:
					msg.x.get_conf.value.currentChannel = 0x11;
					msg.length = SIZEOF_PLME_SET_REQUEST + sizeof(phyAttrCurrentChannel);
					send_msg(&msg);
					break;
				case phyChannelsSupported:
					msg.x.set_req.value.channelsSupported = 0x12345678;
					msg.length = SIZEOF_PLME_SET_REQUEST + sizeof(phyAttrChannelsSupported);
					send_msg(&msg);
					break;
				case phyTransmitPower:
					msg.x.set_req.value.transmitPower = 3;
					msg.length = SIZEOF_PLME_SET_REQUEST + sizeof(phyAttrTransmitPower);
					send_msg(&msg);
					break;
				case phyCCAMode:
					msg.x.set_req.value.cCAMode = 1;
					msg.length = SIZEOF_PLME_SET_REQUEST + sizeof(phyAttrCCAMode);
					send_msg(&msg);
					break;
				case phyCurrentPage:
					msg.x.set_req.value.currentPage = 2;
					msg.length = SIZEOF_PLME_SET_REQUEST + sizeof(phyAttrCurrentPage);
					send_msg(&msg);
					break;
				case phyMaxFrameDuration:
					msg.x.set_req.value.maxFrameDuration = 0x1A;
					msg.length = SIZEOF_PLME_SET_REQUEST + sizeof(phyAttrMaxFrameDuration);
					send_msg(&msg);
					break;
				case phySHRDuration:
					msg.x.set_req.value.sHRDuration = 0x1B;
					msg.length = SIZEOF_PLME_SET_REQUEST + sizeof(phyAttrSHRDuration);
					send_msg(&msg);
					break;
				case phySymbolsPerOctet:
					msg.x.set_req.value.symbolsPerOctet = 0x1C;
					msg.length = SIZEOF_PLME_SET_REQUEST + sizeof(phyAttrSymbolsPerOctet);
					send_msg(&msg);
					break;
				}
			}
			break;
		}

	}
}
