/*
 * Copyright (c) 2014-2016 IBM Corporation.
 * Copyright (c) 2016 Matthijs Kooijman.
 * Copyright (c) 2016-2021 MCCI Corporation.
 * All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the <organization> nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//! @file
//! @brief LMIC API

#ifndef _lmic_h_
#define _lmic_h_

#include "oslmic.h"
#include "lorabase.h"

#if LMIC_DEBUG_LEVEL > 0 || LMIC_X_DEBUG_LEVEL > 0
# if defined(LMIC_DEBUG_INCLUDE)
#   define LMIC_STRINGIFY_(x) #x
#   define LMIC_STRINGIFY(x) LMIC_STRINGIFY_(x)
#   include LMIC_STRINGIFY(LMIC_DEBUG_INCLUDE)
# endif
#  ifdef LMIC_DEBUG_PRINTF_FN
     extern void LMIC_DEBUG_PRINTF_FN(const char *f, ...);
#  endif // ndef LMIC_DEBUG_PRINTF_FN
#endif

// if LMIC_DEBUG_PRINTF is now defined, just use it. This lets you do anything
// you like with a sufficiently crazy header file.
#if LMIC_DEBUG_LEVEL > 0
# ifndef LMIC_DEBUG_PRINTF
//  otherwise, check whether someone configured a print-function to be used,
//  and use it if so.
#   ifdef LMIC_DEBUG_PRINTF_FN
#     define LMIC_DEBUG_PRINTF(f, ...) LMIC_DEBUG_PRINTF_FN(f, ## __VA_ARGS__)
#     ifndef LMIC_DEBUG_INCLUDE // If you use LMIC_DEBUG_INCLUDE, put the declaration in there
        void LMIC_DEBUG_PRINTF_FN(const char *f, ...);
#     endif // ndef LMIC_DEBUG_INCLUDE
#   else // ndef LMIC_DEBUG_PRINTF_FN
//    if there's no other info, just use printf. In a pure Arduino environment,
//    that's what will happen.
#     include <stdio.h>
#     define LMIC_DEBUG_PRINTF(f, ...) printf(f, ## __VA_ARGS__)
#   endif // ndef LMIC_DEBUG_PRINTF_FN
# endif // ndef LMIC_DEBUG_PRINTF
# ifndef LMIC_DEBUG_FLUSH
#   ifdef LMIC_DEBUG_FLUSH_FN
#     define LMIC_DEBUG_FLUSH() LMIC_DEBUG_FLUSH_FN()
#   else // ndef LMIC_DEBUG_FLUSH_FN
//    if there's no other info, assume that flush is not needed.
#     define LMIC_DEBUG_FLUSH() do { ; } while (0)
#   endif // ndef LMIC_DEBUG_FLUSH_FN
# endif // ndef LMIC_DEBUG_FLUSH
#else // LMIC_DEBUG_LEVEL == 0
// If debug level is zero, printf and flush expand to nothing.
# define LMIC_DEBUG_PRINTF(f, ...)      do { ; } while (0)
# define LMIC_DEBUG_FLUSH()             do { ; } while (0)
#endif // LMIC_DEBUG_LEVEL == 0

//
// LMIC_X_DEBUG_LEVEL enables additional, special print functions for debugging
// RSSI features. This is used sparingly.
#if LMIC_X_DEBUG_LEVEL > 0
#  ifdef LMIC_DEBUG_PRINTF_FN
#    define LMIC_X_DEBUG_PRINTF(f, ...) LMIC_DEBUG_PRINTF_FN(f, ## __VA_ARGS__)
#  else
#    error "LMIC_DEBUG_PRINTF_FN must be defined for LMIC_X_DEBUG_LEVEL > 0."
#  endif
#else
#  define LMIC_X_DEBUG_PRINTF(f, ...)  do {;} while(0)
#endif

#ifdef __cplusplus
extern "C"{
#endif

// LMIC version -- this is the IBM LMIC version
#define LMIC_VERSION_MAJOR 1
#define LMIC_VERSION_MINOR 6
#define LMIC_VERSION_BUILD 1468577746

// Arduino LMIC version
#define ARDUINO_LMIC_VERSION_CALC(major, minor, patch, local)	\
	((((major)*UINT32_C(1)) << 24) | (((minor)*UINT32_C(1)) << 16) | (((patch)*UINT32_C(1)) << 8) | (((local)*UINT32_C(1)) << 0))

#define	ARDUINO_LMIC_VERSION    \
    ARDUINO_LMIC_VERSION_CALC(4, 2, 0, 1)  /* 4.2.0-1 */

#define	ARDUINO_LMIC_VERSION_GET_MAJOR(v)	\
	((((v)*UINT32_C(1)) >> 24u) & 0xFFu)

