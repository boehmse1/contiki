/*
 * Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *
 * IEEE 802.15.4 radio driver
 *
 */

#ifndef __ATMEGA128RFA1_H__
#define __ATMEGA128RFA1_H__

void radio_init();

uint8_t radio_get_rssi_threshold();

uint8_t radio_get_operating_channel();
enum phyState radio_set_operating_channel(uint8_t channel);

uint8_t radio_get_tx_power_level();
enum phyState radio_set_tx_power_level(uint8_t power);

uint8_t radio_get_cca_mode();
enum phyState radio_set_cca_mode(uint8_t mode, uint8_t thres);

uint8_t radio_get_current_page();
enum phyState radio_set_current_page(uint8_t page);

uint16_t radio_get_max_frame_duration();
uint8_t radio_get_shr_duration();
float radio_get_symbols_per_octet();

#endif /* __ATMEGA128RFA1_H__ */
