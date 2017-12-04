#ifndef NULLRADIO_H
#define NULLRADIO_H

#include "dev/radio.h"
#include "phy.h"

/** @brief	Threshold for energy detection
 *
 * Note:	ATmega128RFA1 datasheet - Figure 9-18
 * 			energy level resolution 0-83
 * 			ED sensitivity -90dBm (steps are 1dBm PHY_ED_LEVEL)
 */
static const int8_t threshold = -90;

/** @brief 	Tolerance of the output power level
 *
 * See ATmega128RFA1 datasheet - 35.14.3 Transmitter Characteristics
 *
 * Note:	Shifted to MSBs b7 .. b6 of a 8-bit value
 */
static const uint8_t txPowerTolerance = 64;

/** @brief 	Tolerance of the output power level
 *
 * See ATmega128RFA1 datasheet - 35.14.3 Transmitter Characteristics
 *
 * Note:	Shifted to MSBs b7 .. b6 of a 8-bit value
 */
static const phyAttrChannelsSupported channelsSupported = 0xAABBCCDD;

/** @brief	Channel page (according to 2,4GHz O-QPSK PHY) */
static const phyAttrCurrentPage currentPage = 0;

/** @brief	Maximum frame duration (according to 2,4GHz O-QPSK PHY) */
static const phyAttrMaxFrameDuration maxFrameDuration = 266;

/** @brief	Duration of the SHR (according to 2,4GHz O-QPSK PHY) */
static const phyAttrSHRDuration sHRDuration = 10;

/** @brief	Number of symbols per octet (according to 2,4GHz O-QPSK PHY) */
static const phyAttrSymbolsPerOctet symbolsPerOctet = 2;

/** @brief	CCA mode todo: static? */
static const phyAttrCCAMode ccaMode = 3;
extern const struct radio_driver nullradio_driver;

#endif /* NULLRADIO_H */