#define	ARDUINO_LMIC_VERSION_GET_MINOR(v)	\
	((((v)*UINT32_C(1)) >> 16u) & 0xFFu)

#define	ARDUINO_LMIC_VERSION_GET_PATCH(v)	\
	((((v)*UINT32_C(1)) >> 8u) & 0xFFu)

#define	ARDUINO_LMIC_VERSION_GET_LOCAL(v)	\
	((v) & 0xFFu)

/// \brief convert a semantic version to an ordinal integer.
#define ARDUINO_LMIC_VERSION_TO_ORDINAL(v)  \
        (((v) & 0xFFFFFF00u) | (((v) - 1) & 0xFFu))

/// \brief compare two semantic versions
/// \return \c true if \p a is less than \p b (as a semantic version).
#define ARDUINO_LMIC_VERSION_COMPARE_LT(a, b)   \
        (ARDUINO_LMIC_VERSION_TO_ORDINAL(a) < ARDUINO_LMIC_VERSION_TO_ORDINAL(b))

/// \brief compare two semantic versions
/// \return \c true if \p a is less than or equal to \p b (as a semantic version).
#define ARDUINO_LMIC_VERSION_COMPARE_LE(a, b)   \
        (ARDUINO_LMIC_VERSION_TO_ORDINAL(a) <= ARDUINO_LMIC_VERSION_TO_ORDINAL(b))

/// \brief compare two semantic versions
/// \return \c true if \p a is greater than \p b (as a semantic version).
#define ARDUINO_LMIC_VERSION_COMPARE_GT(a, b)   \
        (ARDUINO_LMIC_VERSION_TO_ORDINAL(a) > ARDUINO_LMIC_VERSION_TO_ORDINAL(b))

/// \brief compare two semantic versions
/// \return \c true if \p a is greater than or equal to \p b (as a semantic version).
#define ARDUINO_LMIC_VERSION_COMPARE_GE(a, b)   \
        (ARDUINO_LMIC_VERSION_TO_ORDINAL(a) >= ARDUINO_LMIC_VERSION_TO_ORDINAL(b))


//! Only For Antenna Tuning Tests !
//#define CFG_TxContinuousMode 1

// since this was announced as the API variable, we keep it. But it's not used,
// MAX_LEN_FRAME is what the code uses.
enum { MAX_FRAME_LEN      =  MAX_LEN_FRAME };   //!< Library cap on max frame length

enum { TXCONF_ATTEMPTS    =   8 };   //!< Transmit attempts for confirmed frames
enum { MAX_MISSED_BCNS    =  (2 * 60 * 60 + 127) / 128 };   //!< threshold for dropping out of class B, triggering rejoin requests
                                     // note that we need 100 ppm timing accuracy for
                                     // this, to keep the timing error to +/- 700ms.
enum { MAX_RXSYMS         = 350 };   // Stop tracking beacon if sync error grows beyond this. A 0.4% clock error
                                     // at SF9.125k means 512 ms; one symbol is 4.096 ms,
                                     // so this needs to be at least 125 for an STM32L0.
                                     // And for 100ppm clocks and 2 hours of beacon misses,
                                     // this needs to accommodate 1.4 seconds of error at
                                     // 4.096 ms/sym or at least 342 symbols.

enum { LINK_CHECK_CONT    =  0  ,    // continue with this after reported dead link
       LINK_CHECK_DEAD    =  32 ,    // after this UP frames and no response to ack from NWK assume link is dead (ADR_ACK_DELAY)
       LINK_CHECK_UNJOIN_MIN = LINK_CHECK_DEAD + 4,         // this is the minimum value of LINK_CHECK_UNJOIN if we parameterize
       LINK_CHECK_UNJOIN  =  LINK_CHECK_DEAD + (3 * 240),   // after this many UP frames and no response, switch to join (by default)
       LINK_CHECK_INIT    = -64 ,    // UP frame count until we ask for ack (ADR_ACK_LIMIT)
       LINK_CHECK_OFF     =-128 };   // link check disabled

enum { TIME_RESYNC        = 6*128 }; // secs
enum { TXRX_GUARD_ms      =  6000 };  // msecs - don't start TX-RX transaction before beacon
enum { JOIN_GUARD_ms      =  9000 };  // msecs - don't start Join Req/Acc transaction before beacon
enum { TXRX_BCNEXT_secs   =     2 };  // secs - earliest start after beacon time
enum { RETRY_PERIOD_secs  =     3 };  // secs - random period for retrying a confirmed send

#if CFG_LMIC_EU_like // EU868 spectrum ====================================================

