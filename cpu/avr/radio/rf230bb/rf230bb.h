/*  Copyright (c) 2008, Swedish Institute of Computer Science
 *  Copyright (c) 2017 Sebastian Boehm (BTU-CS)
 *  All rights reserved.
 *
 *  Additional fixes for AVR contributed by:
 *
 *	Colin O'Flynn coflynn@newae.com
 *	Eric Gnoske egnoske@gmail.com
 *	Blake Leverett bleverett@gmail.com
 *	Mike Vidales mavida404@gmail.com
 *	Kevin Brown kbrown3@uccs.edu
 *	Nate Bohlmann nate@elfwerks.com
 *  David Kopf dak664@embarqmail.com
 *  Ivan Delamer delamer@ieee.com
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of the copyright holders nor the names of
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */
/**
 *    \addtogroup radiorf230
 *   @{
 */
/**
 *  \file
 *  \brief This file contains radio driver code.
 *
 */

#ifndef RF230BB_H_
#define RF230BB_H_
/*============================ INCLUDE =======================================*/
#include <serial-phy.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#if defined(__AVR_ATmega128RFA1__)
#include "atmega128rfa1_registermap.h"
#else
#include "at86rf230_registermap.h"
#endif


/*============================ MACROS ========================================*/
#define SUPPORTED_PART_NUMBER                   ( 2 )
#define RF230_REVA                              ( 1 )
#define RF230_REVB                              ( 2 )
#define SUPPORTED_MANUFACTURER_ID               ( 31 )

#if defined(__AVR_ATmega128RFA1__)
#define RF230_SUPPORTED_INTERRUPT_MASK          ( 0xFF )
#else
/* RF230 does not support RX_START interrupts in extended mode, but it seems harmless to always enable it. */
/* In non-extended mode this allows RX_START to sample the RF rssi at the end of the preamble */
//#define RF230_SUPPORTED_INTERRUPT_MASK        ( 0x08 )  //enable trx end only
//#define RF230_SUPPORTED_INTERRUPT_MASK          ( 0x0F ) //disable bat low, trx underrun
#define RF230_SUPPORTED_INTERRUPT_MASK          ( 0x0C )  //disable bat low, trx underrun, pll lock/unlock
#endif

#define RF230_MIN_CHANNEL                       ( 11 )
#define RF230_MAX_CHANNEL                       ( 26 )
#define RF230_MIN_ED_THRESHOLD                  ( 0 )
#define RF230_MAX_ED_THRESHOLD                  ( 15 )
#define RF230_MAX_TX_FRAME_LENGTH               ( 127 ) /**< 127 Byte PSDU. */

#define TX_PWR_3DBM                             ( 0 )
#define TX_PWR_17_2DBM                          ( 15 )

#define TX_PWR_MAX                             TX_PWR_3DBM
#define TX_PWR_MIN                             TX_PWR_17_2DBM
#define TX_PWR_UNDEFINED                       (TX_PWR_MIN+1)


#define BATTERY_MONITOR_HIGHEST_VOLTAGE         ( 15 )
#define BATTERY_MONITOR_VOLTAGE_UNDER_THRESHOLD ( 0 )
#define BATTERY_MONITOR_HIGH_VOLTAGE            ( 1 )
#define BATTERY_MONITOR_LOW_VOLTAGE             ( 0 )

#define FTN_CALIBRATION_DONE                    ( 0 )
#define PLL_DCU_CALIBRATION_DONE                ( 0 )
#define PLL_CF_CALIBRATION_DONE                 ( 0 )

#define RC_OSC_REFERENCE_COUNT_MAX  (1.005*F_CPU*31250UL/8000000UL)
#define RC_OSC_REFERENCE_COUNT_MIN  (0.995*F_CPU*31250UL/8000000UL)

#ifndef RF_CHANNEL
#define RF_CHANNEL              26
#endif
/*============================ TYPEDEFS ======================================*/

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

/** \brief  This macro defines the start value for the RADIO_* status constants.
 *
 *          It was chosen to have this macro so that the user can define where
 *          the status returned from the TAT starts. This can be useful in a
 *          system where numerous drivers are used, and some range of status codes
 *          are occupied.
 *
 *  \see radio_status_t
 */
