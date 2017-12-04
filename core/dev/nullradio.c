#include "dev/nullradio.h"

static uint8_t channel = 26;
static int txpower = 3;

/*---------------------------------------------------------------------------*/
static int
init(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
prepare(const void *payload, unsigned short payload_len)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
transmit(unsigned short transmit_len)
{
  return RADIO_TX_OK;
}
/*---------------------------------------------------------------------------*/
static int
send(const void *payload, unsigned short payload_len)
{
  prepare(payload, payload_len);
  return transmit(payload_len);
}
/*---------------------------------------------------------------------------*/
static int
radio_read(void *buf, unsigned short buf_len)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
channel_clear(void)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
receiving_packet(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
pending_packet(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
on(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
off(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static radio_result_t
get_value(radio_param_t param, radio_value_t *value)
{
	switch (param) {
		  case RADIO_PARAM_CHANNEL:
			  *value = (int)channel;
			  return RADIO_RESULT_OK;
		  case RADIO_PARAM_CCA_MODE:
			  *value = ccaMode;
			  return RADIO_RESULT_OK;
		  case RADIO_PARAM_CURRENT_PAGE:
			  *value = currentPage;
			  return RADIO_RESULT_OK;
		  case RADIO_PARAM_SHR_DURATION:
			  *value = sHRDuration;
			  return RADIO_RESULT_OK;
		  case RADIO_PARAM_SYMBOLS_PER_OCTET:
			  *value = symbolsPerOctet;
			  return RADIO_RESULT_OK;
		  case RADIO_PARAM_TXPOWER:
			  *value = txpower;
			  return RADIO_RESULT_OK;
		  default:
			  return RADIO_RESULT_NOT_SUPPORTED;
	  }
}
/*---------------------------------------------------------------------------*/
static radio_result_t
set_value(radio_param_t param, radio_value_t value)
{
	switch (param) {
		  case RADIO_PARAM_CHANNEL:
			  channel = (uint8_t) value;
			  return RADIO_RESULT_OK;
		  case RADIO_PARAM_CCA_MODE:
			  /* todo: */
			  return RADIO_RESULT_READ_ONLY;
		  case RADIO_PARAM_TXPOWER:
			  txpower = value;
			  return RADIO_RESULT_OK;
		  case RADIO_PARAM_CURRENT_PAGE:
		  case RADIO_PARAM_SHR_DURATION:
		  case RADIO_PARAM_SYMBOLS_PER_OCTET:
			  return RADIO_RESULT_READ_ONLY;
		  default:
			  return RADIO_RESULT_NOT_SUPPORTED;
	  }
}
/*---------------------------------------------------------------------------*/
static radio_result_t
get_object(radio_param_t param, void *dest, size_t size)
{
	switch (param) {
			case RADIO_PARAM_CHANNELS_SUPPORTED:
				*(uint32_t *)dest = channelsSupported;
				return RADIO_RESULT_OK;
			case RADIO_PARAM_MAX_FRAME_DURATION:
				*(uint16_t *)dest = maxFrameDuration;
				return RADIO_RESULT_OK;
			default:
				return RADIO_RESULT_NOT_SUPPORTED;
		}
}
/*---------------------------------------------------------------------------*/
static radio_result_t
set_object(radio_param_t param, const void *src, size_t size)
{
	switch (param) {
			case RADIO_PARAM_CHANNELS_SUPPORTED:
				return RADIO_RESULT_READ_ONLY;
			case RADIO_PARAM_MAX_FRAME_DURATION:
				return RADIO_RESULT_READ_ONLY;
			default:
				return RADIO_RESULT_NOT_SUPPORTED;
		}
}
/*---------------------------------------------------------------------------*/
const struct radio_driver nullradio_driver =
  {
    init,
    prepare,
    transmit,
    send,
    radio_read,
    channel_clear,
    receiving_packet,
    pending_packet,
    on,
    off,
    get_value,
    set_value,
    get_object,
    set_object
  };
/*---------------------------------------------------------------------------*/