enum { MAX_CHANNELS = 16 };      //!< Max supported channels
enum { MAX_BANDS    =  4 };

enum { LIMIT_CHANNELS = (1<<4) };   // EU868 will never have more channels
//! \internal
struct band_t {
    u2_t     txcap;     // duty cycle limitation: 1/txcap
    s1_t     txpow;     // maximum TX power
    u1_t     lastchnl;  // last used channel
    ostime_t avail;     // band is blocked until this time
};
TYPEDEF_xref2band_t; //!< \internal

struct lmic_saved_adr_state_s {
    u4_t        channelFreq[MAX_CHANNELS];
    u2_t        channelMap;
};

#elif CFG_LMIC_US_like  // US915 spectrum =================================================

struct lmic_saved_adr_state_s {
    u2_t        channelMap[(72+15)/16];  // enabled bits
    u2_t        activeChannels125khz;
    u2_t        activeChannels500khz;
};

#endif // ==========================================================================

typedef struct lmic_saved_adr_state_s   lmic_saved_adr_state_t;

// Keep in sync with evdefs.hpp::drChange
enum { DRCHG_SET, DRCHG_NOJACC, DRCHG_NOACK, DRCHG_NOADRACK, DRCHG_NWKCMD, DRCHG_FRAMESIZE };
enum { KEEP_TXPOW = -128 };


#if !defined(DISABLE_PING)
//! \internal
struct rxsched_t {
    dr_t     dr;
    u1_t     intvExp;   // 0..7
    u1_t     slot;      // runs from 0 to 128
    rxsyms_t rxsyms;
    ostime_t rxbase;
    ostime_t rxtime;    // start of next spot
    u4_t     freq;
};
TYPEDEF_xref2rxsched_t;  //!< \internal
#endif // !DISABLE_PING


#if !defined(DISABLE_BEACONS)
//! Parsing and tracking states of beacons.
enum { BCN_NONE    = 0x00,   //!< No beacon received
       BCN_PARTIAL = 0x01,   //!< Only first (common) part could be decoded (info,lat,lon invalid/previous)
       BCN_FULL    = 0x02,   //!< Full beacon decoded
       BCN_NODRIFT = 0x04,   //!< No drift value measured yet
       BCN_NODDIFF = 0x08 }; //!< No differential drift measured yet
//! Information about the last and previous beacons.
struct bcninfo_t {
    ostime_t txtime;  //!< Time when the beacon was sent
    u4_t     time;    //!< GPS time in seconds of last beacon (received or surrogate)
    s4_t     lat;     //!< Lat field of last beacon (valid only if BCN_FULL set)
    s4_t     lon;     //!< Lon field of last beacon (valid only if BCN_FULL set)
    s1_t     rssi;    //!< Adjusted RSSI value of last received beacon
    s1_t     snr;     //!< Scaled SNR value of last received beacon
    u1_t     flags;   //!< Last beacon reception and tracking states. See BCN_* values.
    //
    u1_t     info;    //!< Info field of last beacon (valid only if BCN_FULL set)
};
#endif // !DISABLE_BEACONS

// purpose of receive window - lmic_t.rxState
enum { RADIO_RST=0, RADIO_TX=1, RADIO_RX=2, RADIO_RXON=3, RADIO_TX_AT=4, };
// Netid values /  lmic_t.netid
enum { NETID_NONE=(int)~0U, NETID_MASK=(int)0xFFFFFF };
// MAC operation modes (lmic_t.opmode).
enum { OP_NONE     = 0x0000,
       OP_SCAN     = 0x0001, // radio scan to find a beacon
       OP_TRACK    = 0x0002, // track my networks beacon (netid)
       OP_JOINING  = 0x0004, // device joining in progress (blocks other activities)
       OP_TXDATA   = 0x0008, // TX user data (buffered in pendTxData)
       OP_POLL     = 0x0010, // send empty UP frame to ACK confirmed DN/fetch more DN data
       OP_REJOIN   = 0x0020, // occasionally send JOIN REQUEST
       OP_SHUTDOWN = 0x0040, // prevent MAC from doing anything
       OP_TXRXPEND = 0x0080, // TX/RX transaction pending
       OP_RNDTX    = 0x0100, // prevent TX lining up after a beacon
       OP_PINGINI  = 0x0200, // pingable is initialized and scheduling active
       OP_PINGABLE = 0x0400, // we're pingable
       OP_NEXTCHNL = 0x0800, // find a new channel
       OP_LINKDEAD = 0x1000, // link was reported as dead
       OP_TESTMODE = 0x2000, // developer test mode
       OP_UNJOIN   = 0x4000, // unjoin and rejoin on next engineUpdate().
};
// TX-RX transaction flags - report back to user
enum { TXRX_ACK    = 0x80,   // confirmed UP frame was acked
       TXRX_NACK   = 0x40,   // confirmed UP frame was not acked
       TXRX_NOPORT = 0x20,   // set if a frame with a port was RXed, clr if no frame/no port
       TXRX_PORT   = 0x10,   // set if a frame with a port was RXed, LMIC.frame[LMIC.dataBeg-1] => port
       TXRX_LENERR = 0x08,   // set if frame was discarded due to length error.
       TXRX_PING   = 0x04,   // received in a scheduled RX slot
       TXRX_DNW2   = 0x02,   // received in 2dn DN slot
       TXRX_DNW1   = 0x01,   // received in 1st DN slot
};