#define RADIO_STATUS_START_VALUE                  ( 0x40 )

/** \brief  This enumeration defines the possible return values for the TAT API
 *          functions.
 *
 *          These values are defined so that they should not collide with the
 *          return/status codes defined in the IEEE 802.15.4 standard.
 *
 */
typedef enum{
    RADIO_SUCCESS = RADIO_STATUS_START_VALUE,  /**< The requested service was performed successfully. */
    RADIO_UNSUPPORTED_DEVICE,         /**< The connected device is not an Atmel AT86RF230. */
    RADIO_INVALID_ARGUMENT,           /**< One or more of the supplied function arguments are invalid. */
    RADIO_TIMED_OUT,                  /**< The requested service timed out. */
    RADIO_WRONG_STATE,                /**< The end-user tried to do an invalid state transition. */
    RADIO_BUSY_STATE,                 /**< The radio transceiver is busy receiving or transmitting. */
    RADIO_STATE_TRANSITION_FAILED,    /**< The requested state transition could not be completed. */
    RADIO_CCA_IDLE,                   /**< Channel is clear, available to transmit a new frame. */
    RADIO_CCA_BUSY,                   /**< Channel busy. */
    RADIO_TRX_BUSY,                   /**< Transceiver is busy receiving or transmitting data. */
    RADIO_BAT_LOW,                    /**< Measured battery voltage is lower than voltage threshold. */
    RADIO_BAT_OK,                     /**< Measured battery voltage is above the voltage threshold. */
    RADIO_CRC_FAILED,                 /**< The CRC failed for the actual frame. */
    RADIO_CHANNEL_ACCESS_FAILURE,     /**< The channel access failed during the auto mode. */
    RADIO_NO_ACK,                     /**< No acknowledge frame was received. */
}radio_status_t;

/**
 * @brief	Conversion between internal avr implementation and official
 * 			specification phy enumeration
 *
 * According to Table 18—PHY enumerations description
 *
 * @param	state according to the avr implementation
 *
 * @return	phy_state according to the specification
 */
static inline const phy_state radioStatusToPhyState(radio_status_t state)
{
	switch (state)
	{
		case RADIO_SUCCESS:					return phy_SUCCESS;
		case RADIO_UNSUPPORTED_DEVICE:		return -1;
		case RADIO_INVALID_ARGUMENT:		return phy_INVALID_PARAMETER;
		case RADIO_TIMED_OUT:				return -1;
		case RADIO_WRONG_STATE:				return -1;
		case RADIO_BUSY_STATE:				return -1;
		case RADIO_STATE_TRANSITION_FAILED:	return -1;
		case RADIO_CCA_IDLE:				return phy_IDLE;
		case RADIO_CCA_BUSY:				return phy_BUSY;
		case RADIO_TRX_BUSY:				return phy_BUSY_RX;	/** todo: or phy_BUSY_TX */ /**<  */
		case RADIO_BAT_LOW:					return -1;
		case RADIO_BAT_OK:					return -1;
		case RADIO_CRC_FAILED:				return -1;
		case RADIO_CHANNEL_ACCESS_FAILURE:	return -1;
		case RADIO_NO_ACK:					return -1;
		default:                        	return -1;
	}
}

/**
 * @brief	Conversion between internal avr implementation and official
 * 			specification phy states.
 * 			Only the "states" according to the avr implementation are mentioned.
 *
 * Because the specification does not make any assumptions about the operation
 * modes or transceiver states of the radio module, we have to map here.
 *
 * According to rf230bb.c::radio_set_trx_state() following states are possible:
 * 			RX_ON        Requested transition to RX_ON state.
 * 			TRX_OFF      Requested transition to TRX_OFF state.
 * 			PLL_ON       Requested transition to PLL_ON state.
 * 			RX_AACK_ON   Requested transition to RX_AACK_ON state.
 * 			TX_ARET_ON   Requested transition to TX_ARET_ON state.
 *
 * According to PLME-SET-TRX-STATE.request following states are possible:
 * 			RX_ON
 * 			TRX_OFF
 * 			FORCE_TRX_OFF
 * 			TX_ON
 *
 * @param	phy_state according to the specification
 *
 * @return	state according to the avr implementation
 */
