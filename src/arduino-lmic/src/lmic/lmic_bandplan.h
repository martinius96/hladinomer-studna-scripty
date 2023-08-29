/*
* Copyright (c) 2014-2016 IBM Corporation.
* Copyright (c) 2017, 2019-2021 MCCI Corporation.
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

#ifndef _lmic_bandplan_h_
# define _lmic_bandplan_h_

#ifndef _lmic_h_
# include "lmic.h"
#endif

#if defined(CFG_eu868)
# include "lmic_bandplan_eu868.h"
#elif defined(CFG_us915)
# include "lmic_bandplan_us915.h"
#elif defined(CFG_au915)
# include "lmic_bandplan_au915.h"
#elif defined(CFG_as923)
# include "lmic_bandplan_as923.h"
#elif defined(CFG_kr920)
# include "lmic_bandplan_kr920.h"
#elif defined(CFG_in866)
# include "lmic_bandplan_in866.h"
#else
# error "CFG_... not properly set for bandplan"
#endif

// check post-conditions
#ifndef DNW2_SAFETY_ZONE
# error "DNW2_SAFETY_ZONE not defined by bandplan"
#endif

#ifndef LMICbandplan_maxFrameLen
# error "LMICbandplan_maxFrameLen() not defined by bandplan"
#endif

#ifndef pow2dBm
# error "pow2dBm() not defined by bandplan"
#endif

#ifndef dr2hsym
# error "dr2hsym() not defined by bandplan"
#endif

#if !defined(LMICbandplan_isValidBeacon1) && !defined(DISABLE_BEACONS)
# error "LMICbandplan_isValidBeacon1 not defined by bandplan"
#endif

#if !defined(LMICbandplan_isFSK)
# error "LMICbandplan_isFSK() not defined by bandplan"
#endif

#if !defined(LMICbandplan_txDoneFSK)
# error "LMICbandplan_txDoneFSK() not defined by bandplan"
#endif

#if !defined(LMICbandplan_joinAcceptChannelClear)
# error "LMICbandplan_joinAcceptChannelClear() not defined by bandplan"
#endif

#if !defined(LMICbandplan_getInitialDrJoin)
# error "LMICbandplan_getInitialDrJoin() not defined by bandplan"
#endif

#if !defined(LMICbandplan_hasJoinCFlist)
# error "LMICbandplan_hasJoinCFlist() not defined by bandplan"
#endif

#if !defined(LMICbandplan_advanceBeaconChannel)
# error "LMICbandplan_advanceBeaconChannel() not defined by bandplan"
#endif

#if !defined(LMICbandplan_resetDefaultChannels)
# error "LMICbandplan_resetDefaultChannels() not defined by bandplan"
#endif

#if !defined(LMICbandplan_setSessionInitDefaultChannels)
# error "LMICbandplan_setSessionInitDefaultChannels() not defined by bandplan"
#endif

#if !defined(LMICbandplan_setBcnRxParams)
# error "LMICbandplan_setBcnRxParams() not defined by bandplan"
#endif

#if !defined(LMICbandplan_canMapChannels)
# error "LMICbandplan_canMapChannels() not defined by bandplan"
#endif

#if !defined(LMICbandplan_mapChannels)
# error "LMICbandplan_mapChannels() not defined by bandplan"
#endif

#if !defined(LMICbandplan_convFreq)
# error "LMICbandplan_convFreq() not defined by bandplan"
#endif

#if !defined(LMICbandplan_setRx1Params)
# error "LMICbandplan_setRx1Params() not defined by bandplan"
#endif

#if !defined(LMICbandplan_initJoinLoop)
# error "LMICbandplan_initJoinLoop() not defined by bandplan"
#endif

#if !defined(LMICbandplan_nextTx)
# error "LMICbandplan_nextTx() not defined by bandplan"
#endif

#if !defined(LMICbandplan_updateTx)
# error "LMICbandplan_updateTx() not defined by bandplan"
#endif

#if !defined(LMICbandplan_nextJoinState)
# error "LMICbandplan_nextJoinState() not defined by bandplan"
#endif

#if !defined(LMICbandplan_initDefaultChannels)
# error "LMICbandplan_initDefaultChannels() not defined by bandplan"
#endif

#if !defined(LMICbandplan_nextJoinTime)
# error "LMICbandplan_nextJoinTime() not defined by bandplan"
#endif

#if !defined(LMICbandplan_init)
# error "LMICbandplan_init() not defined by bandplan"
#endif

#if !defined(LMICbandplan_saveAdrState)
# error "LMICbandplan_saveAdrState() not defined by bandplan"
#endif

#if !defined(LMICbandplan_compareAdrState)
# error "LMICbandplan_compareAdrState() not defined by bandplan"
#endif

#if !defined(LMICbandplan_restoreAdrState)
# error "LMICbandplan_restoreAdrState() not defined by bandplan"
#endif

#if !defined(LMICbandplan_isDataRateFeasible)
# error "LMICbandplan_isDataRateFeasible() not defined by bandplan"
#endif

#if !defined(LMICbandplan_validDR)
# error "LMICbandplan_validDR() not defined by bandplan"
#endif

#if !defined(LMICbandplan_processJoinAcceptCFList)
# error "LMICbandplan_processJoinAcceptCFList() not defined by bandplan"
#endif

//
// Things common to lmic.c code
//
#define	LMICbandplan_MINRX_SYMS_LoRa_ClassA	6
#define	LMICbandplan_RX_ERROR_ABS_osticks	ms2osticks(10)

// Semtech inherently (by calculating in ms and taking ceilings)
// rounds up to the next higher ms. It's a lot easier for us
// to just add margin for things like hardware ramp-up time
// and clock calibration when running from the LSE and HSI
// clocks on an STM32.
#define LMICbandplan_RX_EXTRA_MARGIN_osticks	us2osticks(2000)

// probably this should be the same as the Class-A value, but
// we have not the means to thoroughly test this. This is the
// number of rxsyms used in the computations for ping and beacon
// windows.
#define	LMICbandplan_MINRX_SYMS_LoRa_ClassB	5

#define LMICbandplan_PAMBL_SYMS 8
#define LMICbandplan_PAMBL_FSK  5
#define LMICbandplan_PRERX_FSK  1
#define LMICbandplan_RXLEN_FSK  (1+5+2)

// Legacy names
#if !defined(MINRX_SYMS)
# define MINRX_SYMS	LMICbandplan_MINRX_SYMS_LoRa_ClassB
#endif // !defined(MINRX_SYMS)
#define PAMBL_SYMS	LMICbandplan_PAMBL_SYMS
#define PAMBL_FSK	LMICbandplan_PAMBL_FSK
#define PRERX_FSK	LMICbandplan_PRERX_FSK
#define	RXLEN_FSK	LMICbandplan_RXLEN_FSK

// this is regional, but so far all regions are the same
#if !defined(LMICbandplan_MAX_FCNT_GAP)
# define LMICbandplan_MAX_FCNT_GAP  16384
#endif // !defined LWAN_MAX_FCNT_GAP

// this is probably regional, but for now default can be the same
#if !defined(LMICbandplan_TX_RECOVERY_ms)
# define LMICbandplan_TX_RECOVERY_ms    500
#endif

#define BCN_INTV_osticks       sec2osticks(BCN_INTV_sec)
#define TXRX_GUARD_osticks     ms2osticks(TXRX_GUARD_ms)
#define JOIN_GUARD_osticks     ms2osticks(JOIN_GUARD_ms)
#define DELAY_JACC1_osticks    sec2osticks(DELAY_JACC1)
#define DELAY_JACC2_osticks    sec2osticks(DELAY_JACC2)
#define DELAY_EXTDNW2_osticks  sec2osticks(DELAY_EXTDNW2)
#define BCN_RESERVE_osticks    ms2osticks(BCN_RESERVE_ms)
#define BCN_GUARD_osticks      ms2osticks(BCN_GUARD_ms)
#define BCN_WINDOW_osticks     ms2osticks(BCN_WINDOW_ms)
#define AIRTIME_BCN_osticks    us2osticks(AIRTIME_BCN)

// Special APIs - for development or testing
#define isTESTMODE() 0

// internal APIs
ostime_t LMICcore_rndDelay(u1_t secSpan);
void LMICcore_setDrJoin(u1_t reason, u1_t dr);
ostime_t LMICcore_adjustForDrift(ostime_t delay, ostime_t hsym, rxsyms_t rxsyms_in);

// this has been exported to clients forever by lmic.h. including lorabase.h;
// but with multiband lorabase can't really safely do this; it's really an LMIC-ism.
// As are the rest of the static inlines..

///< \brief return non-zero if given DR is valid for this region.
static inline bit_t validDR  (dr_t dr) { return LMICbandplan_validDR(dr); } // in range

///< \brief region-specific table mapping DR to RPS/CRC bits; index by dr+1
extern CONST_TABLE(u1_t, _DR2RPS_CRC)[];

static inline rps_t updr2rps (dr_t dr) { return (rps_t)TABLE_GET_U1(_DR2RPS_CRC, dr+1); }
static inline rps_t dndr2rps (dr_t dr) { return setNocrc(updr2rps(dr),1); }
static inline dr_t  incDR    (dr_t dr) { return TABLE_GET_U1(_DR2RPS_CRC, dr+2)==ILLEGAL_RPS ? dr : (dr_t)(dr+1); } // increase data rate
static inline dr_t  decDR    (dr_t dr) { return TABLE_GET_U1(_DR2RPS_CRC, dr  )==ILLEGAL_RPS ? dr : (dr_t)(dr-1); } // decrease data rate
static inline dr_t  assertDR (dr_t dr) { return TABLE_GET_U1(_DR2RPS_CRC, dr+1)==ILLEGAL_RPS ? (dr_t)DR_DFLTMIN : dr; }   // force into a valid DR
static inline dr_t  lowerDR  (dr_t dr, u1_t n) { while(n--){dr=decDR(dr);} return dr; } // decrease data rate by n steps


#endif // _lmic_bandplan_h_