// Event types for event callback
enum _ev_t { EV_SCAN_TIMEOUT=1, EV_BEACON_FOUND,
             EV_BEACON_MISSED, EV_BEACON_TRACKED, EV_JOINING,
             EV_JOINED, EV_RFU1, EV_JOIN_FAILED, EV_REJOIN_FAILED,
             EV_TXCOMPLETE, EV_LOST_TSYNC, EV_RESET,
             EV_RXCOMPLETE, EV_LINK_DEAD, EV_LINK_ALIVE, EV_SCAN_FOUND,
             EV_TXSTART, EV_TXCANCELED, EV_RXSTART, EV_JOIN_TXCOMPLETE };
typedef enum _ev_t ev_t;

// this macro can be used to initalize a normal table of event strings
#define LMIC_EVENT_NAME_TABLE__INIT                                         \
    "<<zero>>",                                                             \
    "EV_SCAN_TIMEOUT", "EV_BEACON_FOUND",                                   \
    "EV_BEACON_MISSED", "EV_BEACON_TRACKED", "EV_JOINING",                  \
    "EV_JOINED", "EV_RFU1", "EV_JOIN_FAILED", "EV_REJOIN_FAILED",           \
    "EV_TXCOMPLETE", "EV_LOST_TSYNC", "EV_RESET",                           \
    "EV_RXCOMPLETE", "EV_LINK_DEAD", "EV_LINK_ALIVE", "EV_SCAN_FOUND",      \
    "EV_TXSTART", "EV_TXCANCELED", "EV_RXSTART", "EV_JOIN_TXCOMPLETE"

// if working on an AVR (or worried about it), you can use this multi-zero
// string and put this in a single const F() string.  Index through this
// counting up from 0, until you get to the entry you want or to an
// entry that begins with a \0.
#define LMIC_EVENT_NAME_MULTISZ__INIT                                       \
    "<<zero>>\0"                                                            \
    "EV_SCAN_TIMEOUT\0" "EV_BEACON_FOUND\0"                                 \
    "EV_BEACON_MISSED\0" "EV_BEACON_TRACKED\0" "EV_JOINING\0"               \
    "EV_JOINED\0" "EV_RFU1\0" "EV_JOIN_FAILED\0" "EV_REJOIN_FAILED\0"       \
    "EV_TXCOMPLETE\0" "EV_LOST_TSYNC\0" "EV_RESET\0"                        \
    "EV_RXCOMPLETE\0" "EV_LINK_DEAD\0" "EV_LINK_ALIVE\0" "EV_SCAN_FOUND\0"  \
    "EV_TXSTART\0" "EV_TXCANCELED\0" "EV_RXSTART\0" "EV_JOIN_TXCOMPLETE\0"

enum {
    LMIC_ERROR_SUCCESS = 0,
    LMIC_ERROR_TX_BUSY = -1,
    LMIC_ERROR_TX_TOO_LARGE = -2,
    LMIC_ERROR_TX_NOT_FEASIBLE = -3,
    LMIC_ERROR_TX_FAILED = -4,
};

typedef int lmic_tx_error_t;

#define LMIC_ERROR_NAME__INIT                                               \
    "LMIC_ERROR_SUCCESS",                                                   \
    "LMIC_ERROR_TX_BUSY",                                                   \
    "LMIC_ERROR_TX_TOO_LARGE",                                              \
    "LMIC_ERROR_TX_NOT_FEASIBLE",                                           \
    "LMIC_ERROR_TX_FAILED"

#define LMIC_ERROR_NAME_MULTISZ__INIT                                       \
    "LMIC_ERROR_SUCCESS\0"                                                  \
    "LMIC_ERROR_TX_BUSY\0"                                                  \
    "LMIC_ERROR_TX_TOO_LARGE\0"                                             \
    "LMIC_ERROR_TX_NOT_FEASIBLE\0"                                          \
    "LMIC_ERROR_TX_FAILED"