static inline const uint8_t phyStateToRadioState(phy_state state)
{
	switch (state)
	{
//		case phy_BUSY_RX:           	return BUSY_RX;
//		case phy_BUSY_TX:				return BUSY_TX;
		case phy_RX_ON:					return RX_ON;
		case phy_TRX_OFF:				return TRX_OFF;
		case phy_FORCE_TRX_OFF:			return TRX_OFF;
		case phy_TX_ON:					return PLL_ON;	/**< 9.4.1.2.3 PLL_ON – PLL State */
		/** todo: 9.4.2.3 RX_AACK_ON – Receive with Automatic ACK (from extended operation mode) */
		/** todo: 9.4.2.5 TX_ARET_ON – Transmit with Automatic Retry and CSMA-CA Retry (from extended operation mode) */
		default:                        return -1;
//		9.4.1.2.1 SLEEP – Sleep State
//		9.4.1.2.6 RESET State
	}
}

/**
 * \name Transaction status codes
 * \{
 */
#define TRAC_SUCCESS                0
#define TRAC_SUCCESS_DATA_PENDING   1
#define TRAC_SUCCESS_WAIT_FOR_ACK   2
#define TRAC_CHANNEL_ACCESS_FAILURE 3
#define TRAC_NO_ACK                 5
#define TRAC_INVALID                7
/** \} */


/** \brief  This enumeration defines the possible modes available for the
 *          Clear Channel Assessment algorithm.
 *
 *          These constants are extracted from the datasheet.
 *
 */
typedef enum{
    CCA_ENERGY_DETECT         = 0,    /**< Use energy detection above threshold mode. */
    CCA_CARRIER_SENSE         = 1,    /**< Use carrier sense mode. */
    CCA_CARRIER_SENSE_WITH_ED = 2     /**< Use a combination of both energy detection and carrier sense. */
}radio_cca_mode_t;


/** \brief  This enumeration defines the possible CLKM speeds.
 *
 *          These constants are extracted from the RF230 datasheet.
 *
 */
typedef enum{
    CLKM_DISABLED      = 0,
    CLKM_1MHZ          = 1,
    CLKM_2MHZ          = 2,
    CLKM_4MHZ          = 3,
    CLKM_8MHZ          = 4,
    CLKM_16MHZ         = 5
}radio_clkm_speed_t;

typedef void (*radio_rx_callback) (uint16_t data);


/*	Hook Documentation 
**	
**	Sniffing Hooks:
**		RF230BB_HOOK_TX_PACKET(buffer,total_len)
**		RF230BB_HOOK_RX_PACKET(buf,len)
**
**	RF230BB_HOOK_IS_SEND_ENABLED()
**	RF230BB_HOOK_RADIO_ON()
**	RF230BB_HOOK_RADIO_OFF()
**	
*/

/*========================= INLINE FUNCTIONS =================================*/

/**
 * @brief Get the RSSI threshold
 *
 * @return threshold value
 */
static inline const int8_t radio_get_rssi_threshold() { return threshold; }


/*============================ PROTOTYPES ====================================*/

const struct radio_driver rf230_driver;

int rf230_init(void);
void rf230_warm_reset(void);
void rf230_start_sneeze(void);
void rf230_set_channel(uint8_t channel);
void rf230_listen_channel(uint8_t channel);
uint8_t rf230_get_channel(void);
void rf230_set_pan_addr(unsigned pan,unsigned addr,const uint8_t ieee_addr[8]);
void rf230_set_txpower(uint8_t power);
uint8_t rf230_get_txpower(void);

void rf230_set_promiscuous_mode(bool isPromiscuous);
bool rf230_is_ready_to_send();

extern uint8_t rf230_last_correlation,rf230_last_rssi,rf230_smallest_rssi;

uint8_t rf230_get_raw_rssi(void);

#define rf230_rssi	rf230_get_raw_rssi

#endif /* RF230BB_H_ */
/** @} */
/*EOF*/
