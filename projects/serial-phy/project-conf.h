#ifndef __PROJECT_CONF_H__
#define __PROJECT_CONF_H__

// set the RDC layer to sender_fake_rdc_driver
#ifdef NETSTACK_CONF_RDC
#undef NETSTACK_CONF_RDC
#endif /* NETSTACK_CONF_RDC */
#define NETSTACK_CONF_RDC           transceiver_rdc_driver

#endif /* __PROJECT_CONF_H__ */