enum {
    LMIC_BEACON_ERROR_INVALID   = -2,
    LMIC_BEACON_ERROR_WRONG_NETWORK = -1,
    LMIC_BEACON_ERROR_SUCCESS_PARTIAL = 0,
    LMIC_BEACON_ERROR_SUCCESS_FULL = 1,
};

typedef s1_t lmic_beacon_error_t;

static inline bit_t LMIC_BEACON_SUCCESSFUL(lmic_beacon_error_t e) {
    return e < 0;
}

// LMIC_CFG_max_clock_error_ppm
#if !defined(LMIC_CFG_max_clock_error_ppm)
# define LMIC_CFG_max_clock_error_ppm	2000	/* max clock error: 0.2% (2000 ppm) */
#endif


enum {
        // This value represents 100% error in LMIC.clockError
        MAX_CLOCK_ERROR = 65536,
        //! \brief maximum clock error that users can specify: 2000 ppm (0.2%).
        //! \details This is the limit for clock error, unless LMIC_ENABLE_arbitrary_clock_error is set.
        //! The default is 4,000 ppm, which is .004, or 0.4%; this is what you get on an
        //! STM32L0 running with the HSI oscillator after cal. If your clock error is bigger,
        //! usually you want to calibrate it so that millis() and micros() are reasonably
        //! accurate. Important: do not use clock error to compensate for late serving
        //! of the LMIC. If you see that LMIC.radio.rxlate_count is increasing, you need
        //! to adjust your application logic so the LMIC gets serviced promptly when a
        //! Class A downlink (or beacon) is pending.
        LMIC_kMaxClockError_ppm = 4000,
};

// callbacks for client alerts.
// types and functions are always defined, to reduce #ifs in example code and libraries.
typedef void LMIC_ABI_STD lmic_rxmessage_cb_t(void *pUserData, uint8_t port, const uint8_t *pMessage, size_t nMessage);
typedef void LMIC_ABI_STD lmic_txmessage_cb_t(void *pUserData, int fSuccess);
typedef void LMIC_ABI_STD lmic_event_cb_t(void *pUserData, ev_t e);

// network time request callback function
// defined unconditionally, because APIs and types can't change based on config.
// This is called when a time-request succeeds or when we get a downlink
// without time request, "completing" the pending time request.
typedef void LMIC_ABI_STD lmic_request_network_time_cb_t(void *pUserData, int flagSuccess);

// how the network represents time.
typedef u4_t lmic_gpstime_t;

// rather than deal with 1/256 second tick, we adjust ostime back
// (as it's high res) to match tNetwork.
typedef struct lmic_time_reference_s lmic_time_reference_t;

struct lmic_time_reference_s {
    // our best idea of when we sent the uplink (end of packet).
    ostime_t tLocal;
    // the network's best idea of when we sent the uplink.
    lmic_gpstime_t tNetwork;
};

enum lmic_request_time_state_e {
    lmic_RequestTimeState_idle = 0,     // we're not doing anything
    lmic_RequestTimeState_tx,           // we want to tx a time request on next uplink
    lmic_RequestTimeState_rx,           // we have tx'ed, next downlink completes.
    lmic_RequestTimeState_success       // we sucessfully got time.
};

typedef u1_t lmic_request_time_state_t;

enum lmic_engine_update_state_e {
    lmic_EngineUpdateState_idle = 0,    // engineUpdate is idle.
    lmic_EngineUpdateState_busy = 1,    // engineUpdate is busy, but has not been reentered.
    lmic_EngineUpdateState_again = 2,   // engineUpdate is busy, and has to be evaluated again.
};

typedef u1_t lmic_engine_update_state_t;

/*

Structure:  lmic_client_data_t

Function:
        Holds LMIC client data that must live through LMIC_reset().

Description:
        There are a variety of client registration linkage items that
        must live through LMIC_reset(), because LMIC_reset() is called
        at frame rollover time.  We group them together into a structure
        to make copies easy.

*/

//! abstract type for collection of client data that survives LMIC_reset().
typedef struct lmic_client_data_s lmic_client_data_t;

//! contents of lmic_client_data_t
struct lmic_client_data_s {

    /* pointer-width things come first */
#if LMIC_ENABLE_DeviceTimeReq
    lmic_request_network_time_cb_t *pNetworkTimeCb; //! call-back routine for network time
    void        *pNetworkTimeUserData;              //! call-back data for network time.
#endif

#if LMIC_ENABLE_user_events
    lmic_event_cb_t     *eventCb;           //! user-supplied callback function for events.
    void                *eventUserData;     //! data for eventCb
    lmic_rxmessage_cb_t *rxMessageCb;       //! user-supplied message-received callback
    void                *rxMessageUserData; //! data for rxMessageCb
    lmic_txmessage_cb_t *txMessageCb;       //! transmit-complete message handler; reset on each tx complete.
    void                *txMessageUserData; //! data for txMessageCb.
#endif // LMIC_ENABLE_user_events

    /* next we have things that are (u)int32_t */
    /* none at the moment */

    /* next we have things that are (u)int16_t */

    u2_t        clockError;                 //! Inaccuracy in the clock. CLOCK_ERROR_MAX represents +/-100% error

    /* finally, things that are (u)int8_t */
    u1_t        devStatusAns_battery;       //!< value to report in MCMD_DevStatusAns message.
};

/*

Structure:  lmic_radio_data_t

Function:
    Holds LMIC radio driver.

Description:
    Eventually this will be used for all portable things for the radio driver,
    but for now it's where we can start to add things.

*/

typedef struct lmic_radio_data_s lmic_radio_data_t;

struct lmic_radio_data_s {
    // total os ticks of accumulated delay error. Can overflow!
    ostime_t    rxlate_ticks;
    // number of rx late launches.
    unsigned    rxlate_count;
    // total os ticks of accumulated tx delay error. Can overflow!
    ostime_t    txlate_ticks;
    // number of tx late launches.
    unsigned    txlate_count;
};

/*

Structure:  lmic_t

Function:
        Provides the instance data for the LMIC.

*/

struct lmic_t {
    // client setup data, survives LMIC_reset().
    lmic_client_data_t  client;

    // the OS job object. pointer alignment.
    osjob_t     osjob;

#if !defined(DISABLE_BEACONS)
    bcninfo_t   bcninfo;      // Last received beacon info
#endif

#if !defined(DISABLE_PING)
    rxsched_t   ping;         // pingable setup
#endif

    // the radio driver portable context
    lmic_radio_data_t   radio;

    /* (u)int32_t things */

    // Radio settings TX/RX (also accessed by HAL)
    ostime_t    txend;
    ostime_t    rxtime;

    // LBT info
    ostime_t    lbt_ticks;      // ticks to listen

    u4_t        freq;

    ostime_t    globalDutyAvail; // time device can send again

    u4_t        netid;        // current network id (~0 - none)
    devaddr_t   devaddr;
    u4_t        seqnoDn;      // device level down stream seqno
    u4_t        seqnoUp;
    u4_t        dn2Freq;

#if !defined(DISABLE_BEACONS)
    ostime_t    bcnRxtime;
#endif

#if LMIC_ENABLE_DeviceTimeReq
    // put here for alignment, to reduce RAM use.
    ostime_t    localDeviceTime;    // the LMIC.txend value for last DeviceTimeAns
    lmic_gpstime_t netDeviceTime;   // the netDeviceTime for lastDeviceTimeAns
                                    // zero ==> not valid.
#endif // LMIC_ENABLE_DeviceTimeReq

    // Channel scheduling -- very much private
#if CFG_LMIC_EU_like
    band_t      bands[MAX_BANDS];
    u4_t        channelFreq[MAX_CHANNELS];
#if !defined(DISABLE_MCMD_DlChannelReq)
    u4_t        channelDlFreq[MAX_CHANNELS];
#endif
    // bit map of enabled datarates for each channel
    u2_t        channelDrMap[MAX_CHANNELS];
    u2_t        channelMap;
    u2_t        channelShuffleMap;
#elif CFG_LMIC_US_like
    u2_t        channelMap[(72+15)/16];  // enabled bits
    u2_t        channelShuffleMap[(72+15)/16];  // enabled bits
    u2_t        activeChannels125khz;
    u2_t        activeChannels500khz;
#endif

    /* (u)int16_t things */
    rps_t       rps;            // radio parameter selections: SF, BW, CodingRate, NoCrc, implicit hdr
    u2_t        opmode;         // engineUpdate() operating mode flags
    u2_t        devNonce;       // last generated nonce

    s2_t        adrAckReq;      // counter for link integrity tracking (LINK_CHECK_OFF=off)

#if !defined(DISABLE_BEACONS)
    s2_t        drift;          // last measured drift
    s2_t        lastDriftDiff;
    s2_t        maxDriftDiff;
    rxsyms_t    bcnRxsyms;      //
#endif

    /* (u)int8_t things */
    lmic_engine_update_state_t engineUpdateState;   // state of the engineUpdate() evaluator.
    s1_t        rssi;
    s1_t        snr;            // LMIC.snr is SNR times 4
    rxsyms_t    rxsyms;         // symbols for receive timeout.
    u1_t        dndr;
    s1_t        txpow;          // transmit dBm (administrative)
    s1_t        radio_txpow;    // the radio driver's copy of txpow, in dB limited by adrTxPow, and
				// also adjusted for EIRP/antenna gain considerations.
				// This is just the radio's idea of power. So if you are
				// controlling EIRP, and you have 3 dB antenna gain, this
				// needs to reduced by 3 dB.
    s1_t        lbt_dbmax;      // max permissible dB on our channel (eg -80)

    u1_t        txChnl;          // channel for next TX
    u1_t        globalDutyRate;  // max rate: 1/2^k
#if CFG_LMIC_US_like
    u1_t        txChnl_125kHz;  ///< during joins on 500 kHz, the 125 kHz channel
                                ///  that was last used.
#endif
    u1_t        upRepeat;     // configured up repeat
    s1_t        adrTxPow;     // ADR adjusted TX power
    u1_t        datarate;     // current data rate
    u1_t        errcr;        // error coding rate (used for TX only)
    u1_t        rejoinCnt;    // adjustment for rejoin datarate

    u1_t        upRepeatCount;  // current up-repeat
    bit_t       initBandplanAfterReset; // cleared by LMIC_reset(), set by first join. See issue #244

    u1_t        pendTxPort;
    u1_t        pendTxConf;   // confirmed data
    u1_t        pendTxLen;    // count of bytes in pendTxData.
    u1_t        pendTxData[MAX_LEN_PAYLOAD];

    u1_t        pendMacLen;         // number of bytes of pending Mac response data
    bit_t       pendMacPiggyback;   // received on port 0 or piggyback?
    // response data if piggybacked
    u1_t        pendMacData[LWAN_FCtrl_FOptsLen_MAX];

    u1_t        nwkKey[16];   // network session key
    u1_t        artKey[16];   // application router session key

    u1_t        dnConf;       // dn frame confirm pending: LORA::FCT_ACK or 0
    u1_t        lastDnConf;   // downlink with seqnoDn-1 requested confirmation
    u1_t        adrChanged;

    u1_t        rxDelay;      // Rx delay after TX

    u1_t        margin;
    s1_t        devAnsMargin; // SNR value between -32 and 31 (inclusive) for the last successfully received DevStatusReq command
    u1_t        adrEnabled;
    u1_t        moreData;     // NWK has more data pending
#if LMIC_ENABLE_TxParamSetupReq
    u1_t        txParam;        // the saved TX param byte.
#endif
#if LMIC_ENABLE_DeviceTimeReq
    lmic_request_time_state_t txDeviceTimeReqState;  // current state, initially idle.
    u1_t        netDeviceTimeFrac;     // updated on any DeviceTimeAns.
#endif

    // rx1DrOffset is the offset from uplink to downlink datarate
    u1_t        rx1DrOffset;  // captured from join. zero by default.

    // 2nd RX window (after up stream)
    u1_t        dn2Dr;
#if !defined(DISABLE_MCMD_RXParamSetupReq)
    u1_t        dn2Ans;       // 0=no answer pend, 0x80+ACKs
#endif
#if !defined(DISABLE_MCMD_DlChannelReq)
    u1_t        macDlChannelAns;        // 0 ==> no answer pending, 0x80+ACK bits
#endif
#if !defined(DISABLE_MCMD_RXTimingSetupReq)
    bit_t       macRxTimingSetupAns;    // 0 ==> no answer pend, non-zero inserts response.
#endif

    // Class B state
#if !defined(DISABLE_BEACONS)
    u1_t        missedBcns;   // unable to track last N beacons
    u1_t        bcninfoTries; // how often to try (scan mode only)
#endif
    // Public part of MAC state
    u1_t        txCnt;
    u1_t        txrxFlags;  // transaction flags (TX-RX combo)
    u1_t        dataBeg;    // 0 or start of data (dataBeg-1 is port)
    u1_t        dataLen;    // 0 no data or zero length data, >0 byte count of data
    u1_t        frame[MAX_LEN_FRAME];

#if !defined(DISABLE_BEACONS)
    u1_t        bcnChnl;
#endif

    u1_t        noRXIQinversion;
    u1_t        saveIrqFlags;   // last LoRa IRQ flags
};

//! \var struct lmic_t LMIC
//! The state of LMIC MAC layer is encapsulated in this variable.
DECLARE_LMIC; //!< \internal

//! Construct a bit map of allowed datarates from drlo to drhi (both included).
#define DR_RANGE_MAP(drlo,drhi) (((u2_t)0xFFFF<<(drlo)) & ((u2_t)0xFFFF>>(15-(drhi))))
bit_t LMIC_setupBand (u1_t bandidx, s1_t txpow, u2_t txcap);
bit_t LMIC_setupChannel (u1_t channel, u4_t freq, u2_t drmap, s1_t band);
bit_t LMIC_disableChannel (u1_t channel);
bit_t LMIC_enableSubBand(u1_t band);
bit_t LMIC_enableChannel(u1_t channel);
bit_t LMIC_disableSubBand(u1_t band);
bit_t LMIC_selectSubBand(u1_t band);

//! \brief get the number of (fixed) default channels before the programmable channels.
u1_t  LMIC_queryNumDefaultChannels(void);

//! \brief check whether the LMIC is ready for a transmit packet
bit_t LMIC_queryTxReady(void);

void  LMIC_setDrTxpow   (dr_t dr, s1_t txpow);  // set default/start DR/txpow
void  LMIC_setAdrMode   (bit_t enabled);        // set ADR mode (if mobile turn off)

#if !defined(DISABLE_JOIN)
bit_t LMIC_startJoining (void);
void  LMIC_tryRejoin    (void);
void  LMIC_unjoin       (void);
void  LMIC_unjoinAndRejoin (void);
#endif

void  LMIC_shutdown     (void);
void  LMIC_init         (void);
void  LMIC_reset        (void);
void  LMIC_clrTxData    (void);
void  LMIC_setTxData    (void);
void  LMIC_setTxData_strict(void);
lmic_tx_error_t LMIC_setTxData2(u1_t port, xref2u1_t data, u1_t dlen, u1_t confirmed);
lmic_tx_error_t LMIC_setTxData2_strict(u1_t port, xref2u1_t data, u1_t dlen, u1_t confirmed);
lmic_tx_error_t LMIC_sendWithCallback(u1_t port, xref2u1_t data, u1_t dlen, u1_t confirmed, lmic_txmessage_cb_t *pCb, void *pUserData);
lmic_tx_error_t LMIC_sendWithCallback_strict(u1_t port, xref2u1_t data, u1_t dlen, u1_t confirmed, lmic_txmessage_cb_t *pCb, void *pUserData);
void  LMIC_sendAlive    (void);

#if !defined(DISABLE_BEACONS)
bit_t LMIC_enableTracking  (u1_t tryBcnInfo);
void  LMIC_disableTracking (void);
#endif

#if !defined(DISABLE_PING)
void  LMIC_stopPingable  (void);
void  LMIC_setPingable   (u1_t intvExp);
#endif

void LMIC_setSession (u4_t netid, devaddr_t devaddr, xref2u1_t nwkKey, xref2u1_t artKey);
void LMIC_setLinkCheckMode (bit_t enabled);
void LMIC_setClockError(u2_t error);

u4_t LMIC_getSeqnoUp    (void);
u4_t LMIC_setSeqnoUp    (u4_t);
void LMIC_getSessionKeys (u4_t *netid, devaddr_t *devaddr, xref2u1_t nwkKey, xref2u1_t artKey);

void LMIC_requestNetworkTime(lmic_request_network_time_cb_t *pCallbackfn, void *pUserData);
int LMIC_getNetworkTimeReference(lmic_time_reference_t *pReference);

int LMIC_registerRxMessageCb(lmic_rxmessage_cb_t *pRxMessageCb, void *pUserData);
int LMIC_registerEventCb(lmic_event_cb_t *pEventCb, void *pUserData);

int LMIC_findNextChannel(uint16_t *, const uint16_t *, uint16_t, int);

u1_t LMIC_getBatteryLevel(void);
u1_t LMIC_setBatteryLevel(u1_t /* uBattLevel */);

// APIs for client half of compliance.
typedef u1_t lmic_compliance_rx_action_t;

enum lmic_compliance_rx_action_e {
    LMIC_COMPLIANCE_RX_ACTION_PROCESS = 0,  // process this message normally
    LMIC_COMPLIANCE_RX_ACTION_START,        // enter compliance mode, discard this message
    LMIC_COMPLIANCE_RX_ACTION_IGNORE,       // continue in compliance mode, discard this message
    LMIC_COMPLIANCE_RX_ACTION_END           // exit compliance mode, discard this message
};

lmic_compliance_rx_action_t LMIC_complianceRxMessage(u1_t port, const u1_t *pMessage, size_t nMessage);

// Declare onEvent() function, to make sure any definition will have the
// C conventions, even when in a C++ file.
#if LMIC_ENABLE_onEvent
DECL_ON_LMIC_EVENT;
#endif /* LMIC_ENABLE_onEvent */

// Special APIs - for development or testing
// !!!See implementation for caveats!!!

#ifdef __cplusplus
} // extern "C"
#endif

// names for backward compatibility
#include "lmic_compat.h"

#endif // _lmic_h_
